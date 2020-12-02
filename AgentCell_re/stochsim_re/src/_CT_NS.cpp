/*************************************************************************
*
* FILENAME:	CT_NS.cpp
*
* CLASS:	C_Complex_Type_Neighbour_Sensitive
*
* DESCRIPTION:	This class is used to store a special complex type,
*		representing a neighbour sensitive complex type. This 
*		class is a descendant of C_Complex_Type_Multistate.
*
* TYPE:		Core
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
*               C_Complex_Type*	P_Old_Complex_Type  - Complex type this special
*						      type is based upon
*		C_System*	P_System            - Pointer to the system 
*						      containing the complex
*						      type (ie the parent
*                                                     system).
*               long            nNumNeighbours      - The number of neighbours
*                                                     that this complex type
*                                                     has in the array. (This
*                                                     must match the geometry
*                                                     of the array.)
*
*************************************************************************/

C_Complex_Type_Neighbour_Sensitive::C_Complex_Type_Neighbour_Sensitive
(C_Application* p_App, C_Complex_Type* P_Old_Complex_Type, C_System* P_System, long nNumNeighbours)
  : C_Complex_Type_Multistate (p_App, P_Old_Complex_Type, P_System)
{
  int nCount;
  int nCountState;

  m_nNumNeighbours = nNumNeighbours;
  for (nCount = 0; nCount < MAX_MS_NUM_FLAGS; nCount ++)
    {
      m_bIsNSRpdEqm[nCount] = FALSE;
      m_pNSPrFlagSet[nCount] = NULL;
      m_nRENumCoupledStates[nCount] = 0;

      for(nCountState = 0; nCountState < MAX_NS_COUPLED_STATES; nCountState++)
	{
	  m_nRECoupledStateWCMask[nCount][nCountState] = -1;
	  m_nRECoupledStateWCValue[nCount][nCountState] = -1;
	}
    }
		
  for (nCount = 0; nCount < MAX_MS_NUM_REACTIONS; nCount ++)
    {
      P_Reactions[nCount] = NULL;
      m_bIsNSReaction[nCount] = FALSE;
    }
}

	
	
	
/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the neighbour-sensitive (NS) complex type. The
*               state flags are retrieved from the INI file. Then the initial
*               states for all the complexes of this type are set. Then
*               reactions are constructed, representing reactions in which this
*		complex type is involved.
*
* PARAMETERS:	C_Complex*	P_Complex     - Pointer to array of
*                                               complexes in the system.
*		long		nNumComplexes -	Number of complexes in the
*						system.
*
* RETURNS:	Bool	TRUE	- The special complex type was initialised 
*				  successfully
*			FALSE	- An error occurred initialising the 
*				  special complex type
*
*************************************************************************/

Bool
C_Complex_Type_Neighbour_Sensitive::Init(C_Complex** P_Complex,
					 long nNumComplexes)
{

  char	lpszParameter		[MAX_INI_PARAMETER_LENGTH];
  char	lpszBitString		[MAX_INI_PARAMETER_LENGTH];
  char	lpszStateDisplayString	[MAX_INI_PARAMETER_LENGTH];
  char	lpszRelProb		[MAX_INI_PARAMETER_LENGTH];
  char	lpszNSFileSection	[MAX_INI_SECTION_LENGTH];
  char	lpszSectionName		[MAX_INI_SECTION_LENGTH];
  char	lpszRpdEqmFlag		[MAX_INI_VALUE_LENGTH];
  char	lpszWildcardsPara	[MAX_INI_VALUE_LENGTH_LONG];
  char	lpszError		[MAX_MESSAGE_LENGTH];
  char  lpszParaList		[MAX_MESSAGE_LENGTH];
  char	lpszArrayFile		[MAX_FILE_NAME_LENGTH];
  char	lpszComplexFile		[MAX_FILE_NAME_LENGTH];
  // Max number of wildcards
  //   = max length of wildcard list / (minimum number of flags + 1)
  char	lpszWildcards[MAX_INI_VALUE_LENGTH_LONG / 2][MAX_MS_NUM_FLAGS];
  char	lpszCoupledStates	[MAX_INI_VALUE_LENGTH];
  char  lpszReactions           [MAX_INI_VALUE_LENGTH];
  char  lpszNSReactionList      [MAX_MS_NUM_REACTIONS][MAX_INI_VALUE_LENGTH];
  char  lpszReactionDir         [MAX_INI_VALUE_LENGTH];
  char*	lpszPos;
  long	nNumRpdEqm;
  long	nCountEqm;
  long	nCountFlag;
  long	nCountNeighbour;
  long	nMaxState;
  long	nState;
  long	nWildcardMask;
  long	nWildcardValue;
  long	nNumWildcardStrings;
  long	nWildcard;
  long	nDefaultPr;
  long	nCountReaction;
  long	nCountNSReaction;
  long	nFields;
  long	nReactionID;
  Bool  bIsForward;
  double p_flPrFlagSet[NS_MAX_NUM_NEIGHBOURS + 1];
  C_Complex_Type_Neighbour_Sensitive_Reaction* pTmpNSReaction;


  // First, initialise all multistate reactions
  if(!C_Complex_Type_Multistate::Init(P_Complex, nNumComplexes))  
    return FALSE;
	

  nMaxState = PowerOfTwo(m_nNumFlags) - 1;

  // Get name of file containing array information
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE, lpszArrayFile))
    return FALSE;

  // Get name of file containing complex information
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE,
		     lpszComplexFile))
    return FALSE;

  // Get name of file containing neighbour-sensitive complex type information

  strcpy(lpszParameter, Display_String());
  strcat(lpszParameter, INI_NS_TYPES_FILE);
  if (!m_pApp->Get_INI(INI_NS_TYPES_SECTION, lpszParameter, m_lpszINIFile,
		     lpszComplexFile, TRUE))
    {
      // For backwards compatibility
      if (!m_pApp->Get_INI(INI_NS_TYPES_SECTION, lpszParameter, m_lpszINIFile,
		     lpszArrayFile))
	return FALSE;
    }


  // Initialise neighbour-sensitive reactions

  // Get the reactions that this neighbour-sensitive complex is involved in
  if (!m_pApp->Get_INI(INI_NS_GENERAL_SECTION, INI_REACTION_IDS, lpszReactions,
		     m_lpszINIFile))
    return FALSE;

  // Extract reaction IDs from reaction list into an array

  m_nNumNSReactions = 0;
  // Find first reaction ID in list
  lpszPos = strtok(lpszReactions, INI_MS_REACTION_SEPARATOR);
  // While the end of the reaction list is not reached
  // and the maximum number of reactions is not exceeded...
  while ((lpszPos != NULL) && (m_nNumNSReactions < MAX_MS_NUM_REACTIONS))
    {
      // Add reaction ID to array
      strcpy(lpszNSReactionList[m_nNumNSReactions], lpszPos);
      m_nNumNSReactions ++;
      lpszPos = strtok(NULL, INI_MS_REACTION_SEPARATOR);  // Find next reaction
    }
  if (lpszPos != NULL)  // If the end of the list wasn't reached, output error
    {
      m_pApp->Message(MSG_TYPE_ERROR, 85);
      return FALSE;
    }

  // Loop through reaction array and construct appropriate
  // C_Complex_Type_Neighbour-Sensitive_Reaction
  for (nCountNSReaction = 0; nCountNSReaction < m_nNumNSReactions;
       nCountNSReaction ++)
    {
      // Extract the reaction ID and direction from the ID list
      nFields = sscanf(lpszNSReactionList[nCountNSReaction], "%ld%s",
		       &nReactionID, lpszReactionDir);
      // If an incorrect number of fields were extracted,
      // or the direction code is not recognised...
      if ((nFields != 2)
	  || ((strcmp(lpszReactionDir, INI_FORWARDS_REACTION) != 0)
	      && (strcmp(lpszReactionDir, INI_REVERSE_REACTION) != 0)))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 86,
			lpszNSReactionList[nCountNSReaction]);  // Output error
	  return FALSE;
	}
      
      // Get Multistate Reaction of this ID
 
      // If it is a forwards reaction
      if (strcmp(lpszReactionDir, INI_FORWARDS_REACTION) == 0)
	bIsForward = TRUE;
      else
	bIsForward = FALSE;

      nCountReaction = 0;
      // Loop through reactions to find the specified one
      while (P_Reactions[nCountReaction]->Get_Reaction_ID()
	     != (bIsForward
		 ? nReactionID : nReactionID + REVERSE_REACTION_ID_OFFSET))
	{
	  // If the reaction was not found...
	  if(nCountReaction >= m_nNumReactions)
	    {
	      // ...format error message, and...
	      sprintf(lpszError, "%ld%s%s%s%s", nReactionID,
		      PARAMETER_LIST_SEPARATOR, lpszReactionDir,
		      PARAMETER_LIST_SEPARATOR, m_lpszINIFile);
	      // ...output error
	      m_pApp->Message(MSG_TYPE_ERROR, 147, lpszError);
	      return FALSE;
	    }
	  nCountReaction ++;
	}

      // Set boolean flag indicating that this is an NS reaction
      m_bIsNSReaction[nCountReaction] = TRUE;
 
      // Replace initialised multistate reaction object
      // with a neighbour sensitive one.
      
      // Create new CT_NS_R,...
      pTmpNSReaction
	= new C_Complex_Type_Neighbour_Sensitive_Reaction(m_pApp,
							  P_Reactions[nCountReaction]);
      // Free the memory used by the CT_MS_R
      delete P_Reactions[nCountReaction];
      // Replace with the new CT_NS_R
      P_Reactions[nCountReaction] = pTmpNSReaction; 


      // Create the section name containing details of the reaction
      strcpy(lpszNSFileSection, INI_REACTION_SECTION);
      strcat(lpszNSFileSection, lpszNSReactionList[nCountNSReaction]);
      
      // Initialise probabilities in reaction.
      // If initialisation fails, output error
      if (!(((C_Complex_Type_Neighbour_Sensitive_Reaction*)
	     P_Reactions[nCountReaction]) 
	    -> Init(m_lpszINIFile, lpszNSFileSection, m_nNumFlags, m_lpszFlag,
		    this, P_Parent->Get_Event_Manager(), m_nNumNeighbours)))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 173, 1, nReactionID);
	  return FALSE;
	}
    }


  // Initialise neighbour-sensitive rapid equilibria
  
  // Get the number of neighbour-sensitive rapid equilibria
  if (!m_pApp->Get_INI(INI_NS_GENERAL_SECTION, INI_NS_NUM_RAPID_EQM,
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
      sprintf(lpszSectionName, "%s%ld",
	      INI_NS_RAPID_EQM_SECTION, nCountEqm + 1);
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
	      sprintf(lpszError, "%s%s%ld",
		      lpszRpdEqmFlag, PARAMETER_LIST_SEPARATOR, nCountEqm + 1);
	      m_pApp->Message(MSG_TYPE_ERROR, 91, lpszError);
	      return FALSE;
	    }
	}

      // Read in the list of coupled states 
      // for this neighbour-sensitive rapid equilibria
		
      if (!m_pApp->Get_INI(lpszSectionName, INI_NS_COUPLED_STATES,
			 lpszCoupledStates, m_lpszINIFile))  
	return FALSE;
		
      // Extract wildcard mask for coupled states from list
      lpszPos = strtok(lpszCoupledStates, INI_NS_COUPLED_STATES_SEPARATOR);
      m_nRENumCoupledStates[nCountFlag] = 0;
      while(lpszPos != NULL)
	{
	  // If bitstring does not contain wildcard characters...
	  if(strchr(lpszPos, INI_WILDCARD_CHAR) == NULL)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 139, lpszPos);  // Output error

	      return FALSE;
	    }
	  
	  Extract_Wildcard_Mask(lpszPos, 
				&(m_nRECoupledStateWCMask[nCountFlag][m_nRENumCoupledStates[nCountFlag]]),
				&(m_nRECoupledStateWCValue[nCountFlag][m_nRENumCoupledStates[nCountFlag]]));
	  m_nRENumCoupledStates[nCountFlag]++;
	  lpszPos = strtok(NULL, INI_NS_COUPLED_STATES_SEPARATOR);
	}
	

      // Ensure this state flag is not already controlled
      // by another rapid equilibrium

      // If the rapid eqm has already been defined
      if (m_bIsNSRpdEqm[nCountFlag])
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 93, 1, nCountEqm + 1); // Output error
	  return FALSE;
	}							
      // Indicate this flag is controlled by a rapid equilibrium
      m_bIsNSRpdEqm[nCountFlag] = TRUE;

      // Override rapid equilibrium for base class
      m_bIsRpdEqm[nCountFlag] = FALSE;

      // Initialise array to store probabilities

      // Attempt to create array to store probabilities for this flag
      m_pNSPrFlagSet[nCountFlag] = new double* [nMaxState + 1];	
      // If the array was not created successfully, output error
      if (m_pNSPrFlagSet[nCountFlag] == NULL)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 8,
			"array to store neighbour-sensitive rapid equilibria");
	  return FALSE;
	}

      for (nState = 0; nState <= nMaxState; nState ++)
	{
	  m_pNSPrFlagSet[nCountFlag][nState]
	    = new double [m_nNumNeighbours + 1];
	  // If the array was not created successfully, output error
	  if (m_pNSPrFlagSet[nCountFlag][nState] == NULL)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 8,
			    "array to store neighbour-sensitive rapid equilibria");
	      return FALSE;
	    }
	  // Initialise probability array to rogue value so it will be
	  // possible to detect which probabilities have been set
	  for (nCountNeighbour = 0; nCountNeighbour < m_nNumNeighbours + 1;
	       nCountNeighbour++)
	    m_pNSPrFlagSet[nCountFlag][nState][nCountNeighbour] = -1;
	}

      
      // Read in any wildcard bit strings from the INI file 
      // ie strings which represent multiple states. and store them in an array
	
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
	  // Extract the rapid equilibrium probabilities from
	  // probability string
	  if (!Extract_Rapid_Eqm_Probability(lpszRelProb, nCountEqm + 1,
					     p_flPrFlagSet))
	    return FALSE;
	  // Work out the mask and value from the wildcard string
	  Extract_Wildcard_Mask(lpszPos, &nWildcardMask, &nWildcardValue);
	  // Loop through all the possible states
	  // for this neighbour-sensitive complex
	  for (nState = 0; nState <= nMaxState; nState ++)
	    // If this state is defined by this wildcard string...
	    if ((nState & nWildcardMask) == nWildcardValue)
	      {
		
		for (nCountNeighbour = 0;
		     nCountNeighbour < m_nNumNeighbours + 1;
		     nCountNeighbour++)
		  {
		    // If probability for this state has already been set
		    if (m_pNSPrFlagSet[nCountFlag][nState][nCountNeighbour]
			!= -1)
		      {
			// Output error indicating this probability
			// has already been set
			Get_State_Display_String(nState,
						 lpszStateDisplayString);
			m_pApp->Message(MSG_TYPE_ERROR, 110,
				      lpszStateDisplayString);
			return FALSE;
		      }
		    // Store the probability that this flag will be set
		    // at any given time
		    m_pNSPrFlagSet[nCountFlag][nState][nCountNeighbour]
		      = p_flPrFlagSet[nCountNeighbour];
		  }
	      }
	}
			
      // For each state the neighbour-sensitive complex can be in,
      // create bit string and load in the probabilities for this set
      // of rapid equilbria.
      // A bit string will be created - a binary string equivalent of nState.
      // This is only done for the states which haven't been initialised yet;
      // some probabilities will have been set already using wildcards above.

      for (nState = 0; nState <= nMaxState; nState ++)
	// If probabilities for this state have not been set yet
	if (m_pNSPrFlagSet[nCountFlag][nState][0] == -1)
	  {
	    // Create a bit string equivalent of the state
	    LongToBitString(nState, lpszBitString, m_nNumFlags);
	    // Retrieve the relative probabilities from the INI file.
	    if (!m_pApp->Get_INI(lpszSectionName, lpszBitString, lpszRelProb,
			       m_lpszINIFile, TRUE))
	      {
		// If not found, assign the default value
		for (nCountNeighbour = 0;
		     nCountNeighbour < m_nNumNeighbours + 1;
		     nCountNeighbour++)
		  p_flPrFlagSet[nCountNeighbour] = MS_DEFAULT_RE_PR_SET;
		// Count number of times we used a default probability
		nDefaultPr ++;
	      }
	    else
	      {
		// If found, extract rapid equilibrium probabilities
		// from probability list
		if (!Extract_Rapid_Eqm_Probability(lpszRelProb, nCountEqm + 1,
						   p_flPrFlagSet))
		  return FALSE;
	      }
	    // For each neighbour state
	    for (nCountNeighbour = 0;
		 nCountNeighbour < m_nNumNeighbours + 1;
		 nCountNeighbour++)
	      // Copy probability from temporary array
	      m_pNSPrFlagSet[nCountFlag][nState][nCountNeighbour]
		= p_flPrFlagSet[nCountNeighbour];
	  }
    }

  if (nDefaultPr > 0)  // If default probabilities were assigned
    {
      // Create list of parameters for warning
      sprintf(lpszParaList, "%s%s%d%s%ld%s%s",
	      "neighbour-sensitive rapid equilibrium probability",
	      PARAMETER_LIST_SEPARATOR, MS_DEFAULT_RE_PR_SET,
	      PARAMETER_LIST_SEPARATOR, nDefaultPr, PARAMETER_LIST_SEPARATOR,
	      Display_String());
      // Output warning that default value is being used
      m_pApp->Message(MSG_TYPE_STATUS, 122, lpszParaList);
    }

  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Extract_Rapid_Eqm_Probability
*
* DESCRIPTION:	Extracts the relative probability that a state flag is set
*		according to a neighbour-sensitive rapid equilibrium.
*
* PARAMETERS:	char*	lpszRelProb	- Pointer to string containing relative
*					  probabilities
*		long	nEqmNum		- Number of equilibrium (used in error
*					  messages)
*		double*	p_flPrFlagSet   - Pointer to array of double that will
*					  store the relative probabilities
*
* RETURNS:	Bool	TRUE		- Extraction was successful
*			FALSE		- An error occurred during extraction
*			p_flPrFlagSet	- Pointer to populated relative 
*                                         probabilies
*
*************************************************************************/

Bool
C_Complex_Type_Neighbour_Sensitive::Extract_Rapid_Eqm_Probability (char* lpszRelProb, long nEqmNum, double* p_flPrFlagSet)
{
  char*	 lpszRelProbSeparator;
  char*	 lpszPos;
  double p_flPrSet[NS_MAX_NUM_NEIGHBOURS + 1];
  int	 nCount;

  lpszPos = lpszRelProb;
  // Find the separator dividing the probabilities
  lpszRelProbSeparator = strstr(lpszRelProb, INI_RPD_EQM_SEPARATOR);
  for (nCount = 0; nCount < m_nNumNeighbours + 1; nCount++)
    {

      if (lpszRelProbSeparator != NULL)
	// Null-terminate the first probability
	*lpszRelProbSeparator = NULL_CHAR;
      else
	// If there are not enough probabilities to account for all neighbours
	if (nCount < m_nNumNeighbours)
	  {
	    m_pApp->Message(MSG_TYPE_ERROR, 137, 1, nEqmNum);  // Output error
	    return FALSE;
	  }

      // Convert the probabilities to doubles	
      p_flPrSet[nCount] = atof(lpszPos);
      if (lpszRelProbSeparator != NULL)
	lpszPos = lpszRelProbSeparator + strlen(INI_RPD_EQM_SEPARATOR);
      // Find the separator dividing the two probabilities
      lpszRelProbSeparator = strstr(lpszPos, INI_RPD_EQM_SEPARATOR);
    }
  
  for (nCount = 0; nCount < m_nNumNeighbours + 1; nCount++)
    {
      // If probability is invalid 
      if (p_flPrSet[nCount] < 0 || p_flPrSet[nCount] > 1)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 136, 1, nEqmNum); // Output error
	  return FALSE;
	}
      p_flPrFlagSet[nCount] = p_flPrSet[nCount];
    }
  
  return TRUE;
}
	



/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Deletes the reactions
*
*************************************************************************/

C_Complex_Type_Neighbour_Sensitive::~C_Complex_Type_Neighbour_Sensitive(void)
{
  int nCountFlag;
  int nCount;

  // Free dynamically allocated memory

  for (nCountFlag = 0; nCountFlag < MAX_MS_NUM_FLAGS; nCountFlag++)
    if (m_pNSPrFlagSet[nCountFlag] != NULL)
      {
	for (nCount = 0; nCount < PowerOfTwo(m_nNumFlags) - 1; nCount++)
	  if (m_pNSPrFlagSet[nCountFlag][nCount] != NULL)
	    delete[] m_pNSPrFlagSet[nCountFlag][nCount];
	delete[] m_pNSPrFlagSet[nCountFlag];
      }
}

/*************************************************************************
*
* METHOD NAME:	Is_Coupled_State
*
* DESCRIPTION:	Checks whether the specified state is a coupled state for
*		the rapid equilibrium of the specified flag.
*
* PARAMETERS:  	long nFlag  - Rapid equilibrium flag to be checked 
*
*		long nState - State of the neighbour 
*
* RETURNS:	Bool TRUE   - If the state is a coupled state for this flag
*		     FALSE  - If the state is not a coupled state for this flag
*
*************************************************************************/

Bool
C_Complex_Type_Neighbour_Sensitive::Is_Coupled_State(long nFlag, long nState)
{
  int nCount;

  // Loop through all the coupled states
  for (nCount = 0; nCount < m_nRENumCoupledStates[nFlag]; nCount++)
    if ((nState & m_nRECoupledStateWCMask[nFlag][nCount])
	== m_nRECoupledStateWCValue[nFlag][nCount])
      return TRUE;

  return FALSE;
}

	


/*************************************************************************
*
* METHOD NAME:	Create_New_Complex
*
* DESCRIPTION:	Creates a new complex object for this complex type and
*		returns a pointer to the newly created object.
*
* PARAMETERS:  	long nSystemID - ID this complex has in the system.
*
* RETURNS:	C_Complex*     - Pointer to new complex of this type.
*
*************************************************************************/

C_Complex*
C_Complex_Type_Neighbour_Sensitive::Create_New_Complex(long nSystemID)
{
  C_Complex* P_Complex;
  
  // Attempt to create new C_Complex_Neighbour_Sensitive object
  P_Complex = new C_Complex_Neighbour_Sensitive(m_pApp, P_Parent, nSystemID,
						m_nNumNeighbours);
  
  // If creation was successful...
  if (P_Complex != NULL)
    {
      // Set complex object to be of this complex type
      P_Complex->Set_Type(this);
      
      // Indicate this complex object exists
      P_Complex->Set_Exists(TRUE);
    }
  else  // Otherwise output error
    m_pApp->Message(MSG_TYPE_ERROR, 8, "new Neighbour_Sensitive Complex");		
  return P_Complex;  // Return a pointer to this complex object
}


/*************************************************************************
*
* METHOD NAME:	Get_Relative_Probability
*
* DESCRIPTION:	This version of this overloaded method is used at
*               initialisation time, when optimising the reaction matrix.
*
*
* PARAMETERS:	C_Reaction* P_Reaction       - Reaction ID 
*
*               long        nMyState         - The state of this complex.
*                                              
*               long        nNeighbourState  - The state of the neighbour.
*
* RETURNS:      double  - Relative probability for this neighbour-sensitive
*                         complex (for reaction nReaction, in state 
*                         m_nState, and with neighbours in P_Neighbour).
*                 
*************************************************************************/

double
C_Complex_Type_Neighbour_Sensitive::Get_Relative_Probability(C_Reaction* P_Reaction, long nMyState, long nNeighbourState)
{
  long                                         nCount;
  // Use CT_MS_R type pointer to access MS-specific methods
  C_Complex_Type_Multistate_Reaction*          p_CT_MS_R;
  // Use CT_NS_R type pointer to access NS-specific methods
  C_Complex_Type_Neighbour_Sensitive_Reaction* p_CT_NS_R;

  nCount = 0;
  // Loop through all the reactions until the correct ID is found
  while (P_Reactions[nCount]->Get_Reaction_ID() != P_Reaction->Get_ID())
    {
      nCount ++;
      // If the last reaction is passed, return 0 indicating
      // no reaction was found
      if (nCount >= m_nNumReactions)
	return 0;
    }

  // Explicitly typecast Reaction object to MS and NS pointers
  // (this makes the code a bit more readable by avoiding casts everywhere)
  p_CT_MS_R = (C_Complex_Type_Multistate_Reaction*) P_Reactions[nCount];
  p_CT_NS_R = ((C_Complex_Type_Neighbour_Sensitive_Reaction*)
	       P_Reactions[nCount]);
      
  // If this is a neighbour-sensitive reaction...
  if (m_bIsNSReaction[nCount])
    {
      // ...return the relative reaction probability,
      //    which is (MS relative probability) * (NS relative probability)

      return (p_CT_MS_R -> Get_Relative_Probability(nMyState)
	      * p_CT_NS_R -> Get_Relative_Probability(nNeighbourState));
    }
  else
    // Otherwise, return the MS relative probability
    return p_CT_MS_R -> Get_Relative_Probability(nMyState);
}

/*************************************************************************
*
* METHOD NAME:	Get_Relative_Probability
*
* DESCRIPTION:	This method calculates and returns the relative 
*               probability of a neighbour-sensitive reaction.
*               The relative probability of the base class is calculated
*               first, then multiplied with the relative probability specific
*               to the neighbour-sensitive reaction.
*
* PARAMETERS:	C_Reaction* P_Reaction  - Reaction ID 
*
*               long        nMyState    - The state of the NS complex from
*                                         which this method was called.
*
*               C_Complex_Neighbour_Sensitive**  P_Neighbours
*			                - List of pointers to neighbours
*
* RETURNS:      double  - Relative probability for this neighbour-sensitive
*                         complex (for reaction nReaction, in state 
*                         m_nState, and with neighbours in P_Neighbour).
*                 
*************************************************************************/

double
C_Complex_Type_Neighbour_Sensitive::Get_Relative_Probability(C_Reaction* p_Reaction, long nMyState, C_Complex_Neighbour_Sensitive** P_Neighbours)
{
  long nCount;
  long nNeighbourState;
  C_Complex_Neighbour_Sensitive* pNeighbour;
  C_Complex_Type_Multistate_Reaction*          p_CT_MS_R;
  C_Complex_Type_Neighbour_Sensitive_Reaction* p_CT_NS_R;

  // Loop through all the reaction until the correct ID is found
  nCount = 0;
  while (P_Reactions[nCount]->Get_Reaction_ID() != p_Reaction->Get_ID())
    {
      nCount ++;
      // If the last reaction is passed,
      // return 0 indicating no reaction was found
      if (nCount >= m_nNumReactions)
	return 0;
    }

  // Explicitly typecast Reaction object to MS and NS pointers
  // (this makes the code a bit more readable by avoiding casts everywhere)
  p_CT_MS_R = (C_Complex_Type_Multistate_Reaction*) P_Reactions[nCount];
  p_CT_NS_R = ((C_Complex_Type_Neighbour_Sensitive_Reaction*)
	       P_Reactions[nCount]);

  // If this is a neighbour-sensitive reaction...
  if (m_bIsNSReaction[nCount])
    {
      // Get the neighbour to which this NS-reaction is sensitive
      pNeighbour = P_Neighbours[p_CT_NS_R -> Get_Neighbour_ID()];

      // If the reacting neighbour does not exist
      // (i.e. if we're at the edge of the complex array)
      if (pNeighbour == NULL)
	{
	  // Return zero, indicating this reaction cannot occur
	  return 0;
	}
      nNeighbourState = pNeighbour -> Get_State();
    
      // ...return the relative reaction probability,
      // which is (MS relative probability) * (NS relative probability)

      return (p_CT_MS_R -> Get_Relative_Probability(nMyState)
	      * p_CT_NS_R -> Get_Relative_Probability(nNeighbourState));
    }
  else
    // Otherwise, return the MS relative probability
    return p_CT_MS_R -> Get_Relative_Probability(nMyState);
}


/*************************************************************************
*
* METHOD NAME:	Get_Flag_Modifiers
*
* DESCRIPTION:	Retrieves the bit strings indicating which flags in the
*		neighbour's state need clearing and setting for a particular
*               reaction.
*
* PARAMETERS:	C_Reaction*  P_Reaction      - Pointer to reaction in matrix
*		long*	     p_nFlagsToClear - Bit string for flags to clear
*		long*	     p_nFlagsToSet   - Bit string for flags to set
*
*************************************************************************/

void
C_Complex_Type_Neighbour_Sensitive::Get_Flag_Modifiers(C_Reaction* P_Reaction,
						       long* p_nFlagsToClear,
						       long* p_nFlagsToSet)
{
  long nCount;
  nCount = 0;
  // Loop through all the reaction until the correct ID is found
  while (P_Reactions[nCount]->Get_Reaction_ID() != P_Reaction->Get_ID())
    {
      nCount ++;
      // If the last reaction is passed, return
      if (nCount >= m_nNumReactions)
	return;
    }
  ((C_Complex_Type_Neighbour_Sensitive_Reaction*) P_Reactions[nCount])
    -> Get_Flag_Modifiers(p_nFlagsToClear, p_nFlagsToSet);
}



/*************************************************************************
*
* METHOD NAME:	Get_Is_NS_Reaction
*
* DESCRIPTION:	Returns TRUE if the specified reaction is a
*               neighbour-sensitive reaction.
*
* PARAMETERS:   C_Reaction* P_Reaction  - Pointer to the reaction which is
*                                         to be checked.
*
* RETURNS:      TRUE        - if the reaction is neighbour-sensitive
*               FALSE       - if the reaction is not neighbour-sensitive
*
*************************************************************************/

Bool
C_Complex_Type_Neighbour_Sensitive::Get_Is_NS_Reaction(C_Reaction* P_Reaction)
{
  long nCount;
  
  for (nCount = 0; nCount < m_nNumReactions; nCount ++)
    {
      if (P_Reactions[nCount]->Get_Reaction_ID() == P_Reaction->Get_ID())
	{
	  if (m_bIsNSReaction[nCount])
	    return TRUE;
	  else 
	    return FALSE;
	}
    }
  return FALSE;  // Return FALSE if reaction was not found
}

/*************************************************************************
*
* METHOD NAME:	Calc_Effective_Total_Pr
*
* DESCRIPTION:	When the reaction matrix is being optimised, this is called
*		to return the maximum total probability of reaction for
*               any combination of states between this complex and a
*               neighbouring complex. See also the comments about
*               C_Complex_Type_Multistate::Calc_Effective_Total_Pr()
*               in file _CT_MS.cpp.
*
* NOTE:         This method assumes that the neighbouring complex will
*               always be the same type as this one.
*
* PARAMETERS:	C_Reaction** P_P_Reaction - Pointer to set of reaction
*					    pointers which is being processed
*		int nNumReactions	  - Number of reactions in set
*
* RETURNS:      double                    - The maximum total probability
*                                           of reaction
*
*************************************************************************/

double
C_Complex_Type_Neighbour_Sensitive::Calc_Effective_Total_Pr(C_Reaction** P_P_Reactions, int nNumReactions)
{
  int		nReaction;
  int		nState;
  int		nNeighbourState;
  double	flSumPr;
  double	flMaxSumPr;
  long          nMaxState;

  nMaxState = PowerOfTwo(m_nNumFlags) - 1;  // Calculate the maximum state

  flMaxSumPr = 0;
  // Loop through each state of this complex
  for (nState = 0; nState <= nMaxState; nState ++)
    {
      // Calculate the sum of probabilities of all reactions

      // Loop through each state of neighbour
      for (nNeighbourState = 0; nNeighbourState <= nMaxState;
	   nNeighbourState ++)
	{
	  flSumPr = 0;
	  // Loop through all the reactions that can occur
	  for (nReaction = 0; nReaction < nNumReactions; nReaction ++)
	    // If this is an NS reaction...
	    if (Get_Is_NS_Reaction(P_P_Reactions[nReaction]))
	      {
		flSumPr += 
		  (P_P_Reactions[nReaction]->Get_Probability()
		   * (((C_Complex_Type_Neighbour_Sensitive*) this)
		      -> Get_Relative_Probability(P_P_Reactions[nReaction],
						  nState, nNeighbourState)));
	      }
	    else  // Otherwise, this is an MS reaction, so...
	      // Get base probability and the MS relative probability, 
	      // and add the product to the sum of probabilities
	      flSumPr +=
		(P_P_Reactions[nReaction]->Get_Probability()
		 * (((C_Complex_Type_Multistate*) this)
		    -> Get_Relative_Probability(P_P_Reactions[nReaction],
						nState)));
	  // If the total sum for this combination of states is greater
	  // than the maximum probability so far, update maximum probability
	  if (flSumPr > flMaxSumPr)
	    flMaxSumPr = flSumPr;
	}
    }

  return flMaxSumPr;  // Return the maximum probability found
}


/*************************************************************************
*
* METHOD NAME:	Get_Reaction_From_ID
*
* DESCRIPTION:	Returns a pointer to the NS Reaction identified by its ID.
*
* PARAMETERS:   long nReactionID  - ID of the reaction to find.
*
* RETURNS:      C_Complex_Type_Neighbour_Sensitive_Reaction* 
*                         - pointer to the specified NS Reaction. 
*
*************************************************************************/

C_Complex_Type_Neighbour_Sensitive_Reaction*
C_Complex_Type_Neighbour_Sensitive::Get_NS_Reaction_From_ID(long nReactionID)
{
  long nCount;
  
  // Loop through the list of reactions for this NS complex type
  for (nCount = 0; nCount < m_nNumReactions; nCount ++)
    // If the ID of this reaction matches the specified one
    if(P_Reactions[nCount]->Get_Reaction_ID() == nReactionID)
      // return a CT_NS_R pointer to this reaction
      return
	(C_Complex_Type_Neighbour_Sensitive_Reaction*) P_Reactions[nCount];

  return NULL;  // Return NULL if reaction was not found
}

#ifdef _NS_R_DEBUG 
long
C_Complex_Type_Neighbour_Sensitive::Get_NS_R_Count(long nReaction)
{
    if (m_bIsNSReaction[nReaction]) 
      return ((C_Complex_Type_Neighbour_Sensitive_Reaction*)P_Reactions[nReaction])->Get_Count();
    else 
      return -1;
  }

void
C_Complex_Type_Neighbour_Sensitive::Inc_Count(C_Reaction* P_Reaction)
{
  if (Get_Is_NS_Reaction(P_Reaction))
    Get_NS_Reaction_From_ID(P_Reaction->Get_ID())->Inc_Count();
}
#endif /* _NS_R_DEBUG */
