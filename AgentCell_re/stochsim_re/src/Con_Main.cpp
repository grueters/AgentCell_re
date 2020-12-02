/*************************************************************************
*
* FILENAME:	Con_Main.cpp
*
* DESCRIPTION:	This is the MAIN procedure of the console application.
*		A global declaration of a C_Application pointer is made
*		which is used in other classes to refer to the application
*		object. An object of type C_Console_App is created,
*		representing the complete console application. This is then
*		executed (which creates the C_System object and runs the
*		simulations).
*
* TYPE:		Console
*
*************************************************************************/


#include "_Stchstc.hh"
#include "console.hh"

#ifdef _UNIX
#include <unistd.h>
#endif


//C_Application* pApp;  // Global declaration of pointer to application object


/*************************************************************************
*
* FUNCTION NAME:	Main
*
* DESCRIPTION:		A console application is created and executed.
*
* RETURNS:		int -	Indicating whether or not the application
*				executed successfully.
*
*************************************************************************/

int
main(int argc, char** argv)
{
  Bool bSuccess;
  char lpszFileName[MAX_INI_VALUE_LENGTH];
  char lpszWorkingDir[MAX_FILE_NAME_LENGTH];
  C_Application* pApp;  // Pointer to application object (previously global)

// If we are debugging using Microsoft extensions...
#ifdef _MSDEBUG
  int nFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );  // Get current flag
  nFlag |= _CRTDBG_LEAK_CHECK_DF;		// Turn on leak-checking bit
  // Turn memory checking for every memory allocation / deallocation request
  nFlag |= _CRTDBG_CHECK_ALWAYS_DF;
  _CrtSetDbgFlag(nFlag);			// Set flag to the new value
#endif


  strcpy(lpszFileName, APP_INI_FILE_NAME); // Copy the standard INI file name

  // Get the current working directory
  getcwd(lpszWorkingDir, MAX_FILE_NAME_LENGTH); 

#ifdef _UNIX
  int nOptionChar;

  while(1)
    {
      nOptionChar = getopt(argc, argv, "af:d:");  // Get command-line arguments
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
	case 'd':            // option for user-specified CWD
	  strcpy(lpszWorkingDir,optarg);
	  printf("Changing working directory to %s.\n", lpszWorkingDir);
	  break;
	case 'a':
	  break;
	default:
	  printf ("Error: getopt returned character code 0%o\n", nOptionChar);

	}
    }
#endif /* _UNIX */

  pApp = new C_Console_App(lpszFileName, lpszWorkingDir);  // Create new console application
  bSuccess = pApp->Init();                 // Initialise application
  if (bSuccess)			// If application is initialised successfully
    bSuccess = pApp->Run(pApp);       // Run console application
  else
    //pApp->Message(MSG_TYPE_ERROR, 168);
    fprintf(stderr,MSG_ERROR_APPLICATION_INIT);
  delete pApp;			// Delete application object
  // Return flag indicating whether the application object
  // executed successfully
  if (bSuccess)
    return 0;
  else
    return 1;
}
