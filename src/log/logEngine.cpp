
#include "logEngine.hpp"


void logEngine::start (char *filePath, bool appendMode)
{
	//check if we append (or rewrite the whole file)
	logFile = fopen (filePath, (appendMode ? "a" : "w"));
	
	//we put some text at the start of the current log
	put ("START OF LOG");
}


void logEngine::put (char *textToLog)
{
	//write the textToLog in the log file after a EOL
	fputc ('\n', logFile);
	fputs (textToLog, logFile);
}


void logEngine::stop (void)
{
	put ("END OF LOG");
	fclose (logFile);
}
