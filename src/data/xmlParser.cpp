/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "xmlParser.hpp"

DOMCountErrorHandler::DOMCountErrorHandler ():

fSawErrors (false)
{
}

DOMCountErrorHandler::~DOMCountErrorHandler ()
{
}

bool DOMCountErrorHandler::handleError (const DOMError & domError)
{
    fSawErrors = true;
    short errsev = domError.getSeverity ();

    if (errsev == DOMError::DOM_SEVERITY_WARNING)
        std::cout << "(XML Parser)" << "Warning at file: ";
    if (errsev == DOMError::DOM_SEVERITY_ERROR)
        std::cout << "(XML Parser)" << "Error at file: ";
    if (errsev == DOMError::DOM_SEVERITY_FATAL_ERROR)
        std::cout << "(XML Parser)" << "Fatal error at file: ";

    std::cout << StrX (domError.getLocation ()->getURI ()) << ", line " << domError.getLocation ()->getLineNumber () << ", char " << domError.getLocation ()->getColumnNumber () << std::endl;
    std::cout << "Message: " << StrX (domError.getMessage ()) << std::endl;
    return true;
}

void DOMCountErrorHandler::resetErrors ()
{
    fSawErrors = false;
}

XmlFile::XmlFile (const char *xmlFileName)
{
    errorOccurred = false;
    bool recognizeNEL = false;
    try
    {
        XMLPlatformUtils::Initialize ();
        if (recognizeNEL)
        {
            XMLPlatformUtils::recognizeNEL (recognizeNEL);
        }
    }
    catch (const XMLException & toCatch)
    {
        std::cout << "(XML Parser)" << "Error during initialization! : " << StrX (toCatch.getMessage ()) << std::endl;
        errorOccurred = true;
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
/*    parser->setFeature (XMLUni::fgDOMNamespaces, false);
    parser->setFeature (XMLUni::fgXercesSchema, false);
    parser->setFeature (XMLUni::fgXercesSchemaFullChecking, false);
    parser->setFeature (XMLUni::fgDOMDatatypeNormalization, false);
    parser->setFeature (XMLUni::fgDOMValidateIfSchema, false);*/
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
        doc = parser->parseURI (xmlFileName);
    } catch (const XMLException & toCatch)
    {
        std::cout << "(XML Parser)" << "Error during parsing: " << xmlFileName << std::endl << "Exception message is: " << StrX (toCatch.getMessage ()) << std::endl;
        errorOccurred = true;
    } catch (const DOMException & toCatch)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];
        std::cout << "(XML Parser)" << "DOM Error during parsing: " << xmlFileName << std::endl << "DOMException code is: " << toCatch.code << std::endl;
        if (DOMImplementation::loadDOMExceptionMsg (toCatch.code, errText, maxChars))
        {
            std::cout << "(XML Parser)" << "Message is: " << StrX (errText) << std::endl;
        }
        errorOccurred = true;
    }
    catch (...)
    {
        std::cout << "(XML Parser)" << "Unexpected exception during parsing: " << xmlFileName << std::endl;
        errorOccurred = true;
    }
    if (!errorOccurred)
    {
        if (errorHandler.getSawErrors ())
        {
            std::cout << "(XML Parser)" << "Errors occurred, no output available" << std::endl;
            errorOccurred = true;
        }
    }
}

DOMNode *XmlFile::getRootNode ()
{
    DOMNode *returnNode = 0;
    if (!errorOccurred)
    {
        if (doc)
        {
            returnNode = (DOMNode *) doc->getDocumentElement ();
        }
    }
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
    if (srcString == "false" || srcString == "0")
        return false;
    return true;
}

Vector3d stov3d (const std::string &srcString)
{
    std::stringstream tmpString (srcString);
    double tmpX, tmpY, tmpZ;
    tmpString >> tmpX >> tmpY >> tmpZ;
    return Vector3d(tmpX, tmpY, tmpZ);
}

Quaternion stoq (const std::string &srcString)
{
    std::stringstream tmpString (srcString);
    const double magicValue = 0.123456789101112131415;
    double tmpW, tmpX, tmpY, tmpZ = magicValue;
    tmpString >> tmpW >> tmpX >> tmpY >> tmpZ;
    if ( tmpZ == magicValue )
    {
        return Quaternion (tmpW, tmpX, tmpY);   
    } else {
        return Quaternion (tmpW, tmpX, tmpY, tmpZ);
    }
}
