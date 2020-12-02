/*************************************************************************
*
* FILENAME:	Com_MS.cpp
*
* CLASS:	C_Complex_Multistate
*
* DESCRIPTION:	This class represents each a multistate complex. The type of
*		this complex is based on the class 
*		C_Complex_Type_Multistate. This class is derived from
*		C_Complex.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	This method initialises the state of the complex.
*
* PARAMETERS:   C_Application*  p_App - Pointer to parent application
*
*         	C_System* P_System  - Pointer to the system containing the
*				      complex (ie the parent system of the
*				      complex).
*		long	  nSystemID - ID this complex has in the system.
*				      (Actually the # in the array of C_Complex
*				       in C_System).
*
*************************************************************************/

C_Complex_Multistate::C_Complex_Multistate(C_Application* p_App,
					   C_System* P_Parent, long nSystemID)
  : C_Complex(p_App, P_Parent, nSystemID)
{
// Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\t\tC_Complex_Multistate::C_Complex_Multistate: Constructor with ID %d\n",
		Get_System_ID());	
#endif

  m_nState = 0;
}


/*************************************************************************
*
* METHOD NAME:	Equilibrate
*
* DESCRIPTION:	This method is called when this complex has been selected
*		for reaction (called in C_Complex::Test_Reaction) and 
*		before complex levels are displayed. The state
*		of the complex is updated (ie any state flags depending on 
*		rapid equilibria are equilibriated). The ancestor method is 
*		then called.
*
*************************************************************************/

void
C_Complex_Multistate::Equilibrate()
{
  C_Complex_Type_Multistate*	P_MS_Type;
  double			flRanNum;
  long				nMask;
  int				nFlag;

// Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\tC_Complex_Multistate::Equilibrate\n");	
#endif

  // Get a multistate complex type pointer to this complex type
  P_MS_Type = (C_Complex_Type_Multistate*) P_Type;

  // Equilibrate the states subject to rapid equilibria

  nMask = 1;
  // Loop through each state flag
  //   (Note we are moving from right to left, from LSB to MSB)
  for (nFlag = P_MS_Type->Get_Num_Flags() - 1; nFlag >= 0; nFlag --)
    {
      // If the flag is controlled by rpd eqm...
      if (P_MS_Type->Get_Is_Rpd_Eqm(nFlag))
	{
	  // Get a random number between 0 and 1
	  flRanNum = P_Parent->P_Ran_Gen->New_Num();
	  // If the random number is less than the probability
	  // that the flag is set...
	  if (flRanNum < P_MS_Type->Get_Pr_Flag_Set(nFlag, m_nState))
	    // Set this particular flag by ORing with the mask
	    m_nState |= nMask;
	  else  // Otherwise...
	    // Clear this particular flag by ANDing it
	    // with the complement of the mask
	    m_nState &= (~nMask);
	}
      // Update the mask by shifting the bit one place to the left
      nMask <<= 1;
    }

  // Now the new state is set, call the ancestor method and return
 
  C_Complex::Equilibrate();
}


/*************************************************************************
*
* METHOD NAME:	Get_Relative_Probability
*
* DESCRIPTION:	Retrieves the probability associated with a particular
*		reaction for the current state of the multistate complex.
*		The relative probability is returned.
*
* PARAMETERS:	C_Reaction*	P_Reaction	- Reaction ID
*
* RETURNS:	double	- Relative probability for this multistate complex
*			  reacting in reaction nReaction when in the state
*			  m_nState.
*
*************************************************************************/

double
C_Complex_Multistate::Get_Relative_Probability(C_Reaction* P_Reaction)
{
  return (((C_Complex_Type_Multistate*) P_Type)
	  -> Get_Relative_Probability(P_Reaction, m_nState));
}


/*************************************************************************
*
* METHOD NAME:	Update
*
* DESCRIPTION:	Updates the state of this complex according to the 
*		specified reaction.
*
* PARAMETERS:	C_Reaction* P_Reaction 	- Pointer to reaction in reaction
*					  matrix which is occurring.
*
*************************************************************************/

void
C_Complex_Multistate::Update(C_Reaction* P_Reaction)
{
  long nFlagsToClear;
  long nFlagsToSet;

// Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\t\tC_Complex_Multistate::Update\n");	
#endif

  // Get the bit strings indicating which flags to set and clear
  ((C_Complex_Type_Multistate*) P_Type)
    -> Get_Flag_Modifiers(P_Reaction, &nFlagsToClear, &nFlagsToSet);

// If this is a build with averaging enabled...
#ifdef _DYNAMIC_UPDATE
      // First, decrement the level of this state, and...
      P_Type->Dec_Level(this);
  // Unless there are no flags to modify...
      //  if ((nFlagsToClear | nFlagsToSet) != 0)
      //    {
#endif
      // Set the necessary flags by ORing with the set flags bit string
      m_nState |= nFlagsToSet;
      // Clear the necessary flags by ANDing with the complement
      // of the clear flags bit string
      m_nState &= (~nFlagsToClear);

// If this is a build with averaging enabled...
#ifdef _DYNAMIC_UPDATE
      //    }
      // Increment the level of the new state
      //tmp 1 line for dbg
      Equilibrate();
      P_Type->Inc_Level(this);
#endif
}
