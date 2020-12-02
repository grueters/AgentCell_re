/*************************************************************************
*
* FILENAME:	CT_MS.cpp
*
* CLASS:	C_Complex_Type_Multistate
*
* DESCRIPTION:	This class is used to store a special complex type,
*		representing a multistate complex type. This type has a
*		number of state flags; each of these is an independent
*		attribute of the complex. Any complex of this type
*		will possess these state flags and will contains
*		values indicating whether each state is set or clear
*		(ie 1 or 0). Examples of these state flags include
*		phosphorylation sites, ligand-binding sites, being
*		in particular conformations.
*
* TYPE:			Core
*
*************************************************************************/

#include "_Stchstc.hh"

/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Clears all attributes and calls the ancestor constructor.
*
* PARAMETERS:   C_Application*  p_App  	        - Pointer to parent application
*
*       	C_Complex_Type*	 P_Old_Complex_Type - Complex type this special
*						      type is based upon
*		C_System*	 P_System	    - Pointer to the system 
*						      containing the complex
*						      type (ie the parent
*                                                     system).
*
*************************************************************************/

C_Complex_Type_Multistate::C_Complex_Type_Multistate
(C_Application* p_App, C_Complex_Type* P_Old_Complex_Type, C_System* P_System)
  : C_Complex_Type_Special (p_App, P_Old_Complex_Type, P_System)
{
  int nCount;
  m_nNumFlags = 0;
  m_nNumStates = 1;
  m_nNumReactions = 0;
  m_bHasRapidEqm = FALSE;
  m_pStateLevel = NULL;
  for (nCount = 0; nCount < MAX_MS_NUM_FLAGS; nCount ++)
    {
      m_bIsRpdEqm[nCount] = FALSE;
      m_pPrFlagSet[nCount] = NULL;
      m_lpszFlag[nCount][0] = NULL_CHAR;
      P_Reactions[nCount] = NULL;
    }
}
	
/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the special complex type. The state flags
*		are retrieved from the INI file. Then the initial states
*		for all the complexes of this type are set. Then reactions
*		are constructed, representing reactions in which this
*		complex type is involved.
*
* PARAMETERS:	C_Complex*	P_Complex       - Pointer to array of
*					          complexes in the system.
*		long		m_nNumComplexes - Number of complexes in the
*						  system.
*
* RETURNS:	Bool	TRUE	- The special complex type was initialised 
*				  successfully
*			FALSE	- An error occurred initialising the 
*				  special complex type
*
*************************************************************************/

Bool
C_Complex_Type_Multistate::Init(C_Complex** P_Complex, long m_nNumComplexes)
{
  char	lpszParameter		[MAX_INI_PARAMETER_LENGTH];
  char	lpszBitString		[MAX_INI_PARAMETER_LENGTH];
  char	lpszReactionDir		[MAX_INI_PARAMETER_LENGTH];
  char	lpszStateDisplayString	[MAX_INI_PARAMETER_LENGTH];
  char	lpszRelProb		[MAX_INI_PARAMETER_LENGTH];
  char	lpszMSFileSection	[MAX_INI_SECTION_LENGTH];
  char	lpszSectionName		[MAX_INI_SECTION_LENGTH];
  char	lpszReactions		[MAX_INI_VALUE_LENGTH];
  char	lpszStateFlags		[MAX_INI_VALUE_LENGTH];
  char	lpszRpdEqmFlag		[MAX_INI_VALUE_LENGTH];
  char	lpszWildcardsPara	[MAX_INI_VALUE_LENGTH_LONG];
  char	lpszDynCode		[MAX_INI_VALUE_LENGTH];
  char	lpszError		[MAX_MESSAGE_LENGTH];
  char	lpszParaList		[MAX_MESSAGE_LENGTH];
  ostringstream ParaListStream;
  char	lpszComplexFile		[MAX_FILE_NAME_LENGTH];
  char	lpszReactionList	[MAX_MS_NUM_REACTIONS][MAX_INI_VALUE_LENGTH];
  // Max number of wildcards
  //   = max length of wildcard list / (minimum number of flags + 1)
  char	lpszWildcards[MAX_INI_VALUE_LENGTH_LONG / 2][MAX_MS_NUM_FLAGS];
  char*	lpszPos;
  long	nDynParameters          [MAX_NUM_DYNAMIC_PARAMETERS];
  long	nReactionID;
  long	nNumRpdEqm;
  long	nCountEqm;
  long	nCountFlag;
  long	nCountComplex;
  long	nCountReaction;
  long	nMaxState;
  long	nConc;
  long	nPosNextComplex;
  long	nState;
  long	nWildcardMask=0;
  long	nWildcardValue=0;
  long	nNumWildcardStrings;
  long	nWildcard;
  long	nDefaultConc;
  long	nDefaultPr;
  long*	pConc;
  int	nFields;
  double flPrFlagSet;

  ifstream        Dump_Stream;
  long		  nFromDumpFile;
  char		  lpszDumpFile[MAX_INI_VALUE_LENGTH];
  char		  lpszDumpFilePath[MAX_FILE_NAME_LENGTH];
  char*           lpszColTitles;
  char	          lpszPrevTime[MAX_INI_LINE_LENGTH];
  char	          lpszSnapshotTime[MAX_INI_LINE_LENGTH];
  char	          lpszCTCode[MAX_STRING_LENGTH];
  char	          lpszCTConc[MAX_STRING_LENGTH];
  char	          lpszTmp[MAX_STRING_LENGTH];
  char	          lpszMSCode[MAX_STRING_LENGTH];
  char	          lpszMSState[MAX_STRING_LENGTH];
  char	          lpszStateString[MAX_STRING_LENGTH];
  char*	          lpszMSStatePos;
  char*	          lpszStateStringPos;
  char*           lpszSnapshotLine;
  char*           lpszPrevLine;
  char*	          lpszDummy;
  long            nColWidth;
  long            nColCount;
  long            nCTConc;
  long            nLineLength;
  long            i;
  double          flTime;
  double          flSnapshotTime;
  double          flPrevTime;
  Bool            bEOF;

  // Get name of file containing complex type information
  // (ie the complex INI file name)
	
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE,
		     lpszComplexFile))
    return FALSE;

  // Get name of file containing multistate complex type information

  strcpy(lpszParameter, Display_String());
  strcat(lpszParameter, INI_SPECIAL_TYPES_FILE);
  if (!m_pApp->Get_INI(INI_SPECIAL_TYPES_SECTION, lpszParameter,
		     m_lpszINIFile, lpszComplexFile))
    return FALSE;


  // Initialise state flags

  // Get state flags for this multistate complex from file

  if (!m_pApp->Get_INI(INI_MS_GENERAL_SECTION, INI_STATE_FLAGS,
		     lpszStateFlags, m_lpszINIFile))
    return FALSE;

  // Strip out state flags from string and add flags to array

  m_nNumFlags = 0;
  // Find first symbol in symbol list
  lpszPos = strtok(lpszStateFlags, INI_FLAG_SEPARATOR);
  // While the end of the symbol list is not reached and
  // the maximum number of flags is not exceeded...
  while ((lpszPos != NULL) && (m_nNumFlags < MAX_MS_NUM_FLAGS))
    {
      // If the length of the flag exceeds the maximum allowed, output error
      if (strlen(lpszPos) > MAX_MS_FLAG_LENGTH)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 82, lpszPos);
	  return FALSE;
	}
      strcpy(m_lpszFlag[m_nNumFlags], lpszPos);  // Set the new flag
      m_nNumFlags ++;            // Update count for number of flags
      lpszPos = strtok(NULL, INI_FLAG_SEPARATOR); // Find next flag
    }
  if (lpszPos != NULL)  // If the end of the list wasn't reached, output error
    {
      m_pApp->Message(MSG_TYPE_ERROR, 81);
      return FALSE;
    }
  m_nNumStates = PowerOfTwo(m_nNumFlags);
  nMaxState = m_nNumStates - 1;

  // Initialise array of levels to store the levels for each state

  // Attempt to create new array in which to store the levels of each state
  m_pStateLevel = new long[m_nNumStates];
  // If the memory allocation failed, output out of memory error
  if (m_pStateLevel == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8,
		    "levels of each state of the Complex Type");
      return FALSE;
    }
  // Initialise the levels of all states to zero
  for (i = 0; i < m_nNumStates; i ++)
    m_pStateLevel[i] = 0;	// For each state, reset the level to zero


  // Initialise initial states

  // Create array to store concentrations
  pConc = new long[nMaxState + 1];  // Create array for storing concentrations
  if (pConc == NULL)		// If the array was not created correctly...
    {
      // Output error
      m_pApp->Message(MSG_TYPE_ERROR, 8,
		     "concentrations in Multistate Complexes");
      return FALSE;
    }
  // Initialise array members to -1
  // (so that it is possible to detect which members have been set later on)
  for (nState = 0; nState <= nMaxState; nState ++)
    pConc[nState] = -1;

  // Attempt to get flag indicating whether or not initial levels will
  // be read from a dump file
  if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_INITIAL_LEVELS_FROM_DUMP_FILE,
		     &nFromDumpFile, lpszComplexFile, TRUE))
    {
      // If flag doesn't exist, set it to zero
      nFromDumpFile = 0;
    }
  // If the flag is set, use the dump file
  if (nFromDumpFile)
    {
      // Allocate memory to store each line from dump file
      // (We do this from the heap to avoid allocating too much off the stack)
      lpszColTitles = new char [MAX_INPUT_FROM_DUMP_LINE_LENGTH];

      // Get the name of the dump file to read from
      if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_INITIAL_LEVELS_FILE,
			 lpszDumpFile, lpszComplexFile))
	return FALSE;
      // Construct full path name for dump file
      sprintf(lpszDumpFilePath,"%s%s%s",m_pApp->m_lpszWorkingDir,
	      FILE_PATH_SEPARATOR, lpszDumpFile);
      // Get the time at which the state of the system should be read from
      if (!m_pApp->Get_INI(INI_GENERAL_SECTION,
			 INI_INITIAL_LEVELS_FROM_TIME,
			 &flTime, lpszComplexFile))
	return FALSE;
      // Output message to indicate that dump file is being used
      sprintf(lpszParaList, "%s%s%s%s%s%s%s", Display_String(),
	      PARAMETER_LIST_SEPARATOR, lpszDumpFilePath,
	      PARAMETER_LIST_SEPARATOR, INI_INITIAL_STATES_SECTION,
	      PARAMETER_LIST_SEPARATOR, m_lpszINIFile);
      m_pApp->Message(MSG_TYPE_STATUS, 192, lpszParaList);


      // Open dump file
      Dump_Stream.open(lpszDumpFilePath, ios::in);
