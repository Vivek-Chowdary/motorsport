// A fake gui engine so we can link logEngine.o

#include "guiEngine.hpp"
#include "xmlParser.hpp"

GuiEngine *GuiEngine::guiEnginePointer = NULL;
GuiEngine *GuiEngine::getGuiEnginePointer ()
{
    return (guiEnginePointer);
}

GuiEngine::GuiEngine ()
{
    if (guiEnginePointer != 0)
    {
        delete this;
    } else {
      guiEnginePointer = this;
    }
}

GuiEngine::~GuiEngine ()
{
  guiEnginePointer = 0;
}

int GuiEngine::computeStep (void)
{
  return 0;
}
void GuiEngine::showLoadscreen (){}
void GuiEngine::updateStatistics (void){}
void GuiEngine::addTelemetryLine (const std::string & line)
{
  telemetryText = line;
}
void GuiEngine::addLoadscreenLine (const std::string & line)
{
  loadscreenText = line;
}
//void GuiEngine::processXmlRootNode (DOMNode * n){}
