
#include "logEngine.hpp"


class dataEngine
{
	logEngine log;	//a log object for logging independently from other engines and main
	public:
		void start (void);		//starts the data engine
		/*here will probably go some new functions such as:
			
			int loadCarModel(carList*,FILE*carXmlFile);
			int loadTrackModel(track*,FILE*trackXmlFile);
		
		or maybe...

			int loadStreamingTrack(track*,FILE*trackXmlFile,currentPosition*,distance*);
		
		etc...*/
		void stop (void);		//stops the data engine
};