#if  ( defined(_ALPHA) || defined(_AIX) )
      if (!Dump_Stream.rdbuf()->is_open())
#else
	if (!Dump_Stream.is_open())
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 29, lpszDumpFilePath);
	  return FALSE;
	}
      // Get first line
      Dump_Stream.getline(lpszColTitles,
			  sizeof(char) *  MAX_INPUT_FROM_DUMP_LINE_LENGTH);
      if (Dump_Stream.fail())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 6, lpszDumpFilePath);
	  return FALSE;
	}

      //Calculate column widths

      nColWidth = 0;
      // Find title for column (end of blank space)
      // (Column consists of eg "   Time|     A|     B|"
      //  where '|' is column divider)
      while (lpszColTitles[nColWidth] == VALUES_COLUMN_SEPARATOR[0]
	     || lpszColTitles[nColWidth] == ' ')
	nColWidth ++;
      // Find end of column title
      while (lpszColTitles[nColWidth] != VALUES_COLUMN_SEPARATOR[0])
	nColWidth ++;
      // Calculate the max column width by the width of the column titles
      // plus an extra column in case of overrun
      nLineLength = strlen(lpszColTitles) + nColWidth;

      // Allocate memory for strings

      lpszPrevLine = NULL;
      lpszSnapshotLine = NULL;
      // Try to allocate memory for line strings
      lpszPrevLine = new char[nLineLength];
      lpszSnapshotLine = new char[nLineLength];
      // If either of the strings were not allocated correctly
      if ((lpszPrevLine == NULL) || (lpszSnapshotLine == NULL))
	{
	  // Output error
	  m_pApp->Message(MSG_TYPE_ERROR, 8,
			"reading concentrations from dump file");
	  return FALSE;
	}

      // Read in first line

      // Get the first data line
      Dump_Stream.getline(lpszPrevLine, sizeof(char) * nLineLength);
      if (Dump_Stream.fail())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 6, lpszDumpFilePath);
	  return FALSE;
	}
      // Copy the time of the first line to a string
      strncpy(lpszPrevTime, lpszPrevLine, nColWidth);
      // Get the time of the first line
      flPrevTime = atof(lpszPrevTime);

      // Find the line closest to the chosen time

      // Try to get 2nd line
      Dump_Stream.getline(lpszSnapshotLine, sizeof(char) * nLineLength);
      // If there's only 1 line or 1st line > time we want
      if (flPrevTime > flTime || Dump_Stream.fail())
	{
	  // Set the line we are going to use - ie the previous
	  // line, by swapping pointers
	  lpszDummy = lpszPrevLine;
	  lpszPrevLine = lpszSnapshotLine;
	  lpszSnapshotLine = lpszDummy;
	}
      // Otherwise
      else
	{
	  // Copy the time of the current line to a string
	  strncpy(lpszSnapshotTime, lpszSnapshotLine, nColWidth);
	  // Get the time of the current line
	  flSnapshotTime = atof(lpszSnapshotTime);
	  bEOF = FALSE;
	  // Keep reading in lines until we've gone too far or
	  // end of file is reached
	  while ((flSnapshotTime < flTime) && (!bEOF))
	    {
	      // Save previous time for next iteration
	      flPrevTime = flSnapshotTime;
	      // Save line for next iteration by swapping pointers
	      lpszDummy = lpszPrevLine;
	      lpszPrevLine = lpszSnapshotLine;
	      lpszSnapshotLine = lpszDummy;
	      // Read line from file
	      Dump_Stream.getline(lpszSnapshotLine,sizeof(char) * nLineLength);
	      if (Dump_Stream.fail())
		{
		  m_pApp->Message(MSG_TYPE_ERROR, 6, lpszDumpFilePath);
		  return FALSE;
		}

	      // If we're not at the end of the file
	      if (!(bEOF = Dump_Stream.eof()))
		{
		  // Copy the time of the current line to a string
		  strncpy(lpszSnapshotTime, lpszSnapshotLine, nColWidth);
		  // Get the time of the current line
		  flSnapshotTime = atof(lpszSnapshotTime);
		}
	    }
	  // If time we want is closer to previous line than current one or
	  // we're at EOF
	  if (flTime - flPrevTime < flSnapshotTime - flTime || bEOF)
	    {
	      // Set the line we are going to use - ie the
	      // previous line, by swapping pointers
	      lpszDummy = lpszPrevLine;
	      lpszPrevLine = lpszSnapshotLine;
	      lpszSnapshotLine = lpszDummy;
	    }
	}
      // Close the dump file
      Dump_Stream.close();
      // Copy the time of the current line to a string
      strncpy(lpszSnapshotTime, lpszSnapshotLine, nColWidth);
      // Get the time of the current line
      flSnapshotTime = atof(lpszSnapshotTime);
      ParaListStream << setprecision(PREC_SNAPSHOT_TIME) << std::fixed;
      ParaListStream << flSnapshotTime;
      m_pApp->Message(MSG_TYPE_STATUS, 188, ParaListStream.str().c_str());
      
      // Loop through all the concentrations in the line extracted 
      for (nColCount = nColWidth + strlen(VALUES_COLUMN_SEPARATOR);
	   nColCount < (long) strlen(lpszSnapshotLine);
	   nColCount += nColWidth + strlen(VALUES_COLUMN_SEPARATOR))
	{
	  // Get the column heading (CT code)
	  strncpy(lpszCTCode, &(lpszColTitles[nColCount]), nColWidth);
	  lpszCTCode[nColWidth] = '\0';
	  // Trim spaces from ends
	  for (i = 0; lpszCTCode[i] == ' '; i++);
	  strcpy(lpszTmp, &(lpszCTCode[i]));
	  strcpy(lpszCTCode,lpszTmp);
	  for (i = 0; lpszCTCode[i] != ' ' && lpszCTCode[i] != '\0'; i++);
	  lpszCTCode[i] = '\0';
	  // Get the concentration
	  strncpy(lpszCTConc, &(lpszSnapshotLine[nColCount]), nColWidth);
	  // Convert the concentration to a long integer
	  nCTConc = atol(lpszCTConc);
	  // If this column represents a multistate CT
	  if ((lpszPos = strstr(lpszCTCode, FLAG_CT_SEPARATOR)) != NULL)
	    {
	      // Copy complex type code and state descriptor to string buffers
	      strncpy(lpszMSCode, lpszCTCode,
		      strlen(lpszCTCode) - strlen(lpszPos));
	      lpszMSCode[strlen(lpszCTCode) - strlen(lpszPos)] = '\0';
	      lpszPos++;
	      strcpy(lpszMSState, lpszPos);
	      // If this column matches this complex type
	      if (strcmp(Display_String(), lpszMSCode) == 0)
		{
		  // Loop through the flags and construct state string
		  lpszStateStringPos = lpszStateString;
		  lpszMSStatePos = lpszMSState;
		  while (*lpszMSStatePos != '\0')
		    {
		      while (*lpszMSStatePos != FLAG_SET_INDICATOR[0]
			     && *lpszMSStatePos != FLAG_CLEAR_INDICATOR[0]
			     && *lpszMSStatePos != '\0')
			{
			  lpszMSStatePos++;
			}
		      if (*lpszMSStatePos == '\0')
			{
			  break;
			}
		      if (*lpszMSStatePos == FLAG_SET_INDICATOR[0])
			{
			  *lpszStateStringPos = ON_BIT_CHAR;
			}
		      else
			{
			  if (*lpszMSStatePos == FLAG_CLEAR_INDICATOR[0])
			    *lpszStateStringPos = OFF_BIT_CHAR;
			}
		      lpszMSStatePos ++;
		      lpszStateStringPos ++;
		    }
		  *lpszStateStringPos = '\0';
		  // Check that length of the state string is correct
		  if ((long) strlen(lpszStateString) != m_nNumFlags)
		    {
		      m_pApp->Message(MSG_TYPE_ERROR, 190, 3,
				    Display_String(), lpszCTCode,
				    lpszDumpFilePath);
		      return FALSE;
		    }
		  // Set concentration of this state
		  pConc[BitStringToLong(m_pApp,lpszStateString)] = nCTConc;
		}
	    }
	}
      // Clear up memory
      if (lpszColTitles != NULL)
	delete[] lpszColTitles;
      if (lpszPrevLine != NULL)
	delete[] lpszPrevLine;
      if (lpszSnapshotLine != NULL)
	delete[] lpszSnapshotLine;

    }
  // Otherwise read initial state levels from INI file 
  else
    {
      // Read in any wildcard bit strings from the INI file
      // - ie strings which represent multiple states
	
      // Retrieve the list of wildcards from the INI file
      if (!m_pApp->Get_INI(INI_INITIAL_STATES_SECTION, INI_WILDCARD_STRINGS,
			 lpszWildcardsPara, m_lpszINIFile))
	return FALSE;
  
  // Find first wildcard string in list
      lpszPos = strtok(lpszWildcardsPara, INI_WILDCARD_SEPARATOR);
      // While the end of the list is not reached...
      while (lpszPos != NULL)
	{
	  // Ensure the length of this string matches the number of flags
	  if ((signed) strlen(lpszPos) != m_nNumFlags)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 107, lpszPos);
	      return FALSE;
	    }

	  // Retrieve the concentration from the INI file
	  if (!m_pApp->Get_INI(INI_INITIAL_STATES_SECTION, lpszPos, &nConc, m_lpszINIFile))
	    return FALSE;
	  // Work out the mask and value from the wildcard string
	  Extract_Wildcard_Mask(lpszPos, &nWildcardMask, &nWildcardValue);

      // Loop through all the possible states for this multistate complex
	  for (nState = 0; nState <= nMaxState; nState ++)
	    // If this state is defined by this wildcard string...
	    if ((nState & nWildcardMask) == nWildcardValue)
	      {
		// If this state concentration has not been set yet
		if (pConc[nState] == -1)
		  // Set this state
		  pConc[nState] = nConc;
		// Otherwise, output error indicating this state concentration
		// has already been set	    
		else
		  {
		    Get_State_Display_String(nState, lpszStateDisplayString);
		    m_pApp->Message(MSG_TYPE_ERROR, 108, lpszStateDisplayString);
		    return FALSE;
		  }
	      }
	  // Find next wildcard string
	  lpszPos = strtok(NULL, INI_WILDCARD_SEPARATOR);
	}

      // For each state the multistate complex can be in, create bit string and
      // load in the concentration from the INI file.
      // A bit string will be created - a binary string equivalent of nState.
      // This is only done for the states which haven't been initialised yet;
      // some concentrations will have been set already using wildcards above.

      nDefaultConc = 0;
      for (nState = 0; nState <= nMaxState; nState ++)
	// If state has not been set yet...
	if (pConc[nState] == -1)
	  {
	    // Create a bit string equivalent of the state
	    LongToBitString(nState, lpszBitString, m_nNumFlags);
	    // Retrieve the relative probability from the INI file.
	    if (!m_pApp->Get_INI(INI_INITIAL_STATES_SECTION, lpszBitString,
			       &(pConc[nState]), m_lpszINIFile, TRUE))
	      {
		// If not found, use default value
		pConc[nState] = MS_DEFAULT_CONC;
		// Increment number of default concentrations assigned
		nDefaultConc ++;
	      }
	  }
      // If default concentrations were assigned, output message
      if (nDefaultConc > 0)
	{
	  // Create list of parameters for warning
	  sprintf(lpszParaList, "%s%s%d%s%ld%s%s", "concentration",
		  PARAMETER_LIST_SEPARATOR, MS_DEFAULT_CONC,
		  PARAMETER_LIST_SEPARATOR, nDefaultConc,
		  PARAMETER_LIST_SEPARATOR, Display_String());
	  // Output warning that default value is being used
	  m_pApp->Message(MSG_TYPE_STATUS, 122, lpszParaList);
	}
    }
  // For each possible state, attempt to set up complexes to the
  // appropriate state

  nPosNextComplex = 0;
  // For every possible state of this complex type ...
  for (nState = 0; nState <= nMaxState; nState ++)
    // For each multistate complex that must be initialised with this state ...
    for (nCountComplex = 0; nCountComplex < pConc[nState]; nCountComplex ++)
      {
	// Find the next complex in the system of this type
	// (ie multistate complex type)
	while (P_Complex[nPosNextComplex]->Get_Type() != this) 
	  {
	    nPosNextComplex ++;
	    // If the end of the complex list is reached, output error
	    if (nPosNextComplex >= m_nNumComplexes)
	      {
		m_pApp->Message(MSG_TYPE_ERROR, 94);
		return FALSE;
	      }
	  }
	// Initialise the state of the complex
	((C_Complex_Multistate*) P_Complex[nPosNextComplex])
	  -> Set_State(nState);
	// Increment the counter for this state
	m_pStateLevel[nState] ++;
	// Increment the position counter
	nPosNextComplex ++;
      }
  // Attempt to find any other complexes of this type whose state
  // have not been set
  while (nPosNextComplex < m_nNumComplexes)
    {
      // If a multistate complex type is found, output error
      if (P_Complex[nPosNextComplex]->Get_Type() == this)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 95);
	  return FALSE;
	}
      nPosNextComplex ++;
    }


  // Initialise reactions

  // Get the reactions that this multistate complex is involved in
  if (!m_pApp->Get_INI(INI_MS_GENERAL_SECTION, INI_REACTION_IDS, lpszReactions,
		     m_lpszINIFile))
    return FALSE;

  // Extract reaction IDs from reaction list into an array

  m_nNumReactions = 0;
  // Find first reaction ID in list
  lpszPos = strtok(lpszReactions, INI_MS_REACTION_SEPARATOR);
  // While the end of the reaction list is not reached and
  // the maximum number of reactions is not exceeded...
  while ((lpszPos != NULL) && (m_nNumReactions < MAX_MS_NUM_REACTIONS))
    {
      // Add reaction ID to array
      strcpy(lpszReactionList[m_nNumReactions], lpszPos);
      m_nNumReactions ++;
      // Find next reaction
      lpszPos = strtok(NULL, INI_MS_REACTION_SEPARATOR);
    }
  // If the end of the list wasn't reached, output error
  if (lpszPos != NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 85);
      return FALSE;
    }

  // Loop through reaction array and construct appropriate
  // C_Complex_Type_Multistate_Reaction

  for (nCountReaction = 0; nCountReaction < m_nNumReactions; nCountReaction ++)
    {
      // Extract the reaction ID and direction from the ID list
      nFields = sscanf(lpszReactionList[nCountReaction], "%ld%s",
		       &nReactionID, lpszReactionDir);
      // If an incorrect number of fields were extracted,
      // or the direction code is not recognised...
      if ((nFields != 2)
	  || ((strcmp(lpszReactionDir, INI_FORWARDS_REACTION) != 0)
	      && (strcmp(lpszReactionDir, INI_REVERSE_REACTION) != 0)))
	{
	  // Output error
	  m_pApp->Message(MSG_TYPE_ERROR, 86, lpszReactionList[nCountReaction]);
	  return FALSE;
	}
      // If it is a forwards reaction
      if (strcmp(lpszReactionDir, INI_FORWARDS_REACTION) == 0)
	P_Reactions[nCountReaction]
	  = new C_Complex_Type_Multistate_Reaction(m_pApp, nReactionID);
      
      else  // Otherwise create a reverse reaction
	P_Reactions[nCountReaction]
	  = new C_Complex_Type_Multistate_Reaction(m_pApp, nReactionID 
						   + REVERSE_REACTION_ID_OFFSET);
      // Create the section name containing details of the reaction
      strcpy(lpszMSFileSection, INI_REACTION_SECTION);
      strcat(lpszMSFileSection, lpszReactionList[nCountReaction]);
      // Initialise probabilities in reaction.
      // If initialisation fails, output error
      if (!P_Reactions[nCountReaction]->Init(m_lpszINIFile, lpszMSFileSection,
					     m_nNumFlags, m_lpszFlag, this,
					     P_Parent->Get_Event_Manager()))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 87, 1, nReactionID);
	  return FALSE;
	}
    }

	
  // Initialise rapid equilibria

  // Get the number of rapid equilibria
  if (!m_pApp->Get_INI(INI_MS_GENERAL_SECTION, INI_MS_NUM_RAPID_EQM,
		     &nNumRpdEqm, m_lpszINIFile))
    return FALSE;

  // Set flag specifying whether this complex type contains any rapid eqm's
  if (nNumRpdEqm > 0)
    m_bHasRapidEqm = TRUE;

  // For each rapid equilibrium, read in the state flag
  // and associated probabilities

  nDefaultPr = 0;
  for (nCountEqm = 0; nCountEqm < nNumRpdEqm; nCountEqm ++)
    {

      // Read in the state flag this rapid equilibrium refers to

      // Create section name from rapid equilibrium number
      sprintf(lpszSectionName, "%s%ld", INI_MS_RAPID_EQM_SECTION,
	      nCountEqm + 1);
      // Get the state flag for this eqm
      if (!m_pApp->Get_INI(lpszSectionName, INI_RPD_EQM_FLAG, lpszRpdEqmFlag,
			 m_lpszINIFile, TRUE))
	{
	  // For backwards compatibility
	  if (!m_pApp->Get_INI(lpszSectionName, INI_RPD_EQM_STATE,
			     lpszRpdEqmFlag, m_lpszINIFile))
	    return FALSE;
	}
      nCountFlag = 0;
      // Try to identify the state flag this rpd eqm refers to
      while (strcmp(lpszRpdEqmFlag, m_lpszFlag[nCountFlag]) != 0)
	{
	  nCountFlag ++;
	  // If the flag has not been found, output error
	  if (nCountFlag >= m_nNumFlags)
	    {
	      sprintf(lpszError, "%s%s%ld", lpszRpdEqmFlag,
		      PARAMETER_LIST_SEPARATOR, nCountEqm + 1);
	      m_pApp->Message(MSG_TYPE_ERROR, 91, lpszError);
	      return FALSE;
	    }
	}

      // Ensure this state flag is not already controlled
      // by another rapid equilibrium

      // If the rapid eqm has already been defined
      if (m_bIsRpdEqm[nCountFlag])
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 93, 1, nCountEqm + 1); // Output error
	  return FALSE;
	}
      // Indicate this flag is controlled by a rapid equilibrium
      m_bIsRpdEqm[nCountFlag] = TRUE;

      // Initialise array to store probabilities

      // Attempt to create array to store probabilities for this flag
      m_pPrFlagSet[nCountFlag] = new double[nMaxState + 1];
      // If the array was not created successfully, output error
      if (m_pPrFlagSet[nCountFlag] == NULL)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 8, "array to store rapid equilibria");
	  return FALSE;
	}

      // Initialise probability array to rogue value so it will be possible 
      // to detect which probabilities have been set
      for (nState = 0; nState <= nMaxState; nState ++)
	m_pPrFlagSet[nCountFlag][nState] = -1;

      // Read in any wildcard bit strings from the INI file - ie strings
      // which represent multiple states. and store them in an array
	
      nNumWildcardStrings = 0;
      // Retrieve the list of wildcards from the INI file
      if (!m_pApp->Get_INI(lpszSectionName, INI_WILDCARD_STRINGS,
			 lpszWildcardsPara, m_lpszINIFile))
	
	return FALSE;
      // Find first wildcard string in list
      lpszPos = strtok(lpszWildcardsPara, INI_WILDCARD_SEPARATOR);
      // While the end of the list is not reached...
      while (lpszPos != NULL)
	{
	  // Ensure the length of this string matches the number of flags
	  if ((signed) strlen(lpszPos) != m_nNumFlags)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 107, lpszPos);
	      return FALSE;
	    }
	  strcpy(lpszWildcards[nNumWildcardStrings], lpszPos);
	  nNumWildcardStrings ++;
	  // Find next wildcard string
	  lpszPos = strtok(NULL, INI_WILDCARD_SEPARATOR);
	}

      // Loop through each wildcard string and set up the probabilities
      // for the states they represent

      for (nWildcard = 0; nWildcard < nNumWildcardStrings; nWildcard ++)
	{
	  // Redefine lpszPos for convenience
	  lpszPos = lpszWildcards[nWildcard];
	  // Retrieve the relative probabilities from the INI file
	  if (!m_pApp->Get_INI(lpszSectionName, lpszPos, lpszRelProb,
			     m_lpszINIFile))
	    return FALSE;
	  // Extract rapid equilibrium probability from probability string
	  if (!Extract_Rapid_Eqm_Probability(lpszRelProb, nCountEqm + 1,
					     &flPrFlagSet, lpszDynCode))
	    return FALSE;
	  // Work out the mask and value from the wildcard string
	  Extract_Wildcard_Mask(lpszPos, &nWildcardMask, &nWildcardValue);
	  // Loop through all the possible states for this multistate complex
	  for (nState = 0; nState <= nMaxState; nState ++)
	    // If this state is defined by this wildcard string...
	    if ((nState & nWildcardMask) == nWildcardValue)
	      {
		// If probability for this state has already been set
		if (m_pPrFlagSet[nCountFlag][nState] != -1)	
		  {
		    // Output error indicating this probability has
		    // already been set
		    Get_State_Display_String(nState, lpszStateDisplayString);
		    m_pApp->Message(MSG_TYPE_ERROR, 110, lpszStateDisplayString);
		    return FALSE;
		  }
		// Store the probability that this flag will be set
		// at any given time
		m_pPrFlagSet[nCountFlag][nState] = flPrFlagSet;
	      }
	  // If the probability can change over time
	  // (ie this is a dynamic object), attempt to create events
	  if (strlen(lpszDynCode) > 0)
	    {
	      // Set up parameter array to pass to event manager
	      nDynParameters[MS_EVENT_PARAM_FLAG] = nCountFlag;
	      nDynParameters[MS_EVENT_PARAM_WILDCARD_MASK] = nWildcardMask;
	      nDynParameters[MS_EVENT_PARAM_WILDCARD_VALUE] = nWildcardValue;

	      if (!(P_Parent -> Get_Event_Manager()
		    -> Init_Dynamic_Object(this, lpszDynCode, 1, 1,
					   nDynParameters, 3)))
		{
		  m_pApp->Message(MSG_TYPE_ERROR, 118, 1, nCountEqm + 1);
		  return FALSE;
		}
	    }
	}
      
      // For each state the multistate complex can be in, create bit string
      // and load in the probability for this rapid equilbrium from the INI
      // file.
      // A bit string will be created - a binary string equivalent of nState.
      // This is only done for the states which haven't been initialised yet;
      // some probabilities will have been set already using wildcards above.

      for (nState = 0; nState <= nMaxState; nState ++)
	// If probability for this state has not been set yet
	if (m_pPrFlagSet[nCountFlag][nState] == -1)
	  {
	    // Create a bit string equivalent of the state
	    LongToBitString(nState, lpszBitString, m_nNumFlags);
	    // Assume that this value cannot change over time
	    // (ie this is not a dynamic object)
	    lpszDynCode[0] = NULL_CHAR;
	    // Retrieve the relative probabilities from the INI file.
	    if (!m_pApp->Get_INI(lpszSectionName, lpszBitString, lpszRelProb,
			       m_lpszINIFile, TRUE))
	      {
		// If not found, use default value
		flPrFlagSet = MS_DEFAULT_RE_PR_SET;
		// Count number of times we used a default probability
		nDefaultPr ++;
	      }
	    // If found, extract rapid equilibrium probability
	    // from probability string	    
	    else if (!Extract_Rapid_Eqm_Probability(lpszRelProb, nCountEqm + 1,
						    &flPrFlagSet, lpszDynCode))
		return FALSE;
	    m_pPrFlagSet[nCountFlag][nState] = flPrFlagSet;
	    // If the probability can change over time (ie this is a dynamic
	    // object), attempt to create events
	    if (strlen(lpszDynCode) > 0)
	      {
		// Set up parameter array to pass to event manager
		nDynParameters[MS_EVENT_PARAM_FLAG] = nCountFlag;
		// (Set mask to all 1s which is equivalent to 0xFFFFFFFF = -1)
		nDynParameters[MS_EVENT_PARAM_WILDCARD_MASK] = -1;
		nDynParameters[MS_EVENT_PARAM_WILDCARD_VALUE] = nState;
		if (!(P_Parent -> Get_Event_Manager()
		      -> Init_Dynamic_Object(this, lpszDynCode, 1, 1,
					     nDynParameters, 3)))
		  {
		    m_pApp->Message(MSG_TYPE_ERROR, 118, 1, nCountEqm + 1);
		    return FALSE;
		  }
	      }
	  }
    }

  if (nDefaultPr > 0)		// If default probabilities were assigned
    {
      // Create list of parameters for warning
      sprintf(lpszParaList, "%s%s%d%s%ld%s%s",
	      "rapid equilibrium probability", PARAMETER_LIST_SEPARATOR,
	      MS_DEFAULT_RE_PR_SET, PARAMETER_LIST_SEPARATOR, nDefaultPr,
	      PARAMETER_LIST_SEPARATOR, Display_String());
      // Output warning that default value is being used
      m_pApp->Message(MSG_TYPE_STATUS, 122, lpszParaList); 
    }

  // Delete temporary arrays

  if (pConc != NULL)
    delete[] pConc;
  
  return TRUE;
}





