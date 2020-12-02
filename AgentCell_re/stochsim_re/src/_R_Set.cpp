/*************************************************************************
*
* FILENAME:	R_Set.cpp
*
* CLASS:	C_Reaction_Set
*
* DESCRIPTION:	This class represents a set of reactions. All the reactions
*		in a set have identical substrates by definition, but will
*		have different products and probabilities of reaction.
*		For example, the following reactions could be in the same
*		set:	(TTWWAA ->) TTWW + AA
*			(TTWWAA ->) TT + WWAA
*			(TTWWAA ->) TTW + WAA
*			(TTWWAA ->) TT + WW + AA
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Sets members variables to initial values.
*
* PARAMETERS:   C_Application*  p_App  - Pointer to parent application
q*
*************************************************************************/

C_Reaction_Set::C_Reaction_Set(C_Application* p_App)
{
  m_pApp = p_App;
  m_nNumReactions = 0;
  P_R = NULL;
}


/*************************************************************************
*
* METHOD NAME:	New_Reaction
*
* DESCRIPTION:	Creates a new reaction in the set. The last reaction in
*		the set is found and the number of reactions counter is
*		incremented. A new reaction is then created by calling
*		C_Reaction::New_Reaction.
*
* PARAMETERS:	long		 nNumProducts	  - The number of products
*						    in the reaction.
*		C_Complex_Type** P_P_Products	  - Pointer to an array of
*                                                   pointers to product
*                                                   complex types.
*		double		 flPr		  - Probability of reaction.
*		long		 nID		  - Reaction ID
*		Bool		 bSubst1Conserved - Flag indicating if
*                                                   substrate 1 is conserved
*                                                   in reaction.
*		Bool		 bSubst2Conserved - Flag indicating if
*                                                   substrate 2 is conserved
*                                                   in reaction.
*		Bool*		 bProductsConserved
*					          - Pointer to array of flags
*						    indicating if products
*						    are conserved in reaction.
*		C_Reaction**	 P_P_Reaction	  - Pointer to reaction
*                                                   created.
*
* RETURNS:	C_Reaction**	 P_P_Reaction	 - Pointer to reaction created.
*		int		 0		 - No errors.
*			         x (> 0)	 - Error in reaction x
*						   (ie in R[x - 1]).
*
*************************************************************************/

int
C_Reaction_Set::New_Reaction(long nNumProducts,
				 C_Complex_Type** P_P_Products, double flPr,
				 long nID, Bool bSubst1Conserved,
				 Bool bSubst2Conserved,
				 Bool* bProductsConserved,
				 C_Reaction** P_P_Reaction)
{
  int	nEndOfSet;
  int nCount;

  // Find last reaction in set
  nEndOfSet = m_nNumReactions;
  // Check there is enough memory for new reaction
  if (nEndOfSet >= MAX_NUM_REACTIONS_IN_SET)
    {		
      m_pApp->Message(MSG_TYPE_ERROR, 17);
      return -1;
    }

  // If there are no reactions in this set yet
  if (m_nNumReactions == 0)
    {
      // Create array of pointers to reactions
      P_R = new C_Reaction* [MAX_NUM_REACTIONS_IN_SET];
      // If the array was not created correctly...
      if (P_R == NULL)
	{
	  // Output error
	  m_pApp->Message(MSG_TYPE_ERROR, 8, "array of Reactions");
	  return m_nNumReactions;
	}
      // Initialise all pointers to reactions
      for (nCount = 0; nCount < MAX_NUM_REACTIONS_IN_SET; nCount ++)
	P_R[nCount] = NULL;
    }

  // Attempt to create new reaction
  P_R[m_nNumReactions] = new C_Reaction(m_pApp);
  // If the reaction was not created correctly...
  if (P_R[m_nNumReactions] == NULL)
    {
      // Output error
      m_pApp->Message(MSG_TYPE_ERROR, 8, "Reaction");
      return m_nNumReactions;
    }

  // Save pointer to the new reaction
  *P_P_Reaction = P_R[m_nNumReactions];
  // Attempt to create new reaction; if there is an error,
  if (!P_R[m_nNumReactions]->New_Reaction(nNumProducts, P_P_Products, flPr,
					  nID, bSubst1Conserved,
					  bSubst2Conserved,
					  bProductsConserved))
    // return reaction number
    return m_nNumReactions + 1;
  // Increment number of reactions in set
  m_nNumReactions ++;
  // Return without error
  return 0;
}


/*************************************************************************
*
* METHOD NAME:	Scale
*
* DESCRIPTION:	Loops through all the reactions, changing their
*		probabilities by a scale factor.
*		ie New_Probability = Old_Probability * flScale
*
* PARAMETERS:	double flScale - Scale factor used to multiply probabilities.
*
*************************************************************************/

void
C_Reaction_Set::Scale(double flScale)
{
  int i;
  // Loop through all reactions and set new probability
  for (i = 0; i < m_nNumReactions; i++)
    P_R[i]->Set_Probability(flScale * P_R[i]->Get_Probability());	
  return;
}


/*************************************************************************
*
* METHOD NAME:	Validate
*
* DESCRIPTION:	Loop through all the reactions and validate the probabilities
*		by calling C_Reaction::Validate.
*
* RETURNS:	Bool	TRUE	- Probabilities validated successfully
*			FALSE	- A probability failed validation
*
*************************************************************************/

Bool
C_Reaction_Set::Validate(void)
{
  int i;
  // Loop through all probabilities	
  for (i = 0; i < m_nNumReactions; i++)
    // If probability validation fails...
    if (! P_R[i] -> Validate (P_R[i] -> Get_Probability()) )
      // Return with error
      return FALSE;
  // Return withour error
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	This method clears up the memory used to store the reactions.
*
*************************************************************************/

C_Reaction_Set::~C_Reaction_Set(void)
{
  long nCount;
  // If there are reactions defined
  if (m_nNumReactions > 0)
    {
      // Delete each reaction
      for (nCount = 0; nCount < m_nNumReactions; nCount ++)
	if (P_R[nCount] != NULL)
	  delete P_R[nCount];
      // Delete array used to store reaction array
      delete P_R;
    }
}
