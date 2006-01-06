/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "xmlTag.hpp"
#include "SDL.h"

XmlTag::XmlTag(std::string xmlFilePath)
{
    initTime = SDL_GetTicks()/1000.0;
    this->filePath = xmlFilePath;
    XmlFile * xmlFile = new XmlFile (filePath);
    construct(NULL, xmlFile->getRootNode(), true);
    delete xmlFile;
}
XmlTag::XmlTag(std::string xmlFilePath, bool logging)
{
    initTime = SDL_GetTicks()/1000.0;
    this->filePath = xmlFilePath;
    XmlFile * xmlFile = new XmlFile (filePath);
    construct(NULL, xmlFile->getRootNode(), logging);
    delete xmlFile;
}
XmlTag::XmlTag(XmlTag * parent, DOMNode * n)
{
    this->filePath = "";
    construct(parent, n, true);
}
XmlTag::XmlTag(XmlTag * parent, DOMNode * n, bool logging)
{
    this->filePath = "";
    construct(parent, n, logging);
}
void XmlTag::construct(XmlTag * parent, DOMNode * n, bool logging)
{
    this->parent = parent;
    this->name = "unknown";
    if (parent == NULL)
    {
        if (logging)
        {
            this->log = new LogEngine (LOG_DEVELOPER, "XmlTag");
        }
        else
        {
           this->log = NULL;
        }
    }
    else
    {
        if (logging)
        {
            this->log = parent->log;
        }
    }

    if (n == NULL)
    {
        if (logging)
        {
           log->__format(LOG_ERROR, "Empty tag (%s)!", getFullName().c_str());
        }
    }
    if (n->getNodeType() != DOMNode::ELEMENT_NODE)
    {
        if (logging)
        {
           log->__format(LOG_ERROR, "This (%s) is not a tag!", getFullName().c_str());
        }
    }
    assignXmlString (name, n->getNodeName());

    //fill tag attributes map
    if (!n->hasAttributes())
    {
        if (logging)
        {
            log->__format(LOG_WARNING, "No attributes in this tag (%s)!", getFullName().c_str() );
        }
    }
    else
    {
        std::string attributeName = "";
        std::string attributeData = "";
        DOMNamedNodeMap *attList = n->getAttributes ();
        int size = attList->getLength ();
        for (int i = 0; i < size; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            assignXmlString (attributeName, attNode->getName ());
            assignXmlString (attributeData, attNode->getValue ());
            if ( (attributeName != "") && (attributeData != "") )
            {
                attributes[attributeName] = attributeData;
                attributesRead[attributeName] = false;
            }
        }
    }
    //fill child tags map
    if (n->hasChildNodes())
    {
        std::string childTagName;
        for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
        {
            if (n) if (n->getNodeType() == DOMNode::ELEMENT_NODE)
            {
                assignXmlString (childTagName, n->getNodeName());
                XmlTag * tmpTag = new XmlTag(this, n);
                tags.push_back(tmpTag);
            }
        }
    }
}
XmlTag::~XmlTag()
{
    for (AttributesBoolIt i = attributesRead.begin(); i != attributesRead.end(); i++)
    {
        if (!(i->second))
        {
            if (log)
            {
                log->__format(LOG_WARNING, "Attribute \"%s\" in \"%s\" has not been used!", i->first.c_str(), getFullName().c_str());
            }
            else
            {
                std::cout << std::endl << " Warning! Attribute \"" << i->first << "\" in \"" << getFullName() << "\" has not been used!";
            }
        }
    }

    for (TagsIt i = tags.begin(); i != tags.end(); i++)
    {
        delete (*i);
    }
    tags.clear();
    if (parent == NULL) 
    {
        if (log)
        {
            log->__format(LOG_ENDUSER, "\"%s\" parsed in %f seconds.", filePath.c_str(), SDL_GetTicks()/1000.0 - initTime);
            delete log;
        }
    }
    log = NULL;
    parent = NULL;
}
std::string XmlTag::getAttribute(std::string attributeName)
{
    if (attributes.find(attributeName) == attributes.end())
    {
        if (log)
        {
            log->__format(LOG_ERROR, "Attempted to read non-existent attribute \"%s\" in tag \"%s\"", attributeName.c_str(), getFullName().c_str());
        }
        else
        {
            std::cout << std::endl << " Error! Attempted to read non-existent attribute \"" << attributeName << "\" in tag \"" << getFullName() << "\".";
        }
        return "";
    }
    attributesRead[attributeName] = true;
    return attributes[attributeName];
}
int XmlTag::nTags()
{
    return tags.size();
}
XmlTag * XmlTag::getTag(int tagNumber)
{
    if (tagNumber > (nTags()-1) || tagNumber < 0)
    {
        //if (logging) log->__format(LOG_ERROR, "Attempted to read non-existent tag #%s in tag \"%s\"", tagNumber, getFullName().c_str());
        return NULL;
    }
    return tags[tagNumber];
}
std::string XmlTag::getFullName()
{
    std::string result = "";
    if (parent == NULL) result = filePath + ": ";
    else result = parent->getFullName();
    return result + "/" + name;
}
std::string XmlTag::getName()
{
    return name;
}
std::string XmlTag::getFilePath()
{
    if (parent == NULL) return filePath;
    return parent->getFilePath();
}
