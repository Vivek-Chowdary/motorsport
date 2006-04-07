Installation:

-Copy "blend2mospvehicle.py" to your blender plugins directory. In typical unix systems, that's "~/.blender/scripts/".
-Edit the file, and customize the last lines (application.set*) so that they point to the appropriate directories.

Usage:

-Start blender.
-Open the vehicle .blend file.
-Assuming the .blend file follows the necessary rules, start the exporter plugin. You can usually do this by clicking "File" > "Export" > "Blend2MospVehicle exporter".

That's it. Your car will have been exported to the specified dir, which by default is  "[.blend directory]/export/".

There's no gui at the moment. All errors will be reported via console, check it when the exported car doesn't work.