/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Deletes the reactions
*
*************************************************************************/

C_Complex_Type_Multistate::~C_Complex_Type_Multistate(void)
{
  long nCount;
  for (nCount = 0; nCount < m_nNumReactions; nCount ++)
    if (P_Reactions[nCount] != NULL)
      delete P_Reactions[nCount];
  if (m_pStateLevel != NULL)
    delete[] m_pStateLevel;
  for (nCount = 0; nCount < m_nNumFlags; nCount ++)
    if (m_pPrFlagSet[nCount] != NULL)
      delete[] m_pPrFlagSet[nCount];
}

	


/*************************************************************************
*
* METHOD NAME:	Create_New_Complex
*
* DESCRIPTION:	Creates a new complex object for this complex type and
*		returns a pointer to the newly created object.
*
* PARAMETERS:  	long nSystemID - ID this complex has in the system.
*				 (Actually the # in the array of C_Complex
*				 in C_System).
*
* RETURNS:	C_Complex* - Pointer to new complex of this type.
*
*************************************************************************/

C_Complex*
C_Complex_Type_Multistate::Create_New_Complex(long nSystemID)
{
  C_Complex* P_Complex;
  // Attempt to create new C_Complex_Multistate object
  P_Complex = new C_Complex_Multistate(m_pApp, P_Parent, nSystemID);
  
  if (P_Complex != NULL)  // If creation was successful...
    {
      // Set complex object to be of this complex type
      P_Complex->Set_Type(this);
      P_Complex->Set_Exists(TRUE);  // Indicate this complex object exists
    }
  else  // Otherwise output error
    m_pApp->Message(MSG_TYPE_ERROR, 8, "new Multistate Complex");		
  return P_Complex;		// Return a pointer to this complex object
}




