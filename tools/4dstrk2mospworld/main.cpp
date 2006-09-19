#include <stdio.h>

int main(int argc, char ** argv)
{
    if (argc < 3)
    {
        printf("Error! Specify a 4D-Stunts .trk filename, and a Motorsport world .xml filename.\n");
        return -1;
    }
    char * trkfilename = argv[1];
    char * xmlfilename = argv[2];
    printf("Converting \"%s\" into \"%s\"...\n", trkfilename, xmlfilename);
    
    FILE * trkfile = fopen(trkfilename, "r");
    FILE * xmlfile = fopen(xmlfilename, "w");

    //start of motorsport xml file
    fprintf(xmlfile, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>");
    fprintf(xmlfile, "\n<world xmlns:xi=\"http://www.w3.org/2001/XInclude\">");
    fprintf(xmlfile, "\n    <customData");
    fprintf(xmlfile, "\n        description = \"Drive the Mosp1 car in the testing grounds!\"");
    fprintf(xmlfile, "\n        useAreaCamera =\"false\"");
    fprintf(xmlfile, "\n        gravityX = \"0.0\" gravityY = \"0.0\" gravityZ = \"-9.8\"");
    fprintf(xmlfile, "\n        skyMaterialName=\"skyboxMaterial\"");
    fprintf(xmlfile, "\n        skyDistance=\"5000.0\"");
    fprintf(xmlfile, "\n        skyDrawFirst=\"1\"");
    fprintf(xmlfile, "\n        height=\"-0.01\"");
    fprintf(xmlfile, "\n        >");
    fprintf(xmlfile, "\n    </customData>");

    //read items
    int items[30][30];
    //fseek(trkfile, 0, SEEK_SET);
    for (int x=0;x<30;x++) for (int y=0;y<30;y++)
    {
        fread(&(items[x][y]), 1, 1, trkfile);
    }

    //read skybox
    fread(&argc, 1, 1, trkfile);

    //read terrain
    int terrain[30][30];
    for (int x=0;x<30;x++) for (int y=0;y<30;y++)
    {
        fread(&(terrain[x][y]), 1, 1, trkfile);
    }

    const int tilesize = 30 * 2;
    int startx = 0;
    int starty = 0;
    char startt = 'i';
    for (int x=0;x<30;x++) for (int y=0;y<30;y++)
    {
        //*/
        if (items[x][y] <  0xfd &&
            items[x][y] >  0x00 &&
            items[x][y] != 0x02 &&
            items[x][y] != 0x03 )
        {
            fprintf(xmlfile, "\n    <sharedobject name=\"i%i,%i\"><area model=\"i0x%02x\" > </area></sharedobject>", x, y, items[x][y]);
            fprintf(xmlfile, "\n    <location name=\"i%i,%i\" position=\"%i %i 0\" rotation=\"0 0 0\"/>", x, y, y*tilesize, x*tilesize);
            fprintf(xmlfile, "\n    <location-area first=\"(location)i%i,%i\" second=\"(area)i%i,%i\"/>", x, y, x, y);
            startx = x;
            starty = y;
            startt = 'i';
        }
        //*/
        /*/
        if (terrain[x][y] < 0x13)
        {
            fprintf(xmlfile, "\n    <sharedobject name=\"t%i,%i\"><area model=\"t0x%02x\" > </area></sharedobject>", x, y, terrain[x][y]);
            fprintf(xmlfile, "\n    <location name=\"t%i,%i\" position=\"%i %i 0\" rotation=\"0 0 0\"/>", x, y, y*tilesize, x*tilesize);
            fprintf(xmlfile, "\n    <location-area first=\"(location)t%i,%i\" second=\"(area)t%i,%i\"/>", x, y, x, y);
            startx = x;
            starty = y;
            startt = 't';
        }
        //*/

    }

    //end of motorsport xml file
    fprintf(xmlfile, "\n    <sharedobject name=\"main\"><vehicle model=\"mosp1\" _model=\"quattros1\" > </vehicle></sharedobject>");
    fprintf(xmlfile, "\n    <location-vehicle first=\"(area)%c%i,%i/paceCar\" second=\"(vehicle)main\" />", startt, startx, starty);
    fprintf(xmlfile, "\n    <vehicle-driver first=\"(vehicle)main\" second=\"user\" />");
    fprintf(xmlfile, "\n</world>");

    printf("Done!\n");
    return 0;
}
