#ifndef DOM_PARSER_HPP
#   define DOM_PARSER_HPP
#include "OgreNoMemoryMacros.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <string.h>
#include <stdlib.h>
#if defined(XERCES_NEW_IOSTREAMS)
#    include <fstream>
#else
#    include <fstream.h>
#endif
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#if defined(XERCES_NEW_IOSTREAMS)
#    include <iostream>
#else
#    include <iostream.h>
#endif
#include <sstream>
#include <string>

XERCES_CPP_NAMESPACE_USE
class DOMCountErrorHandler:public DOMErrorHandler
{
  public:
    DOMCountErrorHandler ();
    ~DOMCountErrorHandler ();
    bool getSawErrors () const;
    bool handleError (const DOMError & domError);
    void resetErrors ();
  private:
    DOMCountErrorHandler (const DOMCountErrorHandler &);
    void operator= (const DOMCountErrorHandler &);
    bool fSawErrors;
};
class StrX
{
  public:
    StrX (const XMLCh * const toTranscode)
    {
        fLocalForm = XMLString::transcode (toTranscode);
    }
     ~StrX ()
    {
        XMLString::release (&fLocalForm);
    }
    const char *localForm () const
    {
        return fLocalForm;
    }

  private:
    char *fLocalForm;
};

inline XERCES_STD_QUALIFIER ostream & operator<< (XERCES_STD_QUALIFIER ostream & target, const StrX & toDump)
{
    target << toDump.localForm ();
    return target;
}

inline bool DOMCountErrorHandler::getSawErrors () const
{
    return fSawErrors;
}

class XmlFile
{
    bool errorOccurred;
    DOMBuilder * parser;
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc;
    public:
        XmlFile (char * xmlFileName);
        ~XmlFile ();
        DOMNode * getRootNode();
};

void assignXmlString (std::string & destString, const XMLCh * srcXmlString);
int stoi (const std::string &srcString);
bool stob (const std::string &srcString);
double stod (const std::string &srcString);
#endif
