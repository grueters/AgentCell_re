/*************************************************************************
*
* FILENAME:	CT_MS_R.cpp
*
* CLASS:	C_Complex_Type_Multistate_Reaction
*
* DESCRIPTION:	This class is used to store details on a reaction which is
*		affected by or affects a multistate complex type. For each
*		possible state of the multistate complex type is a
*		rate constant scaling factor, which will be applied to the
*		rate constant of this particular reaction when a multistate
*		complex type is in that particular state. In addition
*		the effects that this reaction has on the multistate complex
*		is also stored, in terms of which state flags are set
*		and which are cleared.
*
* TYPE:		Core
*
*************************************************************************/


#include "_Stchstc.hh"




/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Initialises the reaction ID and clears other attributes.
*
* PARAMETERS:	
*               C_Application*  p_App   - Pointer to parent application
*
*               long	nReactionID - ReactionID for this object
*
*************************************************************************/

C_Complex_Type_Multistate_Reaction::C_Complex_Type_Multistate_Reaction
(C_Application* p_App, long nReactionID)
{
  m_pApp = p_App;
  m_nID = nReactionID;  // Set reaction ID of object
  m_nFlagsToClear = 0;
  m_nFlagsToSet = 0;
  m_pPrArray = NULL;
  m_nMaxState = 0;
}

		
/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the reaction probabilities. For each possible
*		state the multistate complex can be in, a bit string
*		is constructed uniquely representing this state, and
*		a rate constant scaling factor for this state is
*		read in from the INI file. Wildcard strings can be
*		used in the INI file which represent a number of different
*		states.
*
*		Then the effects of this reaction on the multistate complex
*		are read from the INI file, and two bit masks are
*		constructed: one represents the bit mask which will
*		be applied to the state of a multistate complex to set
*		various states; the other is the bit mask used for
*		clearing various states.
*
* PARAMETERS:	char*	lpszINIFile    - INI File containing details of
*                                        reaction
*		char*	lpszINISection - Section in INI File containing
*					 details of reaction
*		int	nNumFlags      - Number of state flags the multistate
*					 complex possesses.
*		char**	lpszFlag       - Pointer to array of strings
*					 identifying flag strings.
*
*		C_Complex_Type_Multistate* P_Parent
*				       - Pointer to the parent of this
*                                        reaction.
*
*		C_Event_Manager* P_Event_Manager
*				       - Pointer to the system event manager.
*
* RETURNS:	Bool	TRUE	- The reaction details were initialised 
*				  successfully
*			FALSE	- An error occurred initialising the 
*				  reaction details
*
*************************************************************************/

Bool
C_Complex_Type_Multistate_Reaction::Init (char* lpszINIFile,
					  char* lpszINISection,
					  int nNumFlags,
					  char lpszFlag[][MAX_MS_FLAG_LENGTH + 1],
					  C_Complex_Type_Multistate* P_Parent,
					  C_Event_Manager* P_Event_Manager)
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
  int	nCount;

  // Create array to store probabilities

  // Calculate the number of different states the complex can be in
  m_nMaxState = PowerOfTwo(nNumFlags);
  // Create array for storing probabilities
  m_pPrArray = new double[m_nMaxState];
  // If the array was not created correctly...
  if (m_pPrArray == NULL)
    {
      // Output error
      m_pApp->Message(MSG_TYPE_ERROR, 8,
		    "probabilities in Multistate Complexes");
      return FALSE;
    }
  // Initialise array members to -1
  // (so that it is possible to detect which members have been set later on)
  for (nState = 0; nState < m_nMaxState; nState ++)
    m_pPrArray[nState] = -1;

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
      // Loop through all the possible states for this multistate complex
      for (nState = 0; nState < m_nMaxState; nState ++)
	// If this state is defined by this wildcard string...
	if ((nState & nWildcardMask) == nWildcardValue)
	  {
	    // If this state proability has been set already
	    if (m_pPrArray[nState] != -1)
	      {
		// Output array indicating this state probability
		// has already been set
		P_Parent->Get_State_Display_String(nState,
						   lpszStateDisplayString);
		m_pApp->Message(MSG_TYPE_ERROR, 106, lpszStateDisplayString);
		return FALSE;
	      }
	    // Set this state probability
	    m_pPrArray[nState] = flPr;
	  }
      // If the probability can change over time
      // (ie reaction is a dynamic object), attempt to create events
      if (strlen(lpszPrDynCode) > 0)
	{
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

  // For each state the multistate complex can be in, create bit string
  // and load in the probability from the INI file.
  // A bit string will be created - a binary string equivalent of nState.
  // This is only done for the states which haven't been initialised yet;
  // some probilities will have been set already using wildcards above.

  nDefaultRate = 0;
  for (nState = 0; nState < m_nMaxState; nState ++)
    // If state has not been set yet...
    if (m_pPrArray[nState] == -1)
      {
	lpszPrDynCode[0] = NULL_CHAR;
	// Create a bit string equivalent of the state
	LongToBitString(nState, lpszBitString, nNumFlags);
	// Retrieve the relative probability from the INI file.
	// If not found...
	if (!m_pApp->Get_INI(lpszINISection, lpszBitString, lpszPr, lpszINIFile,
			   TRUE))
	  {
	    // ...use default value
	    m_pPrArray[nState] = MS_DEFAULT_RATE;
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
	    m_pPrArray[nState] = atof(lpszPr);
	  }
	// If probability exceeds 1.0, output error
	// (the number should be a ratio so cannot exceed one)
	if (m_pPrArray[nState] > 1.00)
	  {
	    m_pApp->Message(MSG_TYPE_ERROR, 90);
	    return FALSE;
	  }
	// If the probability can change over time
	// (ie reaction is a dynamic object), attempt to create events
	if (strlen(lpszPrDynCode) > 0)
	  {
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
      m_pApp->Message(MSG_TYPE_STATUS, 123, lpszParaList);
    }

  // Create masks which are used when the reaction occurs.
  // The mask is used to set or clear the appropriate state flags

  // Start of by assuming no flags need setting or clearing
  m_nFlagsToClear = 0;
  m_nFlagsToSet = 0;

  // Retrieve the list of effects from the INI file
  if (!m_pApp->Get_INI(lpszINISection, INI_REACTION_EFFECTS, lpszEffectsString,
		     lpszINIFile))
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
      if ((m_nFlagsToSet & nBitEffect) || (m_nFlagsToClear & nBitEffect))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 88);
	  return FALSE;
	}

      // If the effect is to set the flag...
      if (strcmp(lpszSetOrClear, INI_EFFECT_SET) == 0)
	m_nFlagsToSet += nBitEffect;  // ...update the set mask
      else  // Otherwise...
	m_nFlagsToClear += nBitEffect;  // ...update the clear mask

      // Find next flags
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

