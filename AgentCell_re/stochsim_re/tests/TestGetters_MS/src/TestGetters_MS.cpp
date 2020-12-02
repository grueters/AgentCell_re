/*************************************************************************
*
* FILENAME:	TestGetTimeInc.cpp
*
* DESCRIPTION:	Get the size of StochSim's time increment and print.
*
* TYPE:		Wrapper Demo
*
*************************************************************************/

#include "_Stchstc.hh"
#include "console.hh"
#include "StochsimWrapper.hh"

#ifdef _UNIX
#include <unistd.h>
#endif

/*************************************************************************
*
* FUNCTION NAME:	Main
*
* DESCRIPTION:		Two console application is created and executed.
*
* RETURNS:		int -	Indicating whether or not the application
*				executed successfully.
*
*************************************************************************/

int
main(int argc, char** argv)
{
  long i,j;
  char lpszFileName[MAX_INI_VALUE_LENGTH];
  StochsimWrapper ** P_SS;

  long nNumSS = 1;
  long nNumSteps = 10;
  double dt = 0.005;


  strcpy(lpszFileName, APP_INI_FILE_NAME); // Copy the standard INI file name

  P_SS = new StochsimWrapper* [nNumSS];
  for (i = 0; i < nNumSS; i++)
    {
      cout << "Creating ...\n" << endl;
      P_SS[i] = new StochsimWrapper(lpszFileName, i);// Create new StochSim
      cout << "          GetTimeInc = " << P_SS[i]->getTimeInc() << endl;
      cout << "             GetTime = " << P_SS[i]->getTime() << endl;
      cout << " GetOVValue(\"Yp\") = " << P_SS[i]->getOVValue("Yp") << endl;
      cout << "GetOVValue(\"MYp\") = " << P_SS[i]->getOVValue("MYp") << endl;
      cout << "GetOVValue(\"WS\") = " << P_SS[i]->getOVValue("WS") << endl;
      for (j = 0; j < nNumSteps; j++)
	{
	  cout << "\nAdvancing by dt = " << dt <<"...\n" << endl;
	  P_SS[i]->step(dt);
	  cout << "          GetTimeInc = " << P_SS[i]->getTimeInc() << endl;
	  cout << "             GetTime = " << P_SS[i]->getTime() << endl;
	  cout << " GetOVValue(\"Yp\") = " << P_SS[i]->getOVValue("Yp") << endl;
	  cout << "GetOVValue(\"MYp\") = " << P_SS[i]->getOVValue("MYp") << endl;
	  cout << "GetOVValue(\"WS\") = " << P_SS[i]->getOVValue("WS") << endl;
	}
    }

  for (i = 0; i < nNumSS; i++)
    {
	  cout << "\nFinalizing..." << endl;
      P_SS[i]->finalise();
    }
  for (i = 0; i < nNumSS; i++)
    {
	  cout << "Deleting..." << endl;
      delete P_SS[i];
    }
  delete[] P_SS;
}