/*************************************************************************
*
* METHOD NAME:	Get_Relative_Probability
*
* DESCRIPTION:	Retrieves the probability associated with a particular
*		reaction for a particular state of the multistate complex.
*		The relative probability is returned.
*
* PARAMETERS:	C_Reaction*	P_Reaction - Pointer to reaction in matrix
*		long		nState	   - State of multistate complex
*
* RETURNS:	double	- Relative probability for this multistate complex
*			  reacting in reaction nReaction when in the state
*			  nState.
*
*************************************************************************/

double
C_Complex_Type_Multistate::Get_Relative_Probability(C_Reaction* P_Reaction,
						    long nState)
{
  long nCount;
  nCount = 0;
  // Loop through all the reactions until the correct ID is found
  for (nCount = 0; nCount < m_nNumReactions; nCount ++)
    if (P_Reactions[nCount]->Get_Reaction_ID() == P_Reaction->Get_ID())
      break;

  // If the last reaction has been passed, this is not an MS-Reaction,
  // so return a relative probability of 1.
  if (nCount >= m_nNumReactions)
    return 1;
  else
    // Otherwise, return the state-dependent relative reaction probability
    return (P_Reactions[nCount]->Get_Relative_Probability(nState));
}



/*************************************************************************
*
* METHOD NAME:	Get_Flag_Modifiers
*
* DESCRIPTION:	Retrieves the bit strings indicating which flags in the
*		state need clearing and setting for a particular reaction.
*
* PARAMETERS:	C_Reaction*  P_Reaction      - Pointer to reaction in matrix
*		long*	     p_nFlagsToClear - Bit string for flags to clear
*		long*	     p_nFlagsToSet   - Bit string for flags to set
*
*************************************************************************/

