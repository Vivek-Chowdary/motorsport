/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "MTest.h"
#include "logEngine.hpp"
#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#ifndef WIN32
#include <unistd.h>
#endif
#include <cerrno>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
std::string TestLogFile = "motorsport.log";

// TODO: A lot of redundancy, refactor.
//

void EraseTestFileContents ()
{
    std::fstream testFile;
    // empty the file
    testFile.open (TestLogFile.c_str (), std::fstream::out);
    testFile.flush ();
    testFile.close ();
}


void LogFileBackup ()
{
    std::string backupName = TestLogFile + ".bak";
    if (rename (TestLogFile.c_str (), backupName.c_str ()) != 0)
    {
        std::cerr << "error while backingup logfile" << std::endl;
    }
}
void LogFileRestore ()
{
    std::string backupName = TestLogFile + ".bak";
    if (rename (backupName.c_str (), TestLogFile.c_str ()) != 0)
    {
        std::cerr << "error while restoring logfile" << std::endl;
    }
}

//////////////////////////////////////////////////////////////////
void TestLogEngineConstructor_FileCreation ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_TRACE, "TST");
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.getline (buf, 4096);

    std::string bufString (buf);

    MAssert (bufString.find ("TST") != std::string::npos);
    MAssert (bufString.find ("LogFile created") != std::string::npos);

    LogFileRestore ();
}

void TestLogEngineConstructor_Filter ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_ERROR, "TST");
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.getline (buf, 4096);

    std::string bufString (buf);


    // there should be no text like this in the file with this filter.
    MAssert (bufString.find ("TST") == std::string::npos);
    MAssert (bufString.find ("LogFile created") == std::string::npos);

    LogFileRestore ();
}

void TestLogEngineConstructor_SeveralInstances ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_TRACE, "TS1");
        {
            LogEngine logEngine2 (LOG_TRACE, "TS2");
        }
        std::fstream testFile;
        testFile.open (TestLogFile.c_str (), std::fstream::in);
        char buf[4096];
        testFile.read (buf, 4096);

        std::string bufString (buf);

        MAssert (bufString.find ("TS1") != std::string::npos);
        MAssert (bufString.find ("TS2") != std::string::npos);
        MAssert (bufString.find ("2 engine") != std::string::npos);
        MAssert (bufString.find ("s 0 engine") == std::string::npos);
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.read (buf, 4096);

    std::string bufString (buf);

    MAssert (bufString.find ("s 0 engine") != std::string::npos);


    LogFileRestore ();
}


void TestLogEngineConstructor_ComponentName ()
{
    char *componentNames[] = { "111",
        "22",
        "3",
        "",
        "!!!",
        "6 6",
        "7\07",
        "888888888888888888888888888888888888888888888888" "888888888888888888888888888888888888888888888888" "88888888888888888888888888888888888888888888888",
        "дс~",
        "ten"
    };


    for (int n = 0; n < 10; n++)
    {
        LogFileBackup ();

        // scope to construct+destruct the object.
        {
            LogEngine logEngine (LOG_TRACE, componentNames[n]);
        }

        std::fstream testFile;
        testFile.open (TestLogFile.c_str (), std::fstream::in);
        char buf[4096];
        testFile.getline (buf, 4096);

        std::string bufString (buf);

        std::string name (componentNames[n]);
        name.resize (3, ' ');

        MAssert (bufString.find (name) != std::string::npos);

        LogFileRestore ();
    }
}

void TestLogEngineConstructor ()
{
    TestLogEngineConstructor_FileCreation ();
    TestLogEngineConstructor_Filter ();
    TestLogEngineConstructor_SeveralInstances ();
    TestLogEngineConstructor_ComponentName ();
}

//////////////////////////////////////////////////////////////////

void TestPut_filter_eq ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_WARNING, "TST");
        MAssert (logEngine.put (LOG_WARNING, "logtext") == 0);
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.getline (buf, 4096);   // read line

    std::string bufString (buf);

    MAssert (bufString.find ("logtext") != std::string::npos);

    LogFileRestore ();
}

void TestPut_filter_gt ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_INFO, "TST");
        MAssert (logEngine.format (LOG_WARNING, "logtext") == 0);
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // read line

    std::string bufString (buf);

    MAssert (bufString.find ("logtext") != std::string::npos);

    LogFileRestore ();
}


void TestPut_filter_lt ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_WARNING, "TST");
        MAssert (logEngine.format (LOG_INFO, "logtext") == -1);
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // read line

    std::string bufString (buf);

    MAssert (bufString.find ("logtext") == std::string::npos);

    LogFileRestore ();
}


void TestPut ()
{
    TestPut_filter_eq ();
    TestPut_filter_gt ();
    TestPut_filter_lt ();
}

//////////////////////////////////////////////////////////////////
void TestFormat_basics ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_TRACE, "TST");
        MAssert (logEngine.format (LOG_WARNING, "s:%s d:%d %c", "a string", 42, '#') == 0);
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // read line

    std::string bufString (buf);

    MAssert (bufString.find ("s:a string d:42 #") != std::string::npos);

    LogFileRestore ();
}

void TestFormat_filter_eq ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_WARNING, "TST");
        MAssert (logEngine.format (LOG_WARNING, "s:%s d:%d %c", "a string", 42, '#') == 0);
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.getline (buf, 4096);   // read line

    std::string bufString (buf);

    MAssert (bufString.find ("s:a string d:42 #") != std::string::npos);

    LogFileRestore ();
}

void TestFormat_filter_gt ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_INFO, "TST");
        MAssert (logEngine.format (LOG_WARNING, "s:%s d:%d %c", "a string", 42, '#') == 0);
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // read line

    std::string bufString (buf);

    MAssert (bufString.find ("s:a string d:42 #") != std::string::npos);

    LogFileRestore ();
}


void TestFormat_filter_lt ()
{
    LogFileBackup ();

    // scope to construct+destruct the object.
    {
        LogEngine logEngine (LOG_WARNING, "TST");
        MAssert (logEngine.format (LOG_INFO, "s:%s d:%d %c", "a string", 42, '#') == -1);
    }

    std::fstream testFile;
    testFile.open (TestLogFile.c_str (), std::fstream::in);
    char buf[4096];
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // eat line
    testFile.getline (buf, 4096);   // read line

    std::string bufString (buf);

    MAssert (bufString.find ("s:a string d:42 #") == std::string::npos);

    LogFileRestore ();
}


void TestFormat ()
{
    TestFormat_basics ();
    TestFormat_filter_eq ();
    TestFormat_filter_gt ();
    TestFormat_filter_lt ();
}

//////////////////////////////////////////////////////////////////

void TestGetLogLevelCode ()
{
    LogEngine l (LOG_WARNING, "TST");
    MAssert (std::string ("EE") == l.GetLogLevelCode (LOG_ERROR));
    MAssert (std::string ("WW") == l.GetLogLevelCode (LOG_WARNING));
    MAssert (std::string ("II") == l.GetLogLevelCode (LOG_INFO));
    MAssert (std::string ("VV") == l.GetLogLevelCode (LOG_VERBOSE));
    MAssert (std::string ("TT") == l.GetLogLevelCode (LOG_TRACE));
}

//////////////////////////////////////////////////////////////////

int main ()
{
    MTestName = "Log Engine Tests";

    TestLogEngineConstructor ();
    TestPut ();
    TestFormat ();
    TestGetLogLevelCode ();

    MTestReport ();
    return MTestErrors;
}
