/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "xmlParser.hpp"

DOMCountErrorHandler::DOMCountErrorHandler () :fSawErrors (false) {}
DOMCountErrorHandler::~DOMCountErrorHandler()                     {}
void DOMCountErrorHandler::resetErrors () { fSawErrors = false; }
bool DOMCountErrorHandler::handleError (const DOMError & domError)
{
    fSawErrors = true;
    short errsev = domError.getSeverity ();
    if (errsev == DOMError::DOM_SEVERITY_WARNING)     std::cout << "(XmlParser)" << "Warning at file: ";
    if (errsev == DOMError::DOM_SEVERITY_ERROR)       std::cout << "(XmlParser)" << "Error at file: ";
    if (errsev == DOMError::DOM_SEVERITY_FATAL_ERROR) std::cout << "(XmlParser)" << "Fatal error at file: ";
    std::cout << StrX (domError.getLocation ()->getURI ()) << ", line " << domError.getLocation ()->getLineNumber () << ", char " << domError.getLocation ()->getColumnNumber () << std::endl << "Message: " << StrX (domError.getMessage ()) << std::endl;
    return true;
}
XmlFile::XmlFile (std::string xmlFileName)
{
    //std::cout << "Loading " << xmlFileName << std::endl;
    error = false;
    bool recognize = false;
    try
    {
        XMLPlatformUtils::Initialize ();
        if (recognize) XMLPlatformUtils::recognizeNEL (recognize);
    }
    catch (const XMLException & toCatch)
    {
        std::cout << "(XmlParser)" << "Error during initialization! : " << StrX (toCatch.getMessage ()) << std::endl;
        error = true;
    }
    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation (gLS);
    parser = ((DOMImplementationLS *) impl)->createDOMBuilder (DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    DOMCountErrorHandler errorHandler;
    parser->setErrorHandler (&errorHandler);
    std::ifstream fin;
    errorHandler.resetErrors ();
    doc = 0;
#ifdef WIN32
#else
    parser->setFeature (XMLUni::fgDOMNamespaces, false);
    parser->setFeature (XMLUni::fgXercesSchema, false);
    parser->setFeature (XMLUni::fgXercesSchemaFullChecking, false);
    parser->setFeature (XMLUni::fgDOMDatatypeNormalization, false);
    parser->setFeature (XMLUni::fgDOMValidateIfSchema, false);
#endif
    try
    {
        parser->resetDocumentPool ();
        doc = parser->parseURI (xmlFileName.c_str());
    }
    catch (const XMLException & toCatch)
    {
        std::cout << "(XmlParser)" << "Error during parsing: " << xmlFileName << std::endl << "Exception message is: " << StrX (toCatch.getMessage ()) << std::endl;
        error = true;
    }
    catch (const DOMException & toCatch)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];
        std::cout << "(XmlParser)" << "DOM Error during parsing: " << xmlFileName << std::endl << "DOMException code is: " << toCatch.code << std::endl;
        if (DOMImplementation::loadDOMExceptionMsg (toCatch.code, errText, maxChars)) std::cout << "(XmlParser)" << "Message is: " << StrX (errText) << std::endl;
        error = true;
    }
    catch (...)
    {
        std::cout << "(XmlParser)" << "Unexpected exception during parsing: " << xmlFileName << std::endl;
        error = true;
    }
    if (!error)
    {
        if (errorHandler.getSawErrors ())
        {
            std::cout << "(XmlParser)" << "Errors occurred, no output available" << std::endl;
            error = true;
        }
    }
}

DOMNode *XmlFile::getRootNode ()
{
    DOMNode *returnNode = 0;
    if (!error) if (doc) returnNode = (DOMNode *) doc->getDocumentElement ();
    return returnNode;
}

XmlFile::~XmlFile ()
{
    parser->release ();
    XMLPlatformUtils::Terminate ();
}

void assignXmlString (std::string & destString, const XMLCh * srcXmlString)
{
    char *tmpString = XMLString::transcode (srcXmlString);
    destString.assign (tmpString);
    XMLString::release (&tmpString);
}

double stod (const std::string & srcString)
{
    std::stringstream tmpString (srcString);
    double tmpInt;
    tmpString >> tmpInt;
    return tmpInt;
}

int stoi (const std::string & srcString)
{
    std::stringstream tmpString (srcString);
    int tmpInt;
    tmpString >> tmpInt;
    return tmpInt;
}

bool stob (const std::string & srcString)
{
    if (srcString == "false" || srcString == "0") return false;
    return true;
}