void
C_Complex_Type_Multistate::Get_Flag_Modifiers(C_Reaction* P_Reaction,
					      long* p_nFlagsToClear,
					      long* p_nFlagsToSet)
{
  long nCount;
  nCount = 0;
  // Loop through all the reaction until the correct ID is found
  // Loop through all the reaction until the correct ID is found
  for (nCount = 0; nCount < m_nNumReactions; nCount ++)
    if (P_Reactions[nCount]->Get_Reaction_ID() == P_Reaction->Get_ID())
      break;
  // If the last reaction has been passed, this is not an MS-Reaction,
  // so do nothing and return.
  if (nCount >= m_nNumReactions)
    {
      *p_nFlagsToClear = 0;
      *p_nFlagsToSet = 0;
      return;
    }
  else
    // Otherwise, this get the flag modifiers for this reaction 
      ((C_Complex_Type_Multistate_Reaction*) P_Reactions[nCount])
      -> Get_Flag_Modifiers(p_nFlagsToClear, p_nFlagsToSet);
	  
}




/*************************************************************************
*
* METHOD NAME:	Get_Variables
*
* DESCRIPTION:	Creates an output variable for storage or display purposes. 
*		First the base class is called to create a variable
*		representing the entire complex type, then variables 
*		are constructed for each state.
*
* PARAMETERS:	C_Output_Variable** P_Variable - Pointer to array of
*					         variables to be populated
*		long*		    p_nNumVar  - Pointer to number
*						 of variables which are to
*						 be created
*		long		    nMaxNumVar - Maximum number of variables
*						 which can be created
*
* RETURNS:	Bool	TRUE		  - Variables were created successfully
*			FALSE		  - An error occurred while creating
*					    variables
*			&P_Variable	  - Populated array of pointers
*			&p_nNumVar	  - Number of variables created
*
*************************************************************************/