C_Complex_Type_Multistate_Reaction::~C_Complex_Type_Multistate_Reaction(void)
{
  if (m_pPrArray != NULL)
    delete[] m_pPrArray;
}

	


/*************************************************************************
*
* METHOD NAME:	Get_Flag_Modifiers
*
* DESCRIPTION:	Retrieves the bit strings indicating which flags in the
*		state need clearing and setting.
*
* PARAMETERS:	long*		p_nFlagsToClear - Bit string for flags to clear
*		long*		p_nFlagsToSet   - Bit string for flags to set
*
*************************************************************************/

void
C_Complex_Type_Multistate_Reaction::Get_Flag_Modifiers(long* p_nFlagsToClear,
						       long* p_nFlagsToSet)
{
  *p_nFlagsToClear = m_nFlagsToClear;
  *p_nFlagsToSet = m_nFlagsToSet;
}




/*************************************************************************
*
* METHOD NAME:	Validate_Dynamic_Values
*
* DESCRIPTION:	Validates a dynamic value during initialisation.
*
* PARAMETERS:	int	nNumValues  - Number of data items in array
*				      (Always 1 for this dyn object)
*		double* p_flPr	    - Pointer to probabilities to validate.
*
* RETURNS:	Bool	TRUE	    - Probability passed validation
*			FALSE       - Probability failed validation
*
*************************************************************************/

Bool
C_Complex_Type_Multistate_Reaction::Validate_Dynamic_Values(int,
							    double* p_flPr)
{
  // If probability exceeds 1.0, output error
  // (the number should be a ratio so cannot exceed one)
  if (p_flPr[0] > 1.00)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 90);
      return FALSE;
    }
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Exec_Event
*
* DESCRIPTION:	Called by the event manager to execute an event which will
*		change the probability of this reaction.
*
* PARAMETERS:	double* p_flPr		- Pointer to new probability
*		long* p_nParameter	- Pointer to parameter array containing
*					  information indicating which states
*					  are affected (ie Wildcard mask and
*					  value)
*
*************************************************************************/

void
C_Complex_Type_Multistate_Reaction::Exec_Event(double* p_flPr,
					       long* p_nParameter)	
{
  long nState;
  long nWildcardMask;
  long nWildcardValue;
  // Extract parameters from array
  nWildcardMask = p_nParameter[MS_EVENT_PARAM_WILDCARD_MASK];
  nWildcardValue = p_nParameter[MS_EVENT_PARAM_WILDCARD_VALUE];
  // Loop through all the possible states for this multistate complex
  for (nState = 0; nState < m_nMaxState; nState ++)
    // If this wildcard string specifies this state...
    if ((nState & nWildcardMask) == nWildcardValue)
      // Update this state's probability of reaction
      m_pPrArray[nState] = p_flPr[0];
}
