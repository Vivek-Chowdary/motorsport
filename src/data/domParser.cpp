/*
* The Apache Software License, Version 1.1
*
* Copyright (c) 1999-2002 The Apache Software Foundation.  All rights
* reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*
* 3. The end-user documentation included with the redistribution,
*    if any, must include the following acknowledgment:
*       "This product includes software developed by the
*        Apache Software Foundation (http://www.apache.org/)."
*    Alternately, this acknowledgment may appear in the software itself,
*    if and wherever such third-party acknowledgments normally appear.
*
* 4. The names "Xerces" and "Apache Software Foundation" must
*    not be used to endorse or promote products derived from this
*    software without prior written permission. For written
*    permission, please contact apache\@apache.org.
*
* 5. Products derived from this software may not be called "Apache",
*    nor may "Apache" appear in their name, without prior written
*    permission of the Apache Software Foundation.
*
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
* ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
* USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
* ====================================================================
*
* This software consists of voluntary contributions made by many
* individuals on behalf of the Apache Software Foundation, and was
* originally based on software copyright (c) 1999, International
* Business Machines, Inc., http://www.ibm.com .  For more information
* on the Apache Software Foundation, please see
* <http://www.apache.org/>.
*/

#include "domParser.hpp"
// ---------------------------------------------------------------------------
//   main
// ---------------------------------------------------------------------------
int processXmlFile (char *configFileName, void (*processFunction) (DOMNode *, void *data), void *data)
{
    // ///////////////////////////////////////////////////////////////////////////////////////////////
    AbstractDOMParser::ValSchemes valScheme = AbstractDOMParser::Val_Auto;
//    bool doNamespaces = false;
//    bool doSchema = false;
//    bool schemaFullChecking = false;
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

//    parser->setFeature (XMLUni::fgDOMNamespaces, doNamespaces);
//    parser->setFeature (XMLUni::fgXercesSchema, doSchema);
//    parser->setFeature (XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);

    if (valScheme == AbstractDOMParser::Val_Auto)
    {
//        parser->setFeature (XMLUni::fgDOMValidateIfSchema, true);
    } else if (valScheme == AbstractDOMParser::Val_Never)
    {
//        parser->setFeature (XMLUni::fgDOMValidation, false);
    } else if (valScheme == AbstractDOMParser::Val_Always)
    {
//        parser->setFeature (XMLUni::fgDOMValidation, true);
    }
    // enable datatype normalization - default is off
//    parser->setFeature (XMLUni::fgDOMDatatypeNormalization, true);
    // And create our error handler and install it
    DOMCountErrorHandler errorHandler;

    parser->setErrorHandler (&errorHandler);
    // ///////////////////////////////////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////////////////////////////////
    // Get the starting time and kick off the parse of the indicated
    // file. Catch any exceptions that might propogate out of it.
    XERCES_STD_QUALIFIER ifstream fin;
    // reset error count first
    errorHandler.resetErrors ();
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

    try
    {
        // reset document pool
        parser->resetDocumentPool ();
        doc = parser->parseURI (configFileName);
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
