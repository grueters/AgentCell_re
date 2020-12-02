/*************************************************************************
*
* FILENAME:	TestCWD.cpp
*
* DESCRIPTION:	This is a sample application that uses the StochsimWrapper
*               class to test the new CWD functionality.
*
* TYPE:		Wrapper Demo
*
*************************************************************************/

#define DEFAULT_WORKING_DIR_0 "../data/cell1/Input"
#define DEFAULT_WORKING_DIR_1 "../data/cell2/Input"


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
* DESCRIPTION:		Two console application are created and executed
*                       the directories specified explicity in the code.
*
* RETURNS:		int -	Indicating whether or not the application
*				executed successfully.
*
*************************************************************************/

int
main(int argc, char** argv)
{
  long i, j;
  char lpszFileName[MAX_INI_VALUE_LENGTH];
  char lpszWorkingDir[2][MAX_FILE_NAME_LENGTH];
  int nNumSS = 2;
  long nNumSteps = 10;
  double flDeltaT = 0.1;
  double flTimeInc;

  StochsimWrapper* P_SS [3];

  strcpy(lpszFileName, APP_INI_FILE_NAME); // Copy the standard INI file name

  strcpy(lpszWorkingDir[0], DEFAULT_WORKING_DIR_0);
  strcpy(lpszWorkingDir[1], DEFAULT_WORKING_DIR_1);

#ifdef _UNIX
  int nOptionChar;

  while(1)
    {
      nOptionChar = getopt(argc, argv, "f:d:e:n:t:");  // Get command-line arguments
      if (nOptionChar == -1)
	break;
      switch (nOptionChar)
	{
	case ':':
	  break;
	  //	  printf("The argument -%c requires a parameter.\n", optopt);
	case '?': 
	  //	  printf("The argument -%c requires a parameter.\n", optopt);
	  break;
	case 'f':            // option for user-specified INI file
	  strcpy(lpszFileName,optarg);
	  printf("The simulation configuration file is %s.\n", lpszFileName);
	  break;
	case 'd':            // option to specify first working directory
	  strcpy(lpszWorkingDir[0],optarg);
	  printf("Working directory 0 is %s.\n", lpszWorkingDir[0]);
	  break;
	case 'e':            // option to specify second working directory
	  strcpy(lpszWorkingDir[1],optarg);
	  printf("Working directory 1 is %s.\n", lpszWorkingDir[1]);
	  break;
	case 'n':            // option to specify number of iterations
	  nNumSteps = (long) atoi(optarg);
	  break;
	case 't':            // option to specify number of iterations
	  flDeltaT = (double) atof(optarg);
	  break;

	default:
	  printf ("Error: getopt returned character code 0%o\n", nOptionChar);

	}
    }
#endif /* _UNIX */

  for (i = 0; i < nNumSS; i++) P_SS[i] = NULL;
  for (i = 0; i < nNumSS; i++)
    {
      P_SS[i] = new StochsimWrapper(lpszFileName, lpszWorkingDir[i], i); // Create new StochSim
      /*
      if (!P_SS[i]->InitOK())
	{
	  fprintf(stderr, "There was an error while initialising StochSim instance %ld.\n", i);
	  return 1;
	}
      */
      flTimeInc = P_SS[i]->getTimeInc();
      printf("The size of the time increment is %le.\n", flTimeInc);
    }

  P_SS[2]=NULL;

  

  for (j = 0; j < nNumSteps; j++)
    {
      for (i = 0; P_SS[i] != NULL; i++)
	{
	  double flTime1;
	  long nCopyNum1;

	  flTime1 = P_SS[i]->getTime();
	  nCopyNum1 = P_SS[i]->getOVValue("Yp");

	  printf("SS%d: Yp=%ld @ t=%f s\n",
		 P_SS[i]->m_nRunID, nCopyNum1, flTime1);

	  P_SS[i]->step(flDeltaT);       
	  
	}
    }

  for (i = 0; i < nNumSS; i++)
    {
      P_SS[i]->finalise();
    }
  for (i = 0; i < nNumSS; i++)
    {
      delete P_SS[i];
    }
  //  delete[] P_SS;

}