Bool
C_Complex_Type_Multistate::Get_Variables(C_Output_Variable** P_Variable,
					 long* p_nNumVar, long nMaxNumVar)
{
  long			nState;
  long			P_List_Of_States[1];
  C_Complex_Type*	P_List_Of_CT[1];
  char			lpszDisplayString[MAX_STRING_LENGTH];
  // Create list of complex types which the variable will represent
  P_List_Of_CT[0] = this;
  // Call base class to create variables for the whole complex type
  C_Complex_Type_Special::Get_Variables(P_Variable, p_nNumVar, nMaxNumVar);

  // Loop through every state this multistate complex type can be in
  for (nState = 0; nState < m_nNumStates; nState ++)
    {
      // Create list of states which the variable will represent
      P_List_Of_States[0] = nState;
      // If there is insufficient room to store further variables, output error
      if (nState + (*p_nNumVar) >= nMaxNumVar)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 57);
	  return FALSE;
	}

      // Create display string for a complex type in this state
      Get_State_Display_String(nState, lpszDisplayString);

      // Create new multistate variable using list of types and states
      P_Variable[nState + (*p_nNumVar)] = 
	new C_Output_Variable_Multistate(m_pApp, lpszDisplayString,
					 P_List_Of_CT, 1,
					 P_List_Of_States, 1);
    }				
  // Update the number of variables which have been created
  (*p_nNumVar) += nState; 
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Inc_Level
*
* DESCRIPTION:	Increments the level of this complex type. A record is kept
*		of how many complexes are in which state, and the
*		appropriate state counter is incremented.
*
* PARAMETERS:	C_Complex* P_Complex - Complex being recorded. This contains
*				       the state of interest.
*
*************************************************************************/

void
C_Complex_Type_Multistate::Inc_Level(C_Complex* P_Complex)
{
  // Call base class method to increment the overall level of this complex type
  C_Complex_Type_Special::Inc_Level(P_Complex);

#ifndef _DYNAMIC_UPDATE
  // Equilibrate before we get the state
  P_Complex -> Equilibrate();
#endif /* _DYNAMIC_UPDATE */
  
  // Increment the counter for the state of this complex
  m_pStateLevel[ ((C_Complex_Multistate*) P_Complex) -> Get_State() ] ++;
}


// This method is only for builds with output averaging enabled!!
#ifdef _DYNAMIC_UPDATE
/*************************************************************************
*
* METHOD NAME:	Dec_Level
*
* DESCRIPTION:	Decrements the level of this complex type. A record is kept
*		of how many complexes are in which state, and the
*		appropriate state counter is decremented.  Unlike Inc_Level(),
*               this method is only required for _DYNAMIC_UPDATE builds.
*
* PARAMETERS:	C_Complex* P_Complex - Complex being recorded. This contains
*				       the state of interest.
*
*************************************************************************/

void
C_Complex_Type_Multistate::Dec_Level(C_Complex* P_Complex)
{
  // Call base class method to decrement the overall level of this complex type
  C_Complex_Type_Special::Dec_Level(P_Complex);
  // Decrement the counter for the state of this complex
  m_pStateLevel[ ((C_Complex_Multistate*) P_Complex) -> Get_State() ] --;
}

#endif /* _DYNAMIC_UPDATE */

/*************************************************************************
*
* METHOD NAME:	Reset_Level
*
* DESCRIPTION:	Resets the level of this complex type. A record is kept
*		of how many complexes are in which state, and 
*		all the state counters are reset.
*
*************************************************************************/

void
C_Complex_Type_Multistate::Reset_Level(void)
{
  long nCount;
  // Call base class method to reset the overall level of this complex type
  C_Complex_Type_Special::Reset_Level();
  for (nCount = 0; nCount < m_nNumStates; nCount ++)
    m_pStateLevel[nCount] = 0;	// For each state, reset the level to zero
}



/*************************************************************************
*
* METHOD NAME:	Get_State_Display_String
*
* DESCRIPTION:	Makes a string from the display string of this complex type
*		and the states of this complex type eg TTWWAAmap.
*
* PARAMETER:	long  nState	      -	The state of the complex type the
*					string should reflect.
*		char* lpszStateString -	The string where the display string
*					should be stored.
*
* RETURNS:	char* lpszStateString - This is set to the display string
*					on return.
*
*************************************************************************/

void
C_Complex_Type_Multistate::Get_State_Display_String(long nState,
						    char* lpszStateString)
{
  int		nCount;
  long	nMask;
  // Start the string using the display string of the ancestor complex type
  strcpy(lpszStateString, Display_String());
  // Add a symbol to separate the complex type string
  // from the state flag indicators
  strcat(lpszStateString, FLAG_CT_SEPARATOR);
  nMask = PowerOfTwo(m_nNumFlags - 1);
  for (nCount = 0; nCount < m_nNumFlags; nCount++)  // For each state flag...
    {
      // If flag is set, use set flag indicator;
      // if it is clear, use clear flag indicator
      strcat(lpszStateString, ((nState & nMask) 
			       ? FLAG_SET_INDICATOR : FLAG_CLEAR_INDICATOR));
      // Add the state flag string to the new string
      strcat(lpszStateString, m_lpszFlag[nCount]);
      nMask >>= 1;  // Move the mask along one, to the next flag
    }
}




/*************************************************************************
*
* METHOD NAME:	Get_Variables_From_File
*
* DESCRIPTION:	Creates output variables for storage or display purposes. 
*		First the base class is called to create variables
*		required in the default implementation of this class, then
*		variables are constructed according to information in the 
*		INI file.
*
* PARAMETERS:	C_Output_Variable** P_Variable - Pointer to array of
*					         variables to be populated
*		long*		    p_nNumVar  - Pointer to number
*					         of variables which are to
*					         be created
*		long		    nMaxNumVar - Maximum number of variables
*						 which can be created
*               Bool                bAveraging - Flag indicating whether or
*                                                not output averaging is
*                                                desired
*
* RETURNS:	Bool	TRUE	     - Variables were created successfully
*			FALSE	     - An error occurred while creating
*				       variables
*			&P_Variable  - Populated array of pointers
*			&p_nNumVar   - Number of variables created
*
*************************************************************************/

Bool
C_Complex_Type_Multistate::Get_Variables_From_File(C_Output_Variable** P_Variable,
						   long* p_nNumVar,
						   long nMaxNumVar,
						   Bool bAveraging)
{
  C_Complex_Type* P_List_Of_CT[1];
  char			lpszDispVarName[MAX_INI_VALUE_LENGTH];
  char			lpszDispVarStates[MAX_INI_VALUE_LENGTH_LONG];
  char			lpszSection[MAX_INI_SECTION_LENGTH];
  char*			lpszPos;
  long			nState;
  long			nNumVar;
  long			nNumStates;
  long			nCountVar;
  long			nWildcardMask=0;
  long			nWildcardValue=0;
  long*			pStates;

  // Call base class to create variables for the whole complex type
  C_Complex_Type_Special::Get_Variables_From_File(P_Variable, p_nNumVar,
						  nMaxNumVar, bAveraging);
	
  // Get the number of variables from the file
  if (!m_pApp->Get_INI(INI_MS_GENERAL_SECTION, INI_MS_NUM_DISPLAY_VAR, &nNumVar,
		     m_lpszINIFile))
    return FALSE;
  if ((nNumVar < 0) || (nNumVar + *p_nNumVar > MAX_NUM_DISPLAY_VARIABLES))
    {
      m_pApp->Message(MSG_TYPE_ERROR, 33);
      return FALSE;
    }

  // Create array to store states the variable can represent
  pStates = new long[m_nNumStates];
	
  // For each variable, read in the information from the INI file

  for (nCountVar = *p_nNumVar; nCountVar < *p_nNumVar + nNumVar; nCountVar++)
    {									
      // Construct section title
      sprintf(lpszSection, "%s%ld", INI_MS_DISPLAY_VAR_SECTION, nCountVar + 1);

      // Read name of display variable
      if (!m_pApp->Get_INI(lpszSection, INI_MS_DISPLAY_VAR_NAME,
			 lpszDispVarName, m_lpszINIFile))
	return FALSE;
      // Read states this display variable represents
      if (!m_pApp->Get_INI(lpszSection, INI_MS_DISPLAY_VAR_STATES,
			 lpszDispVarStates, m_lpszINIFile))
	return FALSE;

      // Identify the states this variable represents from the list.
      // The states in this list can be wildcard strings.

      nNumStates = 0;
      // Find first state
      lpszPos = strtok(lpszDispVarStates, INI_VAR_STATES_SEPARATOR);
      // While the end of the list is not reached...
      while ((lpszPos != NULL) && (nNumStates < m_nNumStates))
	{
	  // Ensure the length of this string matches the number of flags
	  if ((signed) strlen(lpszPos) != m_nNumFlags)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 107, lpszPos);
	      return FALSE;
	    }
	  // Work out the mask and value from the wildcard string
	  Extract_Wildcard_Mask(lpszPos, &nWildcardMask, &nWildcardValue);
	  nState = 0;
	  // Loop through all the possible states for this multistate complex
	  //   to see which states this wildcard string represents
	  while ((nState < m_nNumStates) && (nNumStates < m_nNumStates))
	    {
	      // If this state is defined by this wildcard string...
	      if ((nState & nWildcardMask) == nWildcardValue)
		{
		  // Add the state to a list
		  pStates[nNumStates] = nState;
		  nNumStates ++;
		}
	      nState++;
	    }
	  // Find next state in list
	  lpszPos = strtok(NULL, INI_VAR_STATES_SEPARATOR);
	}
      // If the end of the list wasn't reached, output error
      if (lpszPos != NULL)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 100, lpszDispVarName);
	  return FALSE;
	}

      // Create the output variable using the list of complex types and states

      P_List_Of_CT[0] = this;

      if (!bAveraging)
	P_Variable[nCountVar]
	  = new C_Output_Variable_Multistate(m_pApp, lpszDispVarName,
					     P_List_Of_CT, 1,
					     pStates, nNumStates);
      else
	P_Variable[nCountVar] 
	  = new C_Output_Variable_Multistate_Averaged(m_pApp, lpszDispVarName,
						      P_List_Of_CT, 1, pStates,
						      nNumStates);
    }

  delete[] pStates;
  *p_nNumVar += nNumVar;  // Update number of variables created
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Extract_Wildcard_Mask
*
* DESCRIPTION:	Creates a mask and a value to from a wildcard string. The
*		wildcard string represents a number of states. The mask is 
*		used to indicate which bits define which states the wildcard 
*		string applies to; the value will be the value of the bits in 
*		the mask for the states this wildcard string applies to.
*		eg If wildcard string is 01?0, mask is 1101, value is 0100.
*		Therefore the wildcard string represents 0100 and 0110.
*
* PARAMETERS:	char*	lpszWildcard	- Wildcard string
*		long*	p_nMask		- Pointer to mask
*		long*	p_nValue	- Pointer to value
*
* RETURNS:	Bool	TRUE		  - Extraction was successful
*			FALSE		  - An error occurred during extraction
*			*p_nMask			  - Populated mask
*			*p_nValue			  - Populated value
*
*************************************************************************/

