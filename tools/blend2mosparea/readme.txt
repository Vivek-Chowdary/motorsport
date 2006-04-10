Installation:

-Copy "blend2mosparea.py" to your blender plugins directory. In typical unix systems, that's "~/.blender/scripts/".
-Edit the file, and customize the last lines (application.set*) so that they point to the appropriate directories.

Usage:

-Start blender.
-Open the area .blend file.
-Assuming the .blend file follows the necessary rules, start the exporter plugin. You can usually do this by clicking "File" > "Export" > "Blend2MospArea exporter".

That's it. Your track will have been exported to the specified dir, which by default is  "[.blend directory]/export/".

There's no gui at the moment. All errors will be reported via console, check it when the exported track doesn't work.
