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
    if (domError.getSeverity () == DOMError::DOM_SEVERITY_WARNING)
        XERCES_STD_QUALIFIER cerr << "\nWarning at file ";
    else if (domError.getSeverity () == DOMError::DOM_SEVERITY_ERROR)
        XERCES_STD_QUALIFIER cerr << "\nError at file ";
    else
        XERCES_STD_QUALIFIER cerr << "\nFatal Error at file ";
    XERCES_STD_QUALIFIER cerr << StrX (domError.getLocation ()->getURI ()) << ", line " << domError.getLocation ()->getLineNumber () << ", char " << domError.getLocation ()->getColumnNumber () << "\n  Message: " << StrX (domError.getMessage ()) << XERCES_STD_QUALIFIER endl;
    return true;
}

void DOMCountErrorHandler::resetErrors ()
{
    fSawErrors = false;
}

XmlFile::XmlFile (char *xmlFileName)
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
        XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n" << StrX (toCatch.getMessage ()) << XERCES_STD_QUALIFIER endl;
        errorOccurred = true;
    }

    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation (gLS);
    parser = ((DOMImplementationLS *) impl)->createDOMBuilder (DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    DOMCountErrorHandler errorHandler;

    parser->setErrorHandler (&errorHandler);
    XERCES_STD_QUALIFIER ifstream fin;
    errorHandler.resetErrors ();
    doc = 0;

    try
    {
        parser->resetDocumentPool ();
        doc = parser->parseURI (xmlFileName);
    } catch (const XMLException & toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xmlFileName << "'\n" << "Exception message is:  \n" << StrX (toCatch.getMessage ()) << "\n" << XERCES_STD_QUALIFIER endl;
        errorOccurred = true;
    } catch (const DOMException & toCatch)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];
        XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << xmlFileName << "'\n" << "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;
        if (DOMImplementation::loadDOMExceptionMsg (toCatch.code, errText, maxChars))
        {
            XERCES_STD_QUALIFIER cerr << "Message is: " << StrX (errText) << XERCES_STD_QUALIFIER endl;
        }
        errorOccurred = true;
    }
    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << xmlFileName << "'\n";
        errorOccurred = true;
    }
    if (!errorOccurred)
    {
        if (errorHandler.getSawErrors ())
        {
            XERCES_STD_QUALIFIER cout << "\nErrors occurred, no output available\n" << XERCES_STD_QUALIFIER endl;
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
    char * tmpString = XMLString::transcode(srcXmlString);
    destString.assign(tmpString);
    XMLString::release (&tmpString);
}

int stoi (const std::string &srcString)
{
    std::stringstream tmpString(srcString);
    int tmpInt;
    tmpString >> tmpInt;
    return tmpInt;
}

bool stob (const std::string &srcString)
{
    if (srcString == "true")
        return true;
    if (srcString == "false" || srcString == "0")
        return false;
    return true;
}
