// Minimalistic Unit Test Functionality
//
// documentation last in file
// Free to use, licensee chooses license.
// 
// v1.1 (c) 2004 Mattias Hellström

#ifndef M_TEST_H
#define M_TEST_H
#include <iostream>

#define MAssert(b) MTestAssert((b), #b, __FILE__, __LINE__)

static int MTestErrors = 0;
static int MTestTests = 0;
static char* MTestName = "Autotest";
void MTestAssert(bool b, char* arg, char* file, int line)
{
  if(b == false) {
    MTestErrors++;
    std::cout << "Asertion failed for " << arg 
	 << " at " << file << ":" << line << std::endl;
  } 
  MTestTests++;
}

void MTestReport()
{
  double passPercent = 100.00;
  if( MTestTests != 0 ) {
    passPercent = (100.00 * (MTestTests-MTestErrors)) / MTestTests;
  }

  std::cout << MTestName << " report:" 
	    << " Fail:" << MTestErrors 
	    << " Success:" << MTestTests - MTestErrors
	    << " Total:" << MTestTests 
	    << " Pass Percent:" << passPercent << "%"
	    << std::endl;
}
#endif



//  MTest (Mattias/Minimalistic unit Test framework)
//
//  Global functions and variables:
//    char* MTestName - Text to prefix the report.
//    MAssert(bool)   - Macro recording test result
//    MTestReport()   - Print a testreport
//    MTestErrors     - Contains number of failed asserts.  
//
//  Usagage example:
//  int main(){
//    MTestName = "My auto tests";
//    MAssert( /*logical expression*/ );
//    /* more asserts */
//    MTestReport();
//    return MTestErrors;
//  }
//
//  For more help, use the source.
//  (c) 2004 Mattias Hellström
