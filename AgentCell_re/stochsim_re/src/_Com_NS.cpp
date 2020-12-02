/*************************************************************************
*
* FILENAME:	Com_NS.cpp
*
* CLASS:	C_Complex_Neighbour_Sensitive
*
* DESCRIPTION:	This class represents each a multistate complex. The type of
*		this complex is based on the class 
*		C_Complex_Type_Neighbour_Sensitive. This class is derived from
*		C_Complex_Multistate.
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
*        	C_System* P_System   - Pointer to the system containing the
*				       complex (ie the parent system of the
*				       complex).
*		long      nSystemID  - ID this complex has in the system.
*				       (Actually the # in the array of
*                                      C_Complex in C_System).
*		long      nNumNeighbours
*                                    - Number of neighbours this complex will
*                                      have in the array. (This must match
*                                      the geometry of the array.)
*
*************************************************************************/

C_Complex_Neighbour_Sensitive::C_Complex_Neighbour_Sensitive
(C_Application* p_App, C_System* P_Parent, long nSystemID, long nNumNeighbours)
  : C_Complex_Multistate(p_App, P_Parent, nSystemID)
{
// Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\t\tC_Complex_Neighbour_Sensitive::C_Complex_Neighbour_Sensitive: Constructor with ID %d\n",
		Get_System_ID());	
#endif

  m_bIsArrayMember = FALSE;
  m_nNumNeighbours = nNumNeighbours;
  m_nDefaultCouplingValue = (m_nNumNeighbours-(m_nNumNeighbours%2))/2;
  P_Neighbour = new C_Complex_Neighbour_Sensitive* [nNumNeighbours];
  for(int i = 0; i < nNumNeighbours; i++)
    {
      P_Neighbour[i] = NULL;
    }

#ifdef _CA_DEBUG  // For debugging complex arrays
  m_pAddress = new C_Integer_Pair;
