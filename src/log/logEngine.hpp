#ifndef LOG_ENGINE_HPP
#define LOG_ENGINE_HPP

#include <stdio.h>


class logEngine
{
	FILE *logFile;
	public:
		//starts the log process	
		void start (char *filePath, bool appendMode);
		//writes textToLog to the .logFile
		void put (char *textToLog);
		//stops the log process	
		void stop (void);
};


#else
#error multiple include of logEngine.hpp // this isn't normally done, but I like it :) MF
#endif
