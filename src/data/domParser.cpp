#include "domParser.hpp"
// ---------------------------------------------------------------------------
//   main
// ---------------------------------------------------------------------------
int processXmlFile (char *configFileName, void (*processFunction) (DOMNode *, void *data), void *data)
{
    // ///////////////////////////////////////////////////////////////////////////////////////////////
    AbstractDOMParser::ValSchemes valScheme = AbstractDOMParser::Val_Auto;
    bool doNamespaces = false;
    bool doSchema = false;
    bool schemaFullChecking = false;
    bool errorOccurred = false;
    bool recognizeNEL = false;

//                valScheme = AbstractDOMParser::Val_Never;
//                valScheme = AbstractDOMParser::Val_Auto;
//                valScheme = AbstractDOMParser::Val_Always;
//            doNamespaces = true;
//            doSchema = true;
//            schemaFullChecking = true;
//            recognizeNEL = true;

    // There should be only one and only one parameter left, and that should be the file name.
    // ///////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize the XML4C system
    try
    {
        // XMLPlatformUtils::Initialize ( localeStr );
        XMLPlatformUtils::Initialize ();
        if (recognizeNEL)
        {
            XMLPlatformUtils::recognizeNEL (recognizeNEL);
        }
    }
    catch (const XMLException & toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n" << StrX (toCatch.getMessage ()) << XERCES_STD_QUALIFIER endl;
        return 1;
    }
    // ///////////////////////////////////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////////////////////////////////
    // Instantiate the DOM parser.
    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation (gLS);
    DOMBuilder *parser = ((DOMImplementationLS *) impl)->createDOMBuilder (DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    parser->setFeature (XMLUni::fgDOMNamespaces, doNamespaces);
    parser->setFeature (XMLUni::fgXercesSchema, doSchema);
    parser->setFeature (XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);

    if (valScheme == AbstractDOMParser::Val_Auto)
    {
        parser->setFeature (XMLUni::fgDOMValidateIfSchema, true);
    } else if (valScheme == AbstractDOMParser::Val_Never)
    {
        parser->setFeature (XMLUni::fgDOMValidation, false);
    } else if (valScheme == AbstractDOMParser::Val_Always)
    {
        parser->setFeature (XMLUni::fgDOMValidation, true);
    }
    // enable datatype normalization - default is off
    parser->setFeature (XMLUni::fgDOMDatatypeNormalization, true);
    // And create our error handler and install it
    DOMCountErrorHandler errorHandler;

    parser->setErrorHandler (&errorHandler);
    // ///////////////////////////////////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////////////////////////////////
    // Get the starting time and kick off the parse of the indicated
    // file. Catch any exceptions that might propogate out of it.
    unsigned long duration;

    XERCES_STD_QUALIFIER ifstream fin;
    // reset error count first
    errorHandler.resetErrors ();
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

    try
    {
        // reset document pool
        parser->resetDocumentPool ();
        const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis ();
        doc = parser->parseURI (configFileName);
        const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis ();
        duration = endMillis - startMillis;
    } catch (const XMLException & toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << configFileName << "'\n" << "Exception message is:  \n" << StrX (toCatch.getMessage ()) << "\n" << XERCES_STD_QUALIFIER endl;
        errorOccurred = true;
    } catch (const DOMException & toCatch)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];
        XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << configFileName << "'\n" << "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;
        if (DOMImplementation::loadDOMExceptionMsg (toCatch.code, errText, maxChars))
        {
            XERCES_STD_QUALIFIER cerr << "Message is: " << StrX (errText) << XERCES_STD_QUALIFIER endl;
        }
        errorOccurred = true;
    }
    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << configFileName << "'\n";
        errorOccurred = true;
    }
    if (!errorOccurred)
    {
        // Extract the DOM tree, get the list of all the elements and report the
        // length as the count of elements.
        if (errorHandler.getSawErrors ())
        {
            XERCES_STD_QUALIFIER cout << "\nErrors occurred, no output available\n" << XERCES_STD_QUALIFIER endl;
            errorOccurred = true;
        } else
        {
            if (doc)
            {
                (*processFunction) ((DOMNode *) doc->getDocumentElement (), data);
            }
            // Print out the stats that we collected and time taken.
            XERCES_STD_QUALIFIER cout << configFileName << ": " << duration << " ms." << XERCES_STD_QUALIFIER endl;
        }
    }
    // ///////////////////////////////////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////////////////////////////////
    // Delete the parser itself.  Must be done prior to calling Terminate, below.
    parser->release ();
    // ///////////////////////////////////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////////////////////////////////
    // And call the termination method
    XMLPlatformUtils::Terminate ();
    if (errorOccurred)
        return 4;
    else
        return 0;
    // ///////////////////////////////////////////////////////////////////////////////////////////////
}

DOMCountErrorHandler::DOMCountErrorHandler ():

fSawErrors (false)
{
}

DOMCountErrorHandler::~DOMCountErrorHandler ()
{
}

// ---------------------------------------------------------------------------
//  DOMCountHandlers: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
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