#endif /* _CA_DEBUG*/
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
C_Complex_Neighbour_Sensitive::Equilibrate()
{
  C_Complex_Type_Multistate*	        P_MS_Type;
  C_Complex_Type_Neighbour_Sensitive*	P_NS_Type;
  double				flRanNum;
  long					nMask;
  int					nFlag;
  int					nCountNeighbour;
  int					nCouplingValue;

  // Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\tC_Complex_Neighbour_Sensitive::Equilibrate\n");	
#endif

	
  P_MS_Type = (C_Complex_Type_Multistate*) P_Type;
  P_NS_Type = (C_Complex_Type_Neighbour_Sensitive*) P_Type;

#ifdef _NSDEBUG
  P_Parent->EQUIL_COUNT++;
#endif	

  // Equilibrate the states subject to rapid equilibria

  nMask = 1;
  // Loop through each state flag
  //   (Note we are moving from right to left, from LSB to MSB)
  for (nFlag = P_MS_Type->Get_Num_Flags() - 1; nFlag >= 0; nFlag --)
    {
      // If the flag is controlled by neighbour-sensitive rpd eqm...
      if (P_NS_Type->Get_Is_NSRpd_Eqm(nFlag))
	{
	  nCouplingValue = 0;  // Reset coupling value
	  if (m_bIsArrayMember)
	    {
	      // Loop through all the neighbours
	      for (nCountNeighbour = 0; nCountNeighbour < m_nNumNeighbours;
		   nCountNeighbour++)
		if (P_Neighbour[nCountNeighbour] != NULL)
		  // Check if the state of this neighbour is a coupled state
		  if(P_NS_Type->Is_Coupled_State(nFlag,
						 P_Neighbour[nCountNeighbour]
						 ->Get_State()))
		    nCouplingValue++;
	    }
	  else
	    nCouplingValue = m_nDefaultCouplingValue;

	  // Get a random number between 0 and 1
	  flRanNum = P_Parent->P_Ran_Gen->New_Num();
	  // If the random number is less than the probability
	  // that the flag is set...
	  if (flRanNum < P_NS_Type->Get_Pr_Flag_Set(nFlag, m_nState,
						    nCouplingValue))
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

  // Now that the new state is set, call the ancestor method and return
  C_Complex_Multistate::Equilibrate();
}

/*************************************************************************
*
* METHOD NAME:	Set_Neighbours
*
* DESCRIPTION:	This method sets up the list of pointers to the neighbours
*		of this complex.
*
* PARAMETERS:	C_Complex**	P_NeighbourList
*                                         - Array of pointers to the neighbour
*                                           C_Complex objects.
*
*************************************************************************/

void
C_Complex_Neighbour_Sensitive::Set_Neighbours(C_Complex_Multistate** P_NeighbourList, long nNumNeighbours)
{
  int nCount;
  for (nCount = 0; nCount < m_nNumNeighbours; nCount++)
    P_Neighbour[nCount]
      = (C_Complex_Neighbour_Sensitive*)(P_NeighbourList[nCount]);
}

/*************************************************************************
*
* METHOD NAME:	Get_Relative_Probability
*
* DESCRIPTION:	This method calculates and returns the relative 
*               probability of a neighbour-sensitive reaction
*
* PARAMETERS:	C_Reaction* P_Reaction           - Reaction ID 
*               C_Complex_Neighbour_Sensitive**  P_Neighbours
*					- List of pointers to neighbours
*
* RETURNS:      double  - Relative probability for this neighbour-sensitive
*                         complex (for reaction nReaction, in state 
*                         m_nState, and with neighbours in P_Neighbour).
*                 
*************************************************************************/

double
C_Complex_Neighbour_Sensitive::Get_Relative_Probability(C_Reaction* P_Reaction)
{
  return (((C_Complex_Type_Neighbour_Sensitive*) P_Type)
	  -> Get_Relative_Probability(P_Reaction, m_nState, P_Neighbour));
}

/*************************************************************************
*
* METHOD NAME:	Update
*
* DESCRIPTION:	This version of this overloaded method updates the state
*               of this complex AND its neighbour according to the
*               specified reaction.  First, the Update() method of the
*               base class (C_Complex_Multistate) is called to update the
*               flags of this complex.  Then, the reaction is checked to
*               see if it is a neighbour-sensitive reaction.  If it is,
*               the second version of this overloaded method is called to
*               update the state of the neighobur.
*
* PARAMETERS:	C_Reaction* P_Reaction 	- Pointer to reaction in reaction
*					  matrix which is occurring.
*
*
*************************************************************************/

void
C_Complex_Neighbour_Sensitive::Update(C_Reaction* P_Reaction)
{
  long nNeighbourFlagsToClear;
  long nNeighbourFlagsToSet;
  C_Complex_Type_Neighbour_Sensitive_Reaction* p_CT_NS_R;

  // Call Update() of base class to update the flags of this complex
  C_Complex_Multistate::Update(P_Reaction);

  // If this reaction is neighbour-sensitive
  if (((C_Complex_Type_Neighbour_Sensitive*) P_Type)
      -> Get_Is_NS_Reaction(P_Reaction))
    {
      // Get the CT_NS_R
      p_CT_NS_R = (((C_Complex_Type_Neighbour_Sensitive*) P_Type)
		   -> Get_NS_Reaction_From_ID(P_Reaction->Get_ID()));
      
      // Get the bit strings indicating which flags
      // to set and clear for neighbour
      ((C_Complex_Type_Neighbour_Sensitive*) P_Type)
	-> Get_Flag_Modifiers(P_Reaction, &nNeighbourFlagsToClear,
			      &nNeighbourFlagsToSet);

      // Update the flags of the neighbour
      P_Neighbour[p_CT_NS_R->Get_Neighbour_ID()]
	-> Update(nNeighbourFlagsToClear, nNeighbourFlagsToSet);
    }
}

/*************************************************************************
*
* METHOD NAME:	Update
*
* DESCRIPTION:	This version of this overloaded method is called by a
*               neighbouring complex to update the state of this complex.
*
* PARAMETERS:	long  nFlagsToClear  - Mask representing the flags to be
*                                      cleared.
*               long  nFlagsToSet    - Mask representing the flags to be
*                                      set
*
*************************************************************************/

void
C_Complex_Neighbour_Sensitive::Update(long nFlagsToClear, long nFlagsToSet)
{

// If this is a build with averaging enabled...
#ifdef _DYNAMIC_UPDATE
  // Unless there are no flags to modify...
  if ((nFlagsToClear | nFlagsToSet) != 0)
    {
      // First, decrement the level of this state
      P_Type->Dec_Level(this);
#endif /* _DYNAMIC_UPDATE */

      // Set the necessary flags by ORing with the set flags bit string
    m_nState |= nFlagsToSet;
    // Clear the necessary flags by ANDing with the complement
    // of the clear flags bit string
    m_nState &= (~nFlagsToClear);

// If this is a build with averaging enabled...
#ifdef _DYNAMIC_UPDATE
    P_Type->Inc_Level(this);  // Increment the level of the new state
  }
#endif /* _DYNAMIC_UPDATE */
}

/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Deletes memory allocated to store pointers to neighbours
*
*************************************************************************/

C_Complex_Neighbour_Sensitive::~C_Complex_Neighbour_Sensitive(void)
{
  delete[] P_Neighbour;
#ifdef _CA_DEBUG  // For debugging complex arrays
  delete m_pAddress;
#endif /* _CA_DEBUG*/
}