Bool
C_Complex_Type_Multistate::Extract_Wildcard_Mask(char* lpszWildcard,
						 long* p_nMask, long* p_nValue)
{
  int		nCount;
  long	nDigitValue;
  long	nWildcardMask;
  long	nWildcardValue;
  nWildcardMask = 0;
  nWildcardValue = 0;
  // Loop through each character in wildcard string
  for (nCount = 0; nCount < (signed) strlen(lpszWildcard); nCount ++)
    {
      nDigitValue = PowerOfTwo(m_nNumFlags - nCount - 1);
      switch (lpszWildcard[nCount])
	{
	  // If the bit is '1', the digit must be added to the mask and value
	case ON_BIT_CHAR:
	  nWildcardMask += nDigitValue;
	  nWildcardValue += nDigitValue;
	  break;
	  // If the bit is '0', the digit must be added to the mask alone
	case OFF_BIT_CHAR:
	  nWildcardMask += nDigitValue;
	  break;
	  // If the bit is the wildcard character, don't do anything
	case INI_WILDCARD_CHAR:
	  break;
	  // If the character is not recognised, output error
	default:
	  m_pApp->Message(MSG_TYPE_ERROR, 105, lpszWildcard);
	  return FALSE;
	}
    }
  *p_nMask = nWildcardMask;
  *p_nValue = nWildcardValue;
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Extract_Rapid_Eqm_Probability
*
* DESCRIPTION:	Extracts the relative probability that a state flag is set
*		according to a rapid equilibrium, using a string containing
*		the probability that the flag is clear and the probability
*		the flag is set. eg "1.0,2.0". In this case the value
*		returned, the relative probability that the flag will be set
*		is 2/(1+2) = 0.67. Alternatively, if only one value is
*		specified, that is used as the relative probability that the
*		flag is set.
*
* PARAMETERS:	char*	lpszRelProb	- Pointer to string containing relative
*					  probabilities
*		long	nEqmNum		- Number of equilibrium (used in error
*					  messages)
*		double*	pPrFlagSet	- Pointer to double that will store
*					  the relative probability
*		char*	lpszDynCode	- String representing code identifying
*					  a probability that changes over time
*					  (this is used to create events)
*
* RETURNS:	Bool	TRUE		- Extraction was successful
*			FALSE		- An error occurred during extraction
*			*pPrFlagSet	- Populated relative probability
*			*lpszDynCode	- Populated dynamic object, code string
*
*************************************************************************/

Bool
C_Complex_Type_Multistate::Extract_Rapid_Eqm_Probability (char* lpszRelProb,
							  long nEqmNum,
							  double* pPrFlagSet,
							  char* lpszDynCode)
{
  char*	lpszRelProbSeparator;
  char*	lpszPosDynCode;
  char	lpszRelProbSet[MAX_INI_PARAMETER_LENGTH];
  double	flPrSet;
  double	flPrClear;

  lpszDynCode[0] = NULL_CHAR;
  // Look for a dynamic object code in the probability
  lpszPosDynCode = strstr(lpszRelProb, INI_RE_DYN_CODE_SEPARATOR);
  // If a dynamic object code is specified with the probability
  if (lpszPosDynCode != NULL)
    {
      // Extract the dynamic object code
      strcpy(lpszDynCode, lpszPosDynCode + strlen(INI_RE_DYN_CODE_SEPARATOR));
      // Null terminate the relative probability(s)
      lpszPosDynCode[0] = NULL_CHAR;
    }

  // Find the separator dividing the two probabilities
  lpszRelProbSeparator = strstr(lpszRelProb, INI_RPD_EQM_SEPARATOR);
  // If there are two probabilities specified eg 0.5,0.25
  if (lpszRelProbSeparator != NULL)
    {
      // Null-terminate the first probability
      lpszRelProbSeparator[0] = NULL_CHAR;
      // Copy the probability after the separator
      strcpy(lpszRelProbSet,
	     lpszRelProbSeparator + strlen(INI_RPD_EQM_SEPARATOR));
      flPrClear = atof(lpszRelProb);  // Convert the probabilities to doubles
      flPrSet = atof(lpszRelProbSet);
      // If probabilities are invalid
      // (ie if either are negative or sum to zero)
      if ((flPrClear + flPrSet == 0) || (flPrSet < 0) || (flPrClear < 0))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 92, 1, nEqmNum);  // Output error
	  return FALSE;
	}
      // Calculate relative probability that flag is set
      *pPrFlagSet = flPrSet / (flPrSet + flPrClear);
    }
  else  // Otherwise, if only one probability is specified
    {
      flPrSet = atof(lpszRelProb);  // Convert the probability to a double
      // If probability is invalid (less than 0 or greater than 1)
      if ((flPrSet < 0) || (flPrSet > 1))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 92, 1, nEqmNum);  // Output error
	  return FALSE;
	}
      *pPrFlagSet = flPrSet;  // Store relative probability that flag is set
    }
	
  return TRUE;
}
	



/*************************************************************************
*
* METHOD NAME:	Validate_Dynamic_Values
*
* DESCRIPTION:	Validates a dynamic value (rapid equilibrium probabilties
*		during initialisation). 
*		If nNumValues = 2, the values passed in are:
*
*		0 - Pr (flag is clear)
*		1 - Pr (flag is set)
*
*		If nNumValues = 1, the values passed in are:
*
*		0 - Relative Pr (flag is set)
*
* PARAMETERS:	int	nNumValues  - Number of data items in array
*				      (Can be 1 or 2 for this dyn object)
*		double* p_flPr	    - Pointer to probabilities to validate.
*
* RETURNS:	Bool	TRUE	    - Probability passed validation
*			FALSE       - Probability failed validation
*
*************************************************************************/

