#!BPY

"""
Name: 'Blend2MospVehicle exporter'
Blender: 237
Group: 'Export'
Tooltip: 'Exports the current scene to a Motorsport vehicle'
"""

__author__ = ['Bruno Gonzalez (STenyaK)']
__version__ = '1.0.0'
__url__ = ['Motorsport website, http://www.motorsport-sim.org',
	'Motorsport forum, http://forum.rscnet.org/forumdisplay.php?f=577']
__bpydoc__ = """\
Exports the current scene to a Motorsport vehicle.
"""

import os
import ogreexport
import ogredotscene

class Blend2MospVehicleApplication:
    """Motorsport Exporter
    """
    def __init__(self):
        print "Motorsport Exporter initiated."
        self.setOgreXmlConverterApp ('OgreXMLConverter')
        self.setExportPath(ogreexport.os.path.dirname(ogreexport.Blender.Get('filename'))+"/export")
        self.scene = ogreexport.Blender.Scene.GetCurrent().getName()

    def setExportPath(self, path):
        self.path = path
        return

    def setOgreXmlConverterApp(self, path):
        self.ogreXmlConverterApp = path
        return

    def exportAll(self):
        print "Exporting Motorsport vehicle..."
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
        dsApp.sceneExporter.export()
        return

    def exportTextures(self):
        print "exportTextures() not coded yet."
        return
    
    def transformScene(self):
        os.system("xsltproc blend2mospvehicle.xsl "+self.path+"/"+self.scene+".xml > " +self.path+ "/vehicle.xml")
        return


######
# Main
######
if (__name__ == "__main__"):
    application = Blend2MospVehicleApplication()
    application.setOgreXmlConverterApp ('OgreXMLConverter')
    application.setExportPath(ogreexport.os.path.dirname(ogreexport.Blender.Get('filename'))+"/export")
    application.exportAll()
    application.cleanAll()
    print "############ Done! #############"
