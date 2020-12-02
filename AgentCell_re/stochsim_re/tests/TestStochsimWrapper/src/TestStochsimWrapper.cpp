/*************************************************************************
*
* FILENAME:	TestStochsimWrapper.cpp
*
* DESCRIPTION:	Test the StochsimWrapper
*
* TYPE:		Wrapper Test
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
* DESCRIPTION:		A list of input directories can be passed. 
*                   In each one another instance of Stochsim is created.
*
* RETURNS:		int -	Indicating whether or not the application
*				executed successfully.
*
*************************************************************************/

int
main(int argc, char** argv)
{

  char lpszFileName[MAX_INI_VALUE_LENGTH];
  char lpszWorkingDir[MAX_FILE_NAME_LENGTH];
  StochsimWrapper ** P_SS;

  if (argc<2) {
  	cout << "You need to give at least one argument (path to one STCHSTC.INI file)." << endl;
  	cout << "   Example: ./TestStochsimWrapper ../test/test1/Input" << endl;
  	
    exit(-1);
  }

  long nNumSS = argc-1;
  double flTimeInc;
  strcpy(lpszFileName, APP_INI_FILE_NAME); // Copy the standard INI file name
  P_SS = new StochsimWrapper* [nNumSS];

  
  for (long i = 0; i < nNumSS; i++)
    {
      cout << "Creating a stochsim at: " << argv[i+1] << endl;
      strcpy(lpszWorkingDir, argv[i+1]);
      P_SS[i] = new StochsimWrapper(lpszFileName, lpszWorkingDir, i);// Create new StochSim
      flTimeInc = P_SS[i]->getTimeInc();
      printf("The size of the time increment is %le.\n", flTimeInc);
    }

  double dtWrapper=0.2;
  double runtime=0.4;
  double t=0;

  while (t < runtime) 
    {
      t += dtWrapper;
      for (long i = 0; i < nNumSS; i++)
    	{
	  P_SS[i]->step(dtWrapper);
	  cout << "run = " << i;
	  cout << ", t = " << t; 
	  cout << ", Yp = " << P_SS[i]->getOVValue("Yp");
	  cout << ", MYp = " << P_SS[i]->getOVValue("MYp") << endl;
    	}
    } 

  for (long i = 0; i < nNumSS; i++)
    {
	  cout << "Terminating stochsim at: " << argv[i+1] << endl;
      P_SS[i]->finalise();
    }
  for (long i = 0; i < nNumSS; i++)
    {
      delete P_SS[i];
    }
  delete[] P_SS;
}
