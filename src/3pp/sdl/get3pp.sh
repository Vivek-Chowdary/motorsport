#Get and install sdl
echo Retrieve and install SDL for rpm-able i386 linuxes.
wget http://www.libsdl.org/release/SDL-1.2.6-1.i386.rpm 
wget http://www.libsdl.org/release/SDL-devel-1.2.6-1.i386.rpm
rpm -U SDL*