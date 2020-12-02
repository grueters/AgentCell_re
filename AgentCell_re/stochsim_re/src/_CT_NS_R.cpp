/*************************************************************************
*
* FILENAME:	CT_NS_R.cpp
*
* CLASS:	C_Complex_Type_Neighbour_Sensitive_Reaction
*
* DESCRIPTION:	This class is used to store details on a reaction which is
*		affected by or affects a the state of a neighbouring complex 
*               in a C_Complex_Array.  It is a descendant class of the
*               C_Complex_Type_Multistate_Reaction class.  During
*               initialisation,  all MS-Reactions are created first, and 
*               any MS-Reaction object which should also be neighbour-sensitive
*               (according to the INI files) is replaced by an NS-Reaction
*               object which copies all attributes of the pre-initialised
*               MS-Reaction object during construction.
*        
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor 
*
* DESCRIPTION:	This constructor creates a new
*               C_Complex_Typde_Neighbour_Sensitive_Reaction object based
*               on a pre-initialised C_Complex_Type_Multistate_Reaction
*               object.  All the attributes of the base class are copied.
*
* PARAMETERS:   C_Application*  p_App - Pointer to parent application
*
*	        long	nReactionID - ReactionID for this object
*
*************************************************************************/

C_Complex_Type_Neighbour_Sensitive_Reaction::C_Complex_Type_Neighbour_Sensitive_Reaction
(C_Application* p_App, C_Complex_Type_Multistate_Reaction* pCT_MS_R)
  : C_Complex_Type_Multistate_Reaction (p_App, pCT_MS_R->Get_Reaction_ID())
{
  long nState;

  m_pNSPrArray = NULL;
  m_nNeighbourID = -1;
  m_nNeighbourFlagsToSet = 0;
  m_nNeighbourFlagsToClear = 0;
#ifdef _NS_R_DEBUG
  m_nCount = 0;
#endif /* _NS_R_DEBUG */

  m_nMaxState = pCT_MS_R->Get_Max_State();
  pCT_MS_R->Get_Flag_Modifiers(&m_nFlagsToClear, &m_nFlagsToSet);

  // Allocate memory for MS probabilities 
  m_pPrArray = new double[m_nMaxState];
  // Loop through all states
  for (nState = 0; nState < m_nMaxState; nState ++)
    {
      // Copy probabiities of MS reactions
      m_pPrArray[nState] = pCT_MS_R->Get_Relative_Probability(nState);
    }
#ifdef _NS_R_DEBUG
  m_nCount = 0;
#endif /* _NS_R_DEBUG */
}


/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the reaction probabilities.
*
*
* PARAMETERS:	char*	lpszINIFile	   - INI File containing details of
*                                            reaction
*		char*	lpszINISection     - Section in INI File containing
*					     details of reaction
*		int	nNumFlags	   - Number of state flags the
*                                            neighbour-sensitive complex
*                                            possesses.
*		char**	lpszFlag	   - Pointer to array of strings
*					     identifying flag strings.
*
*               C_Complex_Type_Neighbour_Sensitivve* P_Parent
*					   - Pointer to the parent of this
*                                            reaction.
*
*		C_Event_Manager* P_Event_Manager
*					   - Pointer to the system event
*                                            manager.
*
* RETURNS:	Bool	TRUE	- The reaction details were initialised 
*				  successfully
*			FALSE	- An error occurred initialising the 
*				  reaction details
*
*************************************************************************/

Bool
C_Complex_Type_Neighbour_Sensitive_Reaction::Init (char* lpszINIFile, char* lpszINISection, int nNumFlags, char lpszFlag[][MAX_MS_FLAG_LENGTH + 1], C_Complex_Type_Neighbour_Sensitive* P_Parent, C_Event_Manager* P_Event_Manager, long nNumNeighbours)
{
  char	lpszBitString[MAX_INI_PARAMETER_LENGTH];
  char	lpszEffectsString[MAX_INI_VALUE_LENGTH];
  char	lpszSetOrClear[MAX_INI_VALUE_LENGTH];
  char	lpszWildcardsPara[MAX_INI_VALUE_LENGTH_LONG];
  char	lpszStateDisplayString[MAX_INI_PARAMETER_LENGTH];
  char	lpszPr[MAX_INI_VALUE_LENGTH];
  char	lpszPrDynCode[MAX_INI_VALUE_LENGTH];
  char	lpszParaList[MAX_MESSAGE_LENGTH];
  // Max number of wildcards
  //   = max length of wildcard list / (minimum number of flags + 1)
  char	lpszWildcards[MAX_INI_VALUE_LENGTH_LONG / 2][MAX_MS_NUM_FLAGS];
  char*	lpszPos;
  char*	lpszPosDynCode;
  double flPr;
  long	nState;
  long	nBitEffect;
  // The mask is used to indicate which bits define
  // which states the wildcard string applies to
  long	nWildcardMask;
  // The wildcard value gives the value of the bits in the mask
  // for the states this wildcard string applies to 
  long	nWildcardValue;
  long	nNumWildcardStrings;
  long	nWildcard;
  long	nDefaultRate;
  long	nDynParameters[MAX_NUM_DYNAMIC_PARAMETERS];
  long  nNeighbourID;
  int	nCount;

  // Initialise probability array

  // Allocate memory for the probability array
  m_pNSPrArray = new double[m_nMaxState];
  // Initialise array members to -1
  // (so that it is possible to detect which members have been set later on)
  for (nState = 0; nState < m_nMaxState; nState ++)
    m_pNSPrArray[nState] = -1;

  // Retrieve the ID of the neighour to which this reaction is sensitive
  if (!m_pApp->Get_INI(lpszINISection, INI_NS_REACTION_NEIGHBOUR, &nNeighbourID,
		     lpszINIFile))
    return FALSE;
                       
  // Validate neibhbour ID value (e.g. should be between 0 and 3 for a
  // rectangular geometry array where NS-Complexes have 4 nearest neighbours)
  if (nNeighbourID < 0 || nNeighbourID >= nNumNeighbours)
    {
      // Create list of parameters for error message
      sprintf(lpszParaList, "%ld%s%ld", nNeighbourID, PARAMETER_LIST_SEPARATOR,
	      m_nID);
      m_pApp->Message(MSG_TYPE_ERROR, 146, lpszParaList);
      return FALSE;
    }
  m_nNeighbourID = nNeighbourID;

  // Read in any wildcard bit strings from the INI file
  // ie strings which represent multiple states, and store them in an array

  nNumWildcardStrings = 0;
  // Retrieve the list of wildcards from the INI file
  if (!m_pApp->Get_INI(lpszINISection, INI_WILDCARD_STRINGS, lpszWildcardsPara,
		     lpszINIFile))
    return FALSE;
  // Find first wildcard string in list
  lpszPos = strtok(lpszWildcardsPara, INI_WILDCARD_SEPARATOR);
  
  // While the end of the list is not reached...
  while (lpszPos != NULL)
    {
      // Store wildcard string
      strcpy(lpszWildcards[nNumWildcardStrings], lpszPos);
      nNumWildcardStrings ++;
      // Find next wildcard string
      lpszPos = strtok(NULL, INI_COMPLEX_SEPARATOR);
    }

  // Loop through each wildcard string and set up the probabilities
  // for the states they represent

  for (nWildcard = 0; nWildcard < nNumWildcardStrings; nWildcard ++)
    {
      // Redefine lpszPos for convenience
      lpszPos = lpszWildcards[nWildcard];
      lpszPrDynCode[0] = NULL_CHAR;
      // Ensure the length of this string matches the number of flags
      if ((signed) strlen(lpszPos) != nNumFlags)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 107, lpszPos);
	  return FALSE;
	}

      // Retrieve the relative probability from the INI file
      if (!m_pApp->Get_INI(lpszINISection, lpszPos, lpszPr, lpszINIFile))
	return FALSE;

      // Look for a dynamic object code in the probability
      lpszPosDynCode = strstr(lpszPr, INI_PR_DYN_CODE_SEPARATOR);
      // If a dynamic object code is specified with the probability
      if (lpszPosDynCode != NULL)
	{
	  // Extract the dynamic object code
	  strcpy(lpszPrDynCode, (lpszPosDynCode
				 + strlen(INI_RE_DYN_CODE_SEPARATOR)));
	  // Null terminate the probability
	  lpszPosDynCode[0] = NULL_CHAR;
	}
      // Convert the probability to a number
      flPr = atof(lpszPr);

      // If probability exceeds 1.0, output error
      // (the number should be a ratio so cannot exceed one)
      if (flPr > 1.00)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 90);
	  return FALSE;
	}

      // Work out the mask and value from the wildcard string
      P_Parent->Extract_Wildcard_Mask(lpszPos, &nWildcardMask,
				      &nWildcardValue);

      // Loop through all the possible states for this
      // neighbour-sensitive complex
      for (nState = 0; nState < m_nMaxState; nState ++)
	// If this state is defined by this wildcard string...
	if ((nState & nWildcardMask) == nWildcardValue)
	  {
	    // If this state proability has been set already
	    if (m_pNSPrArray[nState] != -1)
	      {
		// Output error
		P_Parent->Get_State_Display_String(nState,
						   lpszStateDisplayString);
		m_pApp->Message(MSG_TYPE_ERROR, 106, lpszStateDisplayString);
		return FALSE;
	      }
	    // Set this state probability
	    m_pNSPrArray[nState] = flPr;
	  }
      // If the probability can change over time
      // (ie reaction is a dynamic object)... 
      if (strlen(lpszPrDynCode) > 0)
	{
	  // ...attempt to create events

	  // Set up parameter array to pass to event manager
	  nDynParameters[MS_EVENT_PARAM_WILDCARD_MASK] = nWildcardMask;
	  nDynParameters[MS_EVENT_PARAM_WILDCARD_VALUE] = nWildcardValue;
	  // Register this object with the event manager as a dynamic object
	  if (!P_Event_Manager->Init_Dynamic_Object(this, lpszPrDynCode, 1, 1,
						    nDynParameters, 2))
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 115, lpszPos);
	      return FALSE;
	    }
	}
    }

  // For each state the neighbour-sensitive complex can be in, create bit 
  // string and load in the probability from the INI file.
  // A bit string will be created - a binary string equivalent of nState.
  // This is only done for the states which haven't been initialised yet; 
  // some probilities will have been set already using wildcards above.

  nDefaultRate = 0;
  for (nState = 0; nState < m_nMaxState; nState ++)
    // If state has not been set yet...
    if (m_pNSPrArray[nState] == -1)
      {
	lpszPrDynCode[0] = NULL_CHAR;
	// Create a bit string equivalent of the state
	LongToBitString(nState, lpszBitString, nNumFlags);
	// Retrieve the relative probability from the INI file. 
	if (!m_pApp->Get_INI(lpszINISection, lpszBitString, lpszPr, lpszINIFile,
			   TRUE))
	  {
	    // ...if not found, use default value
	    m_pNSPrArray[nState] = MS_DEFAULT_RATE;
	    // Count number of times the default rate was used
	    nDefaultRate ++;
	  }
	else  // Otherwise...
	  {
	    // Look for a dynamic object code in the probability
	    lpszPosDynCode = strstr(lpszPr, INI_PR_DYN_CODE_SEPARATOR);
	    // If a dynamic object code is specified with the probability
	    if (lpszPosDynCode != NULL)
	      {
		// Extract the dynamic object code
		strcpy(lpszPrDynCode, (lpszPosDynCode
				       + strlen(INI_PR_DYN_CODE_SEPARATOR)));
		// Null terminate the probability
		lpszPosDynCode[0] = NULL_CHAR;
	      }
	    // Convert the probability to a number
	    m_pNSPrArray[nState] = atof(lpszPr);
	  }
	// If probability exceeds 1.0, output error
	// (the number should be a ratio so cannot exceed one)
	if (m_pNSPrArray[nState] > 1.00)
	  {
	    m_pApp->Message(MSG_TYPE_ERROR, 90);
	    return FALSE;
	  }
	// If the probability can change over time
	// (ie reaction is a dynamic object)...
	if (strlen(lpszPrDynCode) > 0)
	  {
	    // ...attempt to create events

	    // Set up parameter array to pass to event manager	    
	    nDynParameters[MS_EVENT_PARAM_WILDCARD_VALUE] = nState;
	    // (Set mask to all 1s which is equivalent to 0xFFFFFFFF = -1)
	    nDynParameters[MS_EVENT_PARAM_WILDCARD_MASK] = -1;
	    // Register this object with the event manager as a dynamic object
	    if (!P_Event_Manager->Init_Dynamic_Object(this, lpszPrDynCode, 1,
						      1, nDynParameters, 2))
	      {
		m_pApp->Message(MSG_TYPE_ERROR, 115, 1, nState);
		return FALSE;
	      }
	  }
      }

  // If default rates were assigned
  if (nDefaultRate > 0)
    {
      // Create list of parameters for warning
      sprintf(lpszParaList, "%g%s%ld%s%s%s%s", (double) MS_DEFAULT_RATE,
	      PARAMETER_LIST_SEPARATOR, nDefaultRate,
	      PARAMETER_LIST_SEPARATOR, P_Parent->Display_String(),
	      PARAMETER_LIST_SEPARATOR, lpszINISection);
      // Output warning that default value is being used
      m_pApp->Message(MSG_TYPE_STATUS, 148, lpszParaList);
    }

  // Create masks which are used when the reaction occurs. The mask is used to
  // set or clear the appropriate state flags

  // Start off by assuming no flags need setting or clearing
  m_nNeighbourFlagsToClear = 0;
  m_nNeighbourFlagsToSet = 0;

  // Retrieve the list of effects from the INI file
  if (!m_pApp->Get_INI(lpszINISection, INI_NS_REACTION_EFFECTS,
		     lpszEffectsString, lpszINIFile))
    return FALSE;

  // Find first effect in list
  lpszPos = strtok(lpszEffectsString, INI_EFFECTS_SEPARATOR);
  // While the end of the list is not reached...
  while (lpszPos != NULL)
    {
      nCount = 0;
      // Try to find which flag this effect refers to
      while (strstr(lpszPos, lpszFlag[nCount]) == NULL)
	{
	  nCount ++;
	  // If the flag has not been found, output error
	  if (nCount >= nNumFlags)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 89, lpszPos);
	      return FALSE;
	    }
	}
      // Calculate the number that needs to be added to the mask
      nBitEffect = PowerOfTwo(nNumFlags - nCount - 1);

      // Extract the string describing if this is a set or clear state effect
      strcpy(lpszSetOrClear, lpszPos);
      lpszSetOrClear[strlen(lpszPos) - strlen(lpszFlag[nCount])] = NULL_CHAR;
      
      // If the effect description is not recognised, output error
      if ((strcmp(lpszSetOrClear, INI_EFFECT_SET) != 0)
	  && (strcmp(lpszSetOrClear, INI_EFFECT_CLEAR) != 0))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 89, lpszPos);
	  return FALSE;
	}
      // If a previous effect has involved this flag, output error
      if ((m_nNeighbourFlagsToSet & nBitEffect)
	  || (m_nNeighbourFlagsToClear & nBitEffect))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 88);
	  return FALSE;
	}
      // If the effect is to set the flag...
      if (strcmp(lpszSetOrClear, INI_EFFECT_SET) == 0)
	m_nNeighbourFlagsToSet += nBitEffect;  // Update the set mask
      else  // Otherwise...
	m_nNeighbourFlagsToClear += nBitEffect;  // Update the clear mask

      // Find next effect
      lpszPos = strtok(NULL, INI_EFFECTS_SEPARATOR);
    }


  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Deletes the probability array
*
*************************************************************************/

C_Complex_Type_Neighbour_Sensitive_Reaction::~C_Complex_Type_Neighbour_Sensitive_Reaction(void)
{
  if (m_pNSPrArray != NULL)
    delete[] m_pNSPrArray;
}


/*************************************************************************
*
* METHOD NAME:	Exec_Event
*
* DESCRIPTION:	Called by the event manager to execute an event which will
*		change the probability of this reaction.
*
* PARAMETERS:	double* p_flPr		- Pointer to new probability
*		long*   p_nParameter	- Pointer to parameter array containing
*					  information indicating which states
*					  are affected (ie Wildcard mask and
*					  value)
*
*************************************************************************/

void
C_Complex_Type_Neighbour_Sensitive_Reaction::Exec_Event(double* p_flPr,
							long* p_nParameter)
{
  long nState;
  long nWildcardMask;
  long nWildcardValue;

  // Extract parameters from array
  nWildcardMask = p_nParameter[MS_EVENT_PARAM_WILDCARD_MASK];
  nWildcardValue = p_nParameter[MS_EVENT_PARAM_WILDCARD_VALUE];

  // Loop through all the possible states for this neighbour_sensitive complex
  for (nState = 0; nState < m_nMaxState; nState ++)
    // If this wildcard string specifies this state...
    if ((nState & nWildcardMask) == nWildcardValue)
      // Update this state's probability of reaction
      m_pNSPrArray[nState] = p_flPr[0];
}


/*************************************************************************
*
* METHOD NAME:	Get_Flag_Modifiers
*
* DESCRIPTION:	Retrieves the bit strings indicating which flags in the
*		neighbour's state need clearing and setting.
*
* PARAMETERS:	long*  p_nNeighbourFlagsToClear - Bit string for flags to clear
*		long*  p_nNeighbourFlagsToSet   - Bit string for flags to set
*
*************************************************************************/

void
C_Complex_Type_Neighbour_Sensitive_Reaction::Get_Flag_Modifiers(long* p_nNeighbourFlagsToClear, long* p_nNeighbourFlagsToSet)
{
  *p_nNeighbourFlagsToClear = m_nNeighbourFlagsToClear;
  *p_nNeighbourFlagsToSet = m_nNeighbourFlagsToSet;
}


/*************************************************************************
*
* METHOD NAME:	Get_Relative_Probability
*
* DESCRIPTION:	This method returns the relative probability of a
*               neighbour-sensitive reaction.  The state of the neighbour
*               to which this reaction is sensitive to is retrieved, and
*               the appropriate relative probability is returned.
*               Note that this value is not the final relative probability
*               of an NS reaction.  The final relative probability is
*               calculated in
*               C_Complex_Type_Neighbour_Sensitive::Get_Relative_Probability()
*               by multiplyting this return value with the return value of
*               C_Complex_Type_Multistate::Get_Relative_Probability().
*
* PARAMETERS:	long    nNeighbourState - The state of the neighbour to which
*                                         this reaction is sensitive to
*
* RETURNS:      double  - Relative probability for this neighbour-sensitive
*                         complex (for reaction nReaction, in state 
*                         m_nState, and with neighbours in P_Neighbour).
*                 
*************************************************************************/

// *** IMPLEMENTED WITHIN CLASS DEFINITION IN HEADER FILE ***
