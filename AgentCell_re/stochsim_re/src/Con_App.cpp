/*************************************************************************
*
* FILENAME:	Con_App.cpp
*
* CLASS:	C_Console_App
*
* DESCRIPTION:	This class represents the console-based application. It
*		is derived from the abstract class C_Application. Methods
*		concerning output are overriden (eg error message handling)
*		so that output is sent to the console; the Run method is
*		also overriden to create a C_System object and execute it
*		with output to the console.
*
* TYPE:		Console
*
*************************************************************************/


#include "console.hh"
#include "_Stchstc.hh"



/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Sets the name of the application INI file this application
*		should use.
*
* PARAMETERS:	char* lpszAppINIFile -	Name and path of application INI
*					file that this application will use.
*
*************************************************************************/

C_Console_App::C_Console_App(const char* lpszAppINIFile)
  : C_Application(lpszAppINIFile)
{
  // Calls base class constructor with name of application INI file
  m_nRunID = 0;
}

/*************************************************************************
*
* METHOD NAME:	Constructor (with explicit CWD setting)
*
* DESCRIPTION:	Sets the name of the application INI file this application
*		should use.
*
* PARAMETERS:	char* lpszAppINIFile -	Name and path of application INI
*					file that this application will use.
*
*       	char* lpszWorkingDir -	Path to which CWD to be set.
*
*************************************************************************/

C_Console_App::C_Console_App(const char* lpszAppINIFile, const char* lpszWorkingDir)
  : C_Application(lpszAppINIFile, lpszWorkingDir)
{
  // Calls base class constructor with name of application INI file
  m_nRunID = 0;
}


/*************************************************************************
*
* METHOD NAME:	ErrOut
*
* DESCRIPTION:	Outputs an error message to the console.
*
* PARAMETERS:	char* lpszConstErrMsg - Pointer to error message to be
*					displayed.
*
*************************************************************************/

void
C_Console_App::ErrOut(const char * lpszConstErrMsg, int nErrorCode)
{
  TextToConsole(lpszConstErrMsg);	// Output error message to console
}




/*************************************************************************
*
* METHOD NAME:	StatusOut
*
* DESCRIPTION:	Outputs a status message to the console.
*
* PARAMETERS:	char* lpszStatusMsg	 -  Pointer to the string to be
*                                           displayed.
*
*************************************************************************/

void
C_Console_App::StatusOut(const char * lpszStatusMsg)
{
  TextToConsole(lpszStatusMsg);		// Output message to console
}




/*************************************************************************
*
* METHOD NAME:	TextToConsole
*
* DESCRIPTION:	Outputs the message string passed to it. The
*		message is split up into multiple lines so
*		that none of the lines exceeds the maximum line width.
*
* PARAMETERS:	char* lpszConstMsg - Pointer to message to be displayed.
*
*************************************************************************/

void
C_Console_App::TextToConsole(const char * lpszConstMsg)
{
  char	lpszMsgLine[MAX_MESSAGE_LENGTH];
  char	lpszMsg[MAX_MESSAGE_LENGTH];
  char*	lpszWord;
  // If message can be output on one line
  if (strlen(lpszConstMsg) <= MAX_LINE_LENGTH)
    cout << endl << lpszConstMsg;		//   Output message
  // Otherwise split message into multiple lines
  else
    {
      strcpy(lpszMsg, lpszConstMsg);		// Copy message
      lpszWord = strtok(lpszMsg, SPACE_STR);	// Get first word
      // While the end of the message is not reached...
      while (lpszWord != NULL)
	{
	  lpszMsgLine[0] = NULL_CHAR;	// Initialise empty line
	  // While the end of the message is not reached and
	  // the end of line is not reached...
	  while ((lpszWord != NULL)
		 && (strlen(lpszMsgLine) + strlen(lpszWord) < MAX_LINE_LENGTH))
	    {            
	      strcat(lpszMsgLine, lpszWord);      // Add another word to line
	      strcat(lpszMsgLine, SPACE_STR);				
	      lpszWord = strtok(NULL, SPACE_STR); // Get next word
	    }
	  // If the frst word was too long to be added...
	  if (strlen(lpszMsgLine) == 0)
	    {
	      // Copy part of the word to the line
	      strncpy(lpszMsgLine, lpszWord, MAX_LINE_LENGTH);
	      // Null terminate line after the part of the word just copied
	      lpszMsgLine[MAX_LINE_LENGTH] = NULL_CHAR;
	      // Remove the part of the word from the original word for
	      // next time around
	      lpszWord += MAX_LINE_LENGTH;
	    }
	  cout << endl << lpszMsgLine;	// Output line to screen
	}
    }
  cout << endl;
}




/*************************************************************************
*
* METHOD NAME:	Run
*
* DESCRIPTION:	Creates a C_System object and executes it. The C_System
*		object contains everything necessary for a simulation.
*
* RETURNS:	Bool	TRUE  -	Application was executed successfully.
*			FALSE -	An error occurred during execution.
*
*************************************************************************/

Bool
C_Console_App::Run(C_Application* pApp)
{
  C_System System(pApp);	// Create new C_System object
  if (!System.Init())	// Attempt to initialise system
    {
      Message(MSG_TYPE_ERROR, 30);
      return FALSE;
    }
  if (!System.Run())    // Attempt to run system
    {
      Message(MSG_TYPE_ERROR, 31);  // Output error message
      return FALSE;
    }
  Message(MSG_TYPE_STATUS, 150);
  return TRUE;
}



/*************************************************************************
*
* METHOD NAME:	Display_Variable_Values
*
* DESCRIPTION:	Creates a C_System object and executes it. The C_System
*		object contains everything necessary for a simulation.
*
* PARAMETERS:	double	flTime	- Time at which variable values
*				    should be displayed
* C_Output_Variable** P_OV_Disp - Pointer to array of variables
*				  whose value are to be displayed
*	double	flPercComplete  - Percentage of simulation
*				  complete
*	long	 nNumDisplayVar - Number of variables
*
*************************************************************************/

void
C_Console_App::Display_Variable_Values(double flTime,
				       C_Output_Variable** P_OV_Disp,
				       double flPercComplete,
				       long nNumDisplayVar)
{
  int	 i;
  int	 nMaxLen;
  ostringstream MessageStream;

  // Format parameter list
  MessageStream << setprecision(PREC_PERC_COMPLETE) << fixed;
  MessageStream << flTime;
  MessageStream << PARAMETER_LIST_SEPARATOR;
  MessageStream << flPercComplete << ends;
  // Output message to console
  Message(MSG_TYPE_NOLOG, 55, MessageStream.str().c_str());  
  nMaxLen = 0;


  // Loop through all the display variables and find the longest variable name 
  for (i = 0; i < nNumDisplayVar; i++)
    nMaxLen = MAX(nMaxLen, (signed) strlen(P_OV_Disp[i]->Get_Name()));


  // Create format string for variable output eg "%c%c%-15s%7ld"

  // Loop through all the display variables  
  for (i = 0; i < nNumDisplayVar; i++)
    {
      // Display the variable name followed by its value
      MessageStream.str("");
      MessageStream << TAB_CHAR << TAB_CHAR << left;
      MessageStream << setw(nMaxLen);
      MessageStream << P_OV_Disp[i]->Get_Name();
      MessageStream << setw(PREC_VAR_VALUE) << right;;
      MessageStream << P_OV_Disp[i]->Get_Value() << ends;
      // Output line of text
      Message(MSG_TYPE_NOLOG, 56, MessageStream.str().c_str());
    }
  cout << endl;
}
