#!BPY

"""
Name: 'Blend2MospArea exporter'
Blender: 237
Group: 'Export'
Tooltip: 'Exports the current scene to a Motorsport area'
"""

__author__ = ['Bruno Gonzalez (STenyaK)']
__version__ = '1.0.0'
__url__ = ['Motorsport website, http://www.motorsport-sim.org',
	'Motorsport forum, http://forum.rscnet.org/forumdisplay.php?f=577']
__bpydoc__ = """\
Exports the current scene to a Motorsport area.
"""

import os
import ogreexport
import ogredotscene

class Blend2MospAreaApplication:
    """Motorsport Exporter
    """
    def __init__(self):
        print "Motorsport Exporter initiated."
        self.setOgreXmlConverterApp ('OgreXMLConverter')
        self.setExportPath(ogreexport.os.path.dirname(ogreexport.Blender.Get('filename'))+"/export")
        self.scene = ogreexport.Blender.Scene.GetCurrent().getName()

    def setXSLT (self, path):
        self.xslt = path

    def setIndenter (self, path):
        self.indenter = path

    def setExportPath(self, path):
        self.path = path
        return

    def setOgreXmlConverterApp(self, path):
        self.ogreXmlConverterApp = path
        return

    def exportAll(self):
        print "Exporting Motorsport area..."
        try:
            os.mkdir(self.path)
        except:
            print "Error while trying to create dir"
        self.exportMeshes()
        self.exportScene()
        self.exportTextures()
        self.transformScene()
        return

    def cleanAll(self):
        try:
            os.remove(self.path+"/*mesh.xml")
        except:
            print "Error while trying to remove mesh.xml files"
        try:
            os.remove("OgreXMLConverter.log")
        except:
            print "Error while trying to remove OgreXMLConverter log file"
        try:
            os.remove(self.path+"/"+self.scene+".xml")
        except:
            print "Error while trying to remove "+self.scene+".xml files"
        return

    def exportMeshes(self):
        ogreexport.OGRE_XML_CONVERTER = self.ogreXmlConverterApp
        ogreexport.refreshGUI()
        ogreexport.pathString = ogreexport.Draw.Create ( self.path )
        ogreexport.export(ogreexport.selectedObjectsList)
        return

    def exportScene(self):
        dsApp = ogredotscene.DotSceneExporterApplication()
        dsApp.sceneExporter.settings.path = self.path
        dsApp.sceneExporter.settings.doProperties = 1
        dsApp.sceneExporter.export()
        return

    def exportTextures(self):
        print "exportTextures() not coded yet."
        return
    
    def transformScene(self):
        os.system("pwd")
        os.system("xsltproc " +self.xslt+ " " +self.path+ "/" +self.scene+ ".xml > " +self.path+ "/area.xml.tmp")
        os.system(self.indenter+ " " +self.path+ "/area.xml.tmp " +self.path+ "/area.xml")
        return


######
# Main
######
if (__name__ == "__main__"):
    application = Blend2MospAreaApplication()
    application.setOgreXmlConverterApp ('OgreXMLConverter')
    application.setXSLT ("/home/stenyak/dev/motorsport/tools/blend2mosparea/blend2mosparea.xsl")
    application.setIndenter ("/home/stenyak/dev/motorsport/tools/xmlIndenter/xmlIndenter.sh")
    application.setExportPath(ogreexport.os.path.dirname(ogreexport.Blender.Get('filename'))+"/export")
    application.exportAll()
    #application.cleanAll()
    print "###### Done! Area exported! :) ######"
