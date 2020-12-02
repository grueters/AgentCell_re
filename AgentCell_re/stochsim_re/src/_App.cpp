/*************************************************************************
*
* FILENAME:	App.cpp
*
* CLASS:	C_Application
*
* DESCRIPTION:	This abstract class is used as an ancestor for the
*		applications. Specific applications (eg console-based
*		application) are derived from this object, and overide
*		mostly output-related operations. This class stores
*		all output to a log file before calling the appropriate
*		message handler. 
*
*************************************************************************/

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

C_Application::C_Application(const char* lpszAppINIFile)
{
  // Copy name of application INI file
  strcpy(m_lpszAppINIFile, lpszAppINIFile);
  strcpy(m_lpszWorkingDir, ".");
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

C_Application::C_Application(const char* lpszAppINIFile, const char* lpszWorkingDir)
{
  // Copy name of working directory
  strcpy(m_lpszWorkingDir, lpszWorkingDir);
  // Copy name of application INI file
  strcpy(m_lpszAppINIFile, lpszAppINIFile);
}



/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises class. Ensures the system INI file is available
*		for use. The filename to be used for the log file
*		is retrieved and title information is put in it.
*
*************************************************************************/

Bool
C_Application::Init(void)
{
  ofstream*	P_Log_File;
  time_t		tCurrDateTime;
  ifstream*	P_System_INI_Temp;
  char		lpszLogFileName[MAX_INI_VALUE_LENGTH];
  char		lpszMsg[MAX_MESSAGE_LENGTH];
  char          lpszAppINIFilePath[MAX_FILE_NAME_LENGTH];
  
  // Construct full path name for INI file
  sprintf(lpszAppINIFilePath, "%s%s%s", m_lpszWorkingDir, FILE_PATH_SEPARATOR,
	  m_lpszAppINIFile);
  // Open input stream for INI file (only if it exists)
  P_System_INI_Temp =
    new ifstream(lpszAppINIFilePath, ios::in);

// This ifdef is for compatibility with the Compaq and IBM native C++ compilers
#if ( defined(_ALPHA) || defined(_AIX) )
  // Ensure stream was opened successfully
  if (!(P_System_INI_Temp->rdbuf()->is_open()))
#else
    // Ensure stream was opened successfully
    if (!(P_System_INI_Temp->is_open()))
#endif /*  ( defined(_ALPHA) || defined(_AIX) ) */
      {
	//If there was an error, output message directly
	// (don't try using Message as it will fail!)
	sprintf(lpszMsg, MSG_CANNOT_FIND_SYSTEM_INI, lpszAppINIFilePath);
	ErrOut(lpszMsg);
	delete P_System_INI_Temp;
	return FALSE;
      }
  P_System_INI_Temp->close();
  delete P_System_INI_Temp;

  // Attempt to get Log file name
  if (!Get_INI(INI_FILE_NAME_SECTION, INI_LOG_FILE, lpszLogFileName,
		     m_lpszAppINIFile, TRUE))
    return FALSE;
  strcpy(m_lpszLogFileName, m_lpszWorkingDir);
  strcat(m_lpszLogFileName, FILE_PATH_SEPARATOR);
  strcat(m_lpszLogFileName, lpszLogFileName);

  if (strcmp(m_lpszLogFileName, "") != 0)  // If a filename has been defined...
    {
//      // Modifications for wrapper applications
//      char runIDString[128];
//      int i;
//      for (i = 0; i < MAX_NUM_RUN_IDS; i++) runIDString[i] = '0';
//      if (m_nRunID > 0)
//	{sprintf(runIDString, "%d", m_nRunID);}
//      else
//	{runIDString[0]='\0';}
//      strcat(m_lpszLogFileName, runIDString);
//      strcat(m_lpszLogFileName, ".OUT");


      // Open log file in write mode
      P_Log_File = new ofstream(m_lpszLogFileName);
      if (P_Log_File == NULL)  // If open failed, output error
	{
	  Message(MSG_TYPE_ERROR, 2, "Log");
	  return FALSE;
	}
      time(&tCurrDateTime);  // Get current time and date
      // Output time and date to log file
      *P_Log_File << ctime(&tCurrDateTime) << endl << endl; 
      P_Log_File->close();  // Close output file
      delete P_Log_File;
    }	
  return TRUE;
}
		



/*************************************************************************
*
* METHOD NAME:	Get_INI
*
* DESCRIPTION:	This method is used to retrieve the value of a parameter
*		from an INI file. The specified section is located within
*		the specified INI file; then the specified parameter is
*		located and the value of that parameter retrieved.
*
* PARAMETERS:	char* lpszSection    - Name of section within INI file.
*	        char* lpszParameter  - Name of parameter within section.
*		char* lpszFinalValue - String in which to store retrieved
*				       value.
*		char* lpszINIFile    - Name of INI file to use. This does
*				       not need to be specified, and will
*				       default to the system INI file.
*		Bool  bSilent	     - Flag controlling whether or not
*				       errors are reported if parameter
*				       not found. Default is FALSE.
*
* RETURNS:		Bool  TRUE   - Value returned successfully.
*			      FALSE  - An error occurred finding parameter.
*
*************************************************************************/

// This ifdef switches off global optimisation for this function under WIN32.
// (see help for compiler option /Og). 
// The input file stream P_INI_Stream becomes invalid in this function if
// it is optimised and references to the stream (in particular the eof()
// reference in the while loop) leads to access violation.

#ifdef WIN32
#ifdef NDEBUG
#pragma optimize("g", off)
#endif
#endif

Bool
C_Application::Get_INI(const char* lpszSection, const char* lpszParameter,
		       char* lpszFinalValue, const char* lpszINIFile,
		       Bool bSilent)
{
  char	lpszLine[MAX_INI_LINE_LENGTH];
  char	lpszParaName[MAX_INI_PARAMETER_LENGTH];
  char	lpszSectionFull[MAX_INI_SECTION_LENGTH];
  char	lpszValue[MAX_INI_VALUE_LENGTH_LONG];
  char	lpszTemp[MAX(MAX_INI_PARAMETER_LENGTH, MAX_INI_VALUE_LENGTH_LONG)];
  char  lpszMsg[MAX_MESSAGE_LENGTH];
  char  lpszINIFilePath[MAX_FILE_NAME_LENGTH];
  int	i;
  Bool	bInString;
  ifstream*	P_INI_Stream;

  if (lpszINIFile == NULL)  // If no INI file was specified
    lpszINIFile = m_lpszAppINIFile;  // Use the system INI file
  // Create full section heading eg [Simulation]; start with "["
  strcpy(lpszSectionFull, "[");
  // Add specified section name; eg "[Simulation"
  strcat(lpszSectionFull, lpszSection);
  // Add final bracket eg "[Simulation]"
  strcat(lpszSectionFull, "]");

  // Construct full path name for INI file
  sprintf(lpszINIFilePath,"%s%s%s",m_lpszWorkingDir, FILE_PATH_SEPARATOR, lpszINIFile);
  // Open input stream for INI file (only if it exists)
  P_INI_Stream = new ifstream(lpszINIFilePath, ios::in);

// This ifdef is for compatibility with the Compaq and IBM native C++ compilers
#if ( defined(_ALPHA) || defined(_AIX) )
  // Ensure stream was opened successfully
  if (!(P_INI_Stream->rdbuf()->is_open()))
#else
    // Ensure stream was opened successfully
    if (!(P_INI_Stream->is_open()))
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
      {
	//	Message(MSG_TYPE_ERROR, 29, lpszINIFile);
	sprintf(lpszMsg, MSG_CANNOT_FIND_SYSTEM_INI, lpszINIFile);
	ErrOut(lpszMsg);
	delete P_INI_Stream;
	return FALSE;
      }

  // Loop through the INI file until the correct section is found or
  // the end of file is reached
  do
    // Retrieve line from INI file
    // (characters after the closing ']' are ignored)
    P_INI_Stream->getline(lpszLine, MAX_INI_LINE_LENGTH);
  while ((P_INI_Stream->eof() == 0) &&
	 (strncmp(lpszLine, lpszSectionFull, strlen(lpszSectionFull)) != 0));

  // If the heading was found, look for appropriate line
  if (!P_INI_Stream->eof())
    {
      do
	{
	  // Initialise strings and retrieve line from INI file
	  lpszParaName[0] = NULL_CHAR;
		  lpszValue[0] = NULL_CHAR;
	  P_INI_Stream->getline(lpszLine, MAX_INI_LINE_LENGTH);
	  i = 0;
	  bInString = FALSE;

	  // Strip off comments: move through line until
	  // end of string is reached or ";" is reached
	  while (((lpszLine[i] != ';') || (bInString)) &&
		 ((unsigned) i < strlen(lpszLine)))
	    {
	      // If a quote mark is found, start ignoring ";"
	      // (semicolons are allowed within quotation marks)
	      if (lpszLine[i] == '"')
		bInString = !bInString;
	      i++;
	    }	
	  lpszLine[i] = NULL_CHAR;  // Terminate line to remove comments
	  if (strchr(lpszLine, '=') != NULL)  // If there is an '=' in line...
	    {
	      // Copy parameter name (before '=')
	      strncpy(lpszParaName, lpszLine, sizeof(lpszParaName));
	      lpszParaName[strcspn(lpszLine, "=")] = NULL_CHAR;
	      // Copy parameter value (after '=')
	      strcpy(lpszValue, strchr(lpszLine, '=') + 1);

	      // Strip characters from start of parameter name until
	      // char > 32 is reached (skips control chars and spaces)
	      while ((lpszParaName[0] < 33) && (strlen(lpszParaName) > 0))
		{
		  strcpy(lpszTemp, lpszParaName + 1);	
		  strcpy(lpszParaName, lpszTemp);
		}

	      // Strip characters from end of parameter name until
	      // char > 32 is reached (skips control chars and spaces)
	      while ((strlen(lpszParaName) > 0) &&
		     (lpszParaName[strlen(lpszParaName) - 1] < 33))
		lpszParaName[strlen(lpszParaName) - 1] = NULL_CHAR;	

	      // Strip characters from start of value until
	      // char > 32 is reached (also remove quotation marks)
	      while (((lpszValue[0] < 33)  || (lpszValue[0] == '"'))
		     && (strlen(lpszValue) > 0))
		{
		  strcpy(lpszTemp, lpszValue + 1);			
		  strcpy(lpszValue, lpszTemp);
		}
	      
	      // Strip characters from end of value until
	      // char > 32 is reached (also remove quotation marks)
	      while ((strlen(lpszValue) > 0)
		     && ((lpszValue[strlen(lpszValue) - 1] < 33)
			 || (lpszValue[strlen(lpszValue) - 1] == '"')))
		lpszValue[strlen(lpszValue) - 1] = NULL_CHAR;
	    }
	}
      
      // Keep extracting lines until the end of file, the correct line,
      // or the next section is reached
      while ((P_INI_Stream->eof() == 0)
	     && (strcmp(lpszParaName, lpszParameter) != 0)
	     && (lpszLine[0] != '['));
    }

  P_INI_Stream->close();  // Close input file
  delete P_INI_Stream;

  // If we haven't found parameter, output error
  if (strcmp(lpszParaName, lpszParameter) != 0)
    {
      lpszFinalValue = NULL;
      // If we are displaying errors, output error
      if (!bSilent)
	Message(MSG_TYPE_ERROR, 28, 3, lpszParameter,
		lpszSection, lpszINIFile);
      return FALSE;
    }
  strcpy(lpszFinalValue, lpszValue);  // Copy parameter value for returning
  return TRUE;
}

#ifdef WIN32
#ifdef NDEBUG
#pragma optimize("g", on)
#endif
#endif




/*************************************************************************
*
* METHOD NAME:	Get_INI
*
* DESCRIPTION:	This method is used to retrieve the value of a LONG
*		parameter from an INI file. The overloaded method:
*		Get_INI(char*, char*, char* , char*) is called and the
*		string returned is converted into a long integer.
*
* PARAMETERS:	char* lpszSection    -	Name of section within INI file.
*		char* lpszParameter  -	Name of parameter within section.
*		long* p_nFinalValue  -	Pointer where value should be stored.
*		char* lpszINIFile    -	Name of INI file to use. This does
*					not need to be specified, and will
*					default to the system INI file.
*		Bool  bSilent	     -	Flag controlling whether or not
*					errors are reported if parameter
*					not found. Default is FALSE.
*
* RETURNS:	Bool  TRUE	     -	Value returned successfully.
*		      FALSE	     -	An error occurred finding parameter.
*
*************************************************************************/

Bool
C_Application::Get_INI(const char* lpszSection, const char* lpszParameter,
		       long* p_nFinalValue, const char* lpszINIFile,
		       Bool bSilent)
{
  char lpszStrValue[MAX_INI_VALUE_LENGTH_LONG];

  // Ensure no error occurred retrieving parameter value as 
  // string from INI file
  if (!Get_INI(lpszSection, lpszParameter, lpszStrValue, lpszINIFile, bSilent))
    return FALSE;
  // Convert returned string to long integer
  *p_nFinalValue = atol(lpszStrValue);
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Get_INI
*
* DESCRIPTION:	This method is used to retrieve the value of a DOUBLE
*		parameter from an INI file. The overloaded method:
*		Get_INI(char*, char*, char* , char*) is called and the
*		string returned is converted into a double floating point
*		number.
*
* PARAMETERS:	char* lpszSection     -	Name of section within INI file.
*		char* lpszParameter   -	Name of parameter within section.
*		double* p_nFinalValue -	Pointer where value should be stored.
*		char* lpszINIFile     -	Name of INI file to use. This does
*					not need to be specified, and will
*					default to the system INI file.
*		Bool  bSilent	      -	Flag controlling whether or not
*					errors are reported if parameter
*					not found. Default is FALSE.
*
* RETURNS:	Bool  TRUE	      -	Value returned successfully.
*		FALSE		      -	An error occurred finding parameter.
*
*************************************************************************/

Bool
C_Application::Get_INI(const char* lpszSection, const char* lpszParameter,
		       double* p_flFinalValue, const char* lpszINIFile,
		       Bool bSilent)
{
  char lpszStrValue[MAX_INI_VALUE_LENGTH];
  // Ensure no error occurred retrieving parameter value as
  // string from INI file
  if (!Get_INI(lpszSection, lpszParameter, lpszStrValue, lpszINIFile, bSilent))
    return FALSE;
  // Convert returned string to double floating point number
  *p_flFinalValue = atof(lpszStrValue);
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Message
*
* DESCRIPTION:	Displays a message specified by a message number. The
*		message INI file is accessed and the approriate error
*		message retrieved. Parameters within the message retrieved
*		are specified by a code string (INI_MESSAGE_PARAMETER).
*		These are replaced by parameters from the parameter list
*		passed into the method. The message may be an error message
*		or status message. The error message is also output to
*		an error file. 
*
* PARAMETERS:	int   nMessageType	- Type of message (see Stochstc.h).
*		long  nMessageNum	- Message number to be displayed
*		char* lpszConstParaList	- List of parameters to be used
*					  in message (separated by the
*					  PARAMETER_LIST_SEPARATOR).
*					  This is optional (defaults to
*					  NULL).
*
*************************************************************************/

void
C_Application::Message(int nMessageType, long nMessageNum,
		       const char* lpszConstParaList)
{
  int		nParaListNum;
  int		nParaNum;
  char		lpszMessageFile[MAX_INI_VALUE_LENGTH];
  char		lpszMessageFilePath[MAX_FILE_NAME_LENGTH];
  char		lpszParameter[MAX_INI_PARAMETER_LENGTH];
  char		lpszMsg[MAX_MESSAGE_LENGTH];
  char		lpszTemp[MAX_MESSAGE_LENGTH];
  char		lpszNewMsg[MAX_MESSAGE_LENGTH];
  char		lpszParaList[MAX_STRING_LENGTH];
  char*	        lpszPara[MAX_NUM_MESSAGE_PARAMETERS];
  char*	        pParameter;
  char*	        lpszPos;
  char		lpszErrMsg[MAX_MESSAGE_LENGTH];
  ofstream*	P_Log_File;
  ifstream*	P_Message_INI_Temp;

  // If no message number was specified, return
  if (nMessageNum < 1)
    return;

  // Extract parameters from parameter list
  nParaListNum = 0;

  // If there is a parameter list...
  if (lpszConstParaList != NULL)				
    {
      //Check for buffer overflows (Thierry Emonet 2006/04/19)
      int i=0;
      while ((lpszConstParaList[i] != '\0') && (i < MAX_STRING_LENGTH))
	i++;
      if (i == MAX_STRING_LENGTH)
	{
	  cout << "ERROR: MESSAGE BUFFER OVERFLOW in C_Application::Message\n"
	       << "  String passed to Message is too long. The input string\n"
	       << "  should contain less than MAX_STRING_LENGTH = "
	       << MAX_STRING_LENGTH << "\n" 
	       << "  characters. Your string is: " << lpszConstParaList 
	       << endl;
	  exit(1);
	}

      // Copy list to working variable and find first parameter
      strcpy(lpszParaList, lpszConstParaList);
      lpszPos = strtok(lpszParaList, PARAMETER_LIST_SEPARATOR);

      // While the last parameter has not been found and
      // the max number of parameters has not been exceeded
      while ((lpszPos != NULL) && (nParaListNum < MAX_NUM_MESSAGE_PARAMETERS))
	{
	  lpszPara[nParaListNum] = lpszPos;  // Add parameter to array
	  nParaListNum ++;		     // Increment parameter counter
	  // Find next parameter
	  lpszPos = strtok(NULL, PARAMETER_LIST_SEPARATOR);
	}
      if (lpszPos != NULL)  // Ensure the end of the list was reached
	{
	  ErrOut(MSG_TOO_MANY_PARAMETERS);
	  return;
	}
    }

  // Retrieve name of messages INI file
  if (!Get_INI(INI_FILE_NAME_SECTION, INI_MESSAGE_INI_FILE, lpszMessageFile, m_lpszAppINIFile, TRUE))
    {
      ErrOut(MSG_ERROR_RETRIEVING_MSG);
      return;
    }

// Ensure message INI file exists
  
  // Construct full path name for INI file
  sprintf(lpszMessageFilePath,"%s%s%s",m_lpszWorkingDir, FILE_PATH_SEPARATOR, lpszMessageFile);
  //  sprintf(lpszMessageFilePath,"%s%s%s",".", FILE_PATH_SEPARATOR, lpszMessageFile);
  //  sprintf(lpszMessageFilePath,"%s%s%s",
	  //	  "/cygdrive/c/Tom/StochSim/src/StochSim_30OCT2006_for_Tom/tests/TestCWD/data/cell1/Input",
  //	  "..",
  //	  "/",
  //	  "MESSAGE_tmp.INI");
  // Open input stream for INI file (only if it exists)
  P_Message_INI_Temp = new ifstream(lpszMessageFilePath, ios::in);

  // If there was an error, output message directly
  // (don't try using Message as it will fail!)

// This ifdef is for compatibility with the Compaq and IBM native C++ compilers

#if ( defined(_ALPHA) || defined(_AIX) )
  // Ensure stream was opened successfully
  if (!(P_Message_INI_Temp->rdbuf()->is_open()))
#else
  // Ensure stream was opened successfully
  if (!(P_Message_INI_Temp->is_open()))
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
      {
	sprintf(lpszMsg, MSG_CANNOT_FIND_MESSAGE_INI, lpszMessageFile);
	ErrOut(lpszMsg);
	delete P_Message_INI_Temp;
	return;
      }
  P_Message_INI_Temp->close();
  delete P_Message_INI_Temp;

  // Retrieve message from message file

  sprintf(lpszParameter, "%ld", nMessageNum);
    // Convert message number to string
    // (ie the name of the parameter in the INI file)

  // Retrieve message
  if (!Get_INI(INI_MESSAGES_SECTION, lpszParameter, lpszMsg, lpszMessageFile))
    {
      ErrOut(MSG_ERROR_RETRIEVING_MSG);
      return;
    }
	
  // Ensure the final message will not be too long
  if (strlen(lpszMsg) +
      (lpszConstParaList != NULL ? strlen(lpszConstParaList) : 0) >
      MAX_MESSAGE_LENGTH)
    {
      ErrOut(MSG_ERROR_TOO_LONG);
      return;
    }

// Look for any parameters the message requires

  strcpy(lpszNewMsg, lpszMsg);	// Copy message to new (working) message
  nParaNum = 0;

  // Repeat until there are no more parameters in the message to be replaced
  while (strstr(lpszNewMsg, INI_MESSAGE_PARAMETER) != NULL)
    {
      pParameter = strstr(lpszNewMsg, INI_MESSAGE_PARAMETER);
        // Store location of parameter within the new message
      *pParameter = NULL_CHAR;
        // Remove parameter and beyond from the new message

      // Ensure another parameter is available in list
      if (nParaNum > nParaListNum - 1)
	{
	  sprintf(lpszErrMsg,
		  "Wrong number of parameters for Message number %ld.",
		  nMessageNum);
	  ErrOut(lpszErrMsg);
	  //ErrOut(MSG_WRONG_NUMBER_PARAMETERS);
	  return;
	}
      strcat(lpszNewMsg, lpszPara[nParaNum]);
        // Add the actual value of the parameter to the new message
      nParaNum ++;
        // Go to the next parameter
      strcpy(lpszTemp,
	     strstr(lpszMsg, INI_MESSAGE_PARAMETER)
	     + strlen(INI_MESSAGE_PARAMETER));
      strcpy(lpszMsg, lpszTemp);
        // Remove text preceeding parameter from old message
      strcat(lpszNewMsg, lpszMsg);
        // Copy the rest to new message
    }

  // Ensure the correct number of parameters were passed in
  if (nParaNum != nParaListNum)
    {
      sprintf(lpszErrMsg,
	      "Wrong number of parameters for Message number %ld.",
	      nMessageNum);
      ErrOut(lpszErrMsg);
        //ErrOut(MSG_WRONG_NUMBER_PARAMETERS);
      return;
    }

  // If message is an error, append error code to message
  if (nMessageType == MSG_TYPE_ERROR)
    {
      strcat(lpszNewMsg, MSG_ERROR_CODE_PREFIX);
      sprintf(lpszTemp, "%ld", nMessageNum);
      strcat(lpszNewMsg, lpszTemp);
      strcat(lpszNewMsg, MSG_ERROR_CODE_SUFFIX);
    }

// Output message to log file

  // If the message is to be logged, and a filename has been defined...
  if (nMessageType != MSG_TYPE_NOLOG
      && strcmp(m_lpszLogFileName, "") != 0)
    {
      P_Log_File = new ofstream(m_lpszLogFileName, ios::app);
        // Open log file in append mode

      // If open failed, output error
      if (P_Log_File == NULL)
	{
	  ErrOut(MSG_LOG_FILE_ERROR);
	  strcpy(m_lpszLogFileName, "");
	  return;
	}
      *P_Log_File << lpszNewMsg << endl;
        // Output message to stream
      P_Log_File->close();
        // Close output file
      delete P_Log_File;
    }	

  // Display final message
  if (nMessageType == MSG_TYPE_ERROR)
    ErrOut(lpszNewMsg, nMessageNum);
  else
    StatusOut(lpszNewMsg);
}

	
	
	
/*************************************************************************
*
* METHOD NAME:	Message
*
* DESCRIPTION:	Displays a message specified by a message number using
*		a variable number of long integer parameters. The
*		parameter list string is constructed from the numerical
*		parameters so that the following overloaded method can
*		be called: Message(int, long, char*).
*
* PARAMETERS:	int   nMessageType -	Type of message (see Stochstc.h).
*		long  nMessageNum  -	Message number to be displayed
*		int   nNumPara	   -	Number of parameters in list
*					(must be between 1 and 5!)
*		long  nPara1	   -	First long integer parameter
*		long  nPara2	   -	Second long integer parameter
*					(this is optional; default 0).
*		long  nPara3	   -	Third long integer parameter
*					(this is optional; default 0).
*		long  nPara4	   -	Fourth long integer parameter
*					(this is optional; default 0).
*		long  nPara5	   -	Fifth long integer parameter
*					(this is optional; default 0).
*
*************************************************************************/

void
C_Application::Message(int nMessageType, long nMessageNum, int nNumPara,
			    long nPara1, long nPara2, long nPara3, long nPara4,
			    long nPara5)
{
  char lpszParaList[MAX_MESSAGE_LENGTH];
  char lpszParaNew[MAX_MESSAGE_LENGTH];
  long nPara[5];
  int  nCountPara;

  // Move parameters into an array, so they can be handled more efficiently
  nPara[0] = nPara1;
  nPara[1] = nPara2;
  nPara[2] = nPara3;
  nPara[3] = nPara4;
  nPara[4] = nPara5;
  
  // Put the first parameter into the parameter list
  sprintf(lpszParaList, "%ld", nPara[0]);
  
  // Loop through the rest of the specified parameters, and...
  // add each parameter, separated by the parameter list separator, to the list
  for (nCountPara = 1; nCountPara < nNumPara; nCountPara ++)
    {
      sprintf(lpszParaNew, "%s%ld",
	      PARAMETER_LIST_SEPARATOR,
	      nPara[nCountPara]);
      strcat(lpszParaList, lpszParaNew);
    }
  Message(nMessageType, nMessageNum, lpszParaList);
    // Call overloaded method to display message with parameter list
}

	
	
	
/*************************************************************************
*
* METHOD NAME:	Message
*
* DESCRIPTION:	Displays a message specified by a message number using
*		a variable number of string parameters. The
*		parameter list string is constructed from these string
*		parameters so that the following overloaded method can
*		be called: Message(int, long, char*).
*
* PARAMETERS:	int   nMessageType	- Type of message (see Stochstc.h).
*		long  nMessageNum	- Message number to be displayed
*		int   nNumPara		- Number of parameters in list
*					  (must be between 1 and 5!)
*		char* nPara1		- First string parameter
*		char* nPara2		- Second string parameter
*					  (this is optional; default NULL).
*		char* nPara3		- Third string parameter
*					  (this is optional; default NULL).
*		char* nPara4		- Fourth string parameter
*					  (this is optional; default NULL).
*		char* nPara5		- Fifth string parameter
*					  (this is optional; default NULL).
*
*************************************************************************/

void
C_Application::Message(int nMessageType, long nMessageNum, int nNumPara,
			    const char* nPara1, const char* nPara2,
			    const char* nPara3, const char* nPara4,
			    const char* nPara5)
{
  char		lpszParaList[MAX_MESSAGE_LENGTH];
  const char*	nPara[5];
  int			nCountPara;


  // Move parameters into an array, so they can be handled more efficiently
  nPara[0] = nPara1;
  nPara[1] = nPara2;
  nPara[2] = nPara3;
  nPara[3] = nPara4;
  nPara[4] = nPara5;

  // Put the first parameter into the parameter list
  strcpy(lpszParaList, nPara[0]);

  // Loop through the rest of the specified parameters, and...
  // add each parameter, separated by the parameter list separator, to the list
  for (nCountPara = 1; nCountPara < nNumPara; nCountPara ++)
    {
      strcat(lpszParaList, PARAMETER_LIST_SEPARATOR);
      if (nPara[nCountPara] != NULL)
	strcat(lpszParaList, nPara[nCountPara]);
    }
  Message(nMessageType, nMessageNum, lpszParaList);
    // Call overloaded method to display message with parameter list
}



/*************************************************************************
*
* METHOD NAME:	Init_Display_Variable
*
* DESCRIPTION:	Called in case the output device requires initialisation.
*		Nothing happens in this base class implementation.
*
**************************************************************************/


void
C_Application::Init_Display_Variables(C_Output_Variable**, long)
{
}