Bool
C_Complex_Type_Multistate::Validate_Dynamic_Values(int nNumValues,
							double* p_flPr)
{
  double	flPrSet;
  double	flPrClear;

  if (nNumValues > 1)  // If two probabilities were specified
    {
      flPrClear = p_flPr[0];
      flPrSet = p_flPr[1];
      // If probabilities are invalid
      //  (ie if either are negative or sum to zero),return with error
      if ((flPrClear + flPrSet == 0) || (flPrSet < 0) || (flPrClear < 0))
	return FALSE;
    }
  else
    {
      // If probability is invalid (less than 0 or greater than 1),
      // return with error
      if ((p_flPr[0] < 0) || (p_flPr[0] > 1))
	return FALSE;
    }
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Create
*
* DESCRIPTION:	Creates a new complex of this type during simulation. A
*		new object is created and substituted in the simulation
*		for a free complex (taken from the stack).
*
* PARAMETERS:	C_Reaction* P_Reaction 	- Pointer to reaction in reaction
*					  matrix which is occurring.
*
* RETURNS:	Bool	TRUE	- Complex was created successfully.
*			FALSE	- An error occurred while creating
*				  complex.
*
*************************************************************************/

Bool
C_Complex_Type_Multistate::Create(C_Reaction* P_Reaction)
{
  C_Complex* P_Complex;
  C_Complex_Multistate* P_Complex_Multistate;
  P_Complex = P_Parent->P_Free_Complex->Pop();  // Pull free complex off stack
  if (P_Complex == NULL)
    return FALSE;
  // Create new multistate complex with the same ID
  P_Complex_Multistate
    = (C_Complex_Multistate*) Create_New_Complex(P_Complex->Get_System_ID());
  // If multistate complex was not created successfully
  if (P_Complex_Multistate == NULL)
    return FALSE;                  // Return with error

// Increment the level of this complex type if this is a build with
// output averaging enabled (NOTE: This must be done BEFORE calling
// Update()!)
#ifdef _DYNAMIC_UPDATE
  Inc_Level(P_Complex_Multistate);  // 
#endif

  // Update the state of this multistate CT
  P_Complex_Multistate->Update(P_Reaction);
  
  // Replace complex in system with multistate complex
  P_Parent->Replace_Complex(P_Complex->Get_System_ID(), P_Complex_Multistate);
  delete P_Complex;  // Delete complex 

#ifdef _MSDEBUG  // Output message if we're debugging in Microsoft environment
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\t\tC_Complex_Type_Multistate::Create: Creating '%s' with ID %ld\n",
		Display_String(), P_Complex_Multistate->Get_System_ID());
#endif

  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Destroy
*
* DESCRIPTION:	Destroys a complex of this type during simulation. The
*		object is destroyed and a replacement C_Complex pushed
*		onto the stack as an empty placeholder object.
*
* PARAMETERS:	C_Complex* P_Complex_Multistate  - Pointer to the multistate
*						   complex to be destroyed.
*
* RETURNS:	Bool	TRUE	  - Complex was destroyed successfully.
*			FALSE	  - An error occurred while destroying
*				    complex.
*
*************************************************************************/

Bool
C_Complex_Type_Multistate::Destroy(C_Complex* P_Complex_Multistate)
{
  C_Complex* P_Complex;

#ifdef _MSDEBUG  // Output message if we're debugging in Microsoft environment
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\t\tC_Complex_Type_Multistate::Destroy: Destroying '%s' with ID %ld\n",
		Display_String(), P_Complex_Multistate->Get_System_ID());
#endif

  // Attempt to create new C_Complex object
  P_Complex = new C_Complex(m_pApp, P_Parent, P_Complex_Multistate->Get_System_ID());
  if (P_Complex == NULL)  // If creation was unsuccessful...
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8, "new Complex");  // Output error
      return FALSE;
    }
  // Indicate this complex object does not yet exist
  P_Complex->Set_Exists(FALSE);
  // Push this complex onto free complex stack
  P_Parent->P_Free_Complex->Push(P_Complex);
  // Replace complex in system with multistate complex
  P_Parent->Replace_Complex(P_Complex_Multistate->Get_System_ID(), P_Complex);
  delete P_Complex_Multistate;	// Delete the multistate complex object
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Exec_Event
*
* DESCRIPTION:	Called by the event manager to execute an event which will
*		change the probability of a rapid equilibrium.
*
* PARAMETERS:	double* p_flPr	    - Pointer to new probability
*		long* p_nParameter  - Pointer to parameter array containing
*				      information indicating which states
*				      are affected (ie Wildcard mask and
*				      value)
*
*************************************************************************/

void
C_Complex_Type_Multistate::Exec_Event(double* p_flPr, long* p_nParameter)
{
  long nState;
  long nWildcardMask;
  long nWildcardValue;
  long nFlag;
  long nMaxState;

  // Extract parameters from array
  nWildcardMask = p_nParameter[MS_EVENT_PARAM_WILDCARD_MASK];
  nWildcardValue = p_nParameter[MS_EVENT_PARAM_WILDCARD_VALUE];
  nFlag = p_nParameter[MS_EVENT_PARAM_FLAG];
  // Calculate the maximum state this multistate CT can be in
  nMaxState = m_nNumStates - 1;
  // Loop through all the possible states for this multistate complex
  for (nState = 0; nState <= nMaxState; nState ++)
    // If this wildcard string specifies this state...
    if ((nState & nWildcardMask) == nWildcardValue)
      // Update this state's probability of rapid equilibrium
      m_pPrFlagSet[nFlag][nState] = p_flPr[0];
}




/*************************************************************************
*
* METHOD NAME:	Calc_Effective_Total_Pr
*
* DESCRIPTION:	When the reaction matrix is being optimised, this is called
*		to return the maximum total probability that any state of
*		this complex type will react. For each state, the total
*		probability is calculated based on the relative rate of 
*		reaction for this state and reaction probability. The 
*		largest sum is returned.
*
*		For example: if MS CT has 2 flags, and there are 3 reactions
*		it can take part in (in this set) which occur with
*		probabilities 0.1, 0.2, 0.1.
*		If relative probabilities for reaction are:
*		State	Reaction 1	Reaction 2	Reaction 3
*		00		0		0		0.1
*		01		0		0.5		1.0
*		10		1.0		0.5		1.0
*		11		0		1.0		0
*		Totals are calculated by multiplying relative probabilities
*		by probability of reaction:
*		State	Reaction 1	Reaction 2	Reaction 3	Sum
*		00		0		0		0.01	0.01
*		01		0		0.10		0.20	0.30
*		10		0.10		0.10		0.10	0.30
*		11		0		0.20		0	0.20
*		The largest sum (0.3) is returned for optimisation. Note that
*		this is less than simply adding the probabilities for reaction
*		which is 0.40.
*
* PARAMETERS:	C_Reaction** P_P_Reaction - Pointer to array of reaction
*					    pointers which is being processed.
*		int nNumReactions	  - Number of reactions in array
*
*************************************************************************/

double
C_Complex_Type_Multistate::Calc_Effective_Total_Pr(C_Reaction** P_P_Reactions,
						   int nNumReactions)
{
  int		nReaction;
  int		nState;
  double	flSumPr;
  double	flMaxSumPr;
  long	nMaxState;
	
  flMaxSumPr = 0;
  nMaxState = m_nNumStates - 1;  // Calculate the maximum state
  for (nState = 0; nState <= nMaxState; nState ++)  // Loop through each state
    {
      flSumPr = 0;
      // Calculate the sum of probabilities of all reactions
      for (nReaction = 0; nReaction < nNumReactions; nReaction ++)
	flSumPr += (Get_Relative_Probability(P_P_Reactions[nReaction], nState)
		    * P_P_Reactions[nReaction]->Get_Probability()); 
      // If the total sum for this state is greater than the running total
      if (flSumPr > flMaxSumPr)
	flMaxSumPr = flSumPr;  // Update maximum probability
    }
  return flMaxSumPr;  // Return indicating no error
}

/*************************************************************************
*
* METHOD NAME:	Get_Reaction_From_ID
*
* DESCRIPTION:	Returns a pointer to the Multistate Reaction identified
*               by its ID.
*
* PARAMETERS:   long nReactionID  - ID of the reaction to find.
*
* RETURNS:      C_Complex_Type_Multistate_Reaction* 
*                         - pointer to the specified Multistate Reaction. 
*
*************************************************************************/

C_Complex_Type_Multistate_Reaction*
C_Complex_Type_Multistate::Get_Reaction_From_ID(long nReactionID)
{
  long nCount;
  
  for (nCount = 0; nCount < m_nNumReactions; nCount ++)
    if(P_Reactions[nCount]->Get_Reaction_ID() == nReactionID)
      return P_Reactions[nCount];

  return NULL;  // Return NULL if reaction was not found
}

