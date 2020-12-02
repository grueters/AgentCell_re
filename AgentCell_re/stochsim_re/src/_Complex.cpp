/*************************************************************************
*
* FILENAME:	Complex.cpp
*
* CLASS:	C_Complex
*
* DESCRIPTION:	This class represents each complex in the system. Each object
*		of type C_Complex is an independent particle (complex) which
*		can react with the other particles (C_Complex's) in the
*		system. Each object is a placeholder which may or may not
*		actually exist physically; each object has a particular
*		type (pointer to a C_Complex_Type). In other words, if the
*		maximum number of complexes which can exist in the system
*		at any one time is N, and there are x1 complexes of species
*		S1 and x2 complexes of species S2, then there are:
*			x1 C_Complex objects of type S1;
*			x2 C_Complex objects of type S2;
*			N-(x1+x2) C_Complex objects with no type.
*		Note that this class has access to private data members
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	This method initialises member variables so that the
*		complex is 'empty', ie it is an empty placeholder.
*
* PARAMETERS:   C_Application*  p_App  - Pointer to parent application
*              C_System* P_System  - Pointer to the system containing the
*				      complex (ie the parent system of the
*				      complex).
*		long	  nSystemID - ID this complex has in the system.
*				      (Actually the # in the array of C_Complex
*				       in C_System).
*
*************************************************************************/

C_Complex::C_Complex(C_Application* p_App, C_System* P_System, long nSystemID)
	{

// Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		      "\t\tC_Complex::C_Complex: Constructor with ID %d\n",
		      nSystemID);	
#endif

	m_pApp = p_App;
	P_Parent = P_System;
	m_bExists = FALSE;
	// Set the ID this complex has in the system
	// (in fact the # in the array of C_Complex objects in C_System)
	m_nSystemID = nSystemID;
	}


/*************************************************************************
*
* METHOD NAME:	Test_Reaction
*
* DESCRIPTION:	This method is called when this complex has been selected
*		for reaction - either with another complex (in which case
*		the pointer to the other complex is passed in) or uni-
*		molecular reaction. The appropriate set of reactions is
*		selected from the reaction matrix. A random number
*		is used to select a reaction based on the probabilities
*		of the reaction. For instance, if reaction set is:
*			TTWWAA -> TT + WWAA (Pr = 0.2)
*			TTWWAA -> TTWW + AA (Pr = 0.3)
*		If the random number is 0 - 0.2, the first reaction
*		will occur; if the number is 0.2 - 0.5, the second
*		reaction will occur; if the number is greater than
*		0.5, no reaction will occur. If a reaction occurs,
*		the appropriate methods are called 
*		(C_Complex::Bimolecular_React and 
*		C_Complex::Unimolecular_React).
*
* PARAMETERS:	C_Complex* P_Reactant - Pointer to the second reactant (NULL
*		is the reaction is unimolecular).
*
* RETURNS:	Bool	TRUE		  - Reaction was tested successfully.
*			FALSE		  - An error occurred while testing
*					    for a reaction.
*
*************************************************************************/

Bool
C_Complex::Test_Reaction (C_Complex* P_Reactant)
{
  int 			nR_M_Row;
  int 			nR_M_Col;
  double 			flCumulativePr;
  double			flPr;
  double			flRanNum;
  int 			i;
  C_Reaction_Set*	P_Reaction_Set;	
  C_Reaction*		P_Reaction;

  flCumulativePr = 0;
  i = 0;

  // Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  if (P_Reactant != NULL)
    _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		  "\nC_Complex::Test_Reaction: Complex %s (%d) x %s (%d) chosen to react\n",
		  Get_Type()->Display_String(), Get_System_ID(), 
		  P_Reactant->Get_Type()->Display_String(),
		  P_Reactant->Get_System_ID());	
  else
    _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		  "\nC_Complex::Test_Reaction: Complex %s (%d) chosen to react\n",
		  Get_Type()->Display_String(), Get_System_ID());	
#endif

  // Get random floating point number

  flRanNum = P_Parent->P_Ran_Gen->New_Num();

  // Equilibrate any rapid equilibria (eg in special complex types)

#ifdef _DYNAMIC_UPDATE
  // Decrement the level of this complex type
  P_Type->Dec_Level(this);
#endif /* _DYNAMIC_UPDATE */

  Equilibrate();												// Equilibrate this complex
#ifdef _DYNAMIC_UPDATE
  // Increment the level of this complex type
  P_Type->Inc_Level(this);
#endif /* _DYNAMIC_UPDATE */

  // If there is a second reactant
  if (P_Reactant != NULL)
    {
#ifdef _DYNAMIC_UPDATE
      // Decrement the level of this complex type
      P_Reactant->P_Type->Dec_Level(P_Reactant);
#endif /* _DYNAMIC_UPDATE */

      // Equilibrate second complex
      P_Reactant->Equilibrate();

#ifdef _DYNAMIC_UPDATE
      // Increment the level of this complex type
      P_Reactant->P_Type->Inc_Level(P_Reactant);
#endif /* _DYNAMIC_UPDATE */
    }
  
  // Get reaction set from reaction matrix

  // Get row number of set based on this complex type
  nR_M_Row = P_Type->Get_Reaction_Index() + 1;
  // If there is only one reactant...
  if (P_Reactant == NULL)
    // Use zero column (for unimolecular reaction)
    nR_M_Col = 0;
  else  // Otherwise...
    // Get column number of set based on reactant complex type
    nR_M_Col = P_Reactant->P_Type->Get_Reaction_Index() + 1;
  P_Reaction_Set
    = P_Parent->Get_Reaction_Matrix()->Get_Set(nR_M_Row, nR_M_Col);

  // Select reaction from reaction set

  // Loop through all reactions keeping a running total
  // of probabilities until random number is exceeded 
  while ((i < P_Reaction_Set->Get_Num_Reactions())
	 && (flCumulativePr <= flRanNum))
    {
      P_Reaction = P_Reaction_Set->Get_Reaction(i);
      // Get the base probability for the reaction
      flPr = P_Reaction->Get_Probability();
      // Get any modifiers for this complex type
      flPr *= Get_Relative_Probability(P_Reaction);
      // If there is a second reactant, get the modifiers
      // for the second complex type
      if (P_Reactant != NULL)
	flPr *= P_Reactant->Get_Relative_Probability(P_Reaction);
      // Keep track of the current cumulative probability
      flCumulativePr += flPr;
      i++;
    }

  // If a reaction was selected...
  if (flCumulativePr > flRanNum)
    // If there is no second reactant current complex undergoes
    // unimolecular reaction
    if (P_Reactant == NULL)
      return Unimolecular_React(P_Reaction_Set->Get_Reaction(i - 1));
    else  // Otherwise, bind current complex to second reactant
      return Bimolecular_React(P_Reactant,
			       P_Reaction_Set->Get_Reaction(i - 1));

  return TRUE;  // Return indicating no error
}


/*************************************************************************
*
* METHOD NAME:	Bimolecular_React
*
* DESCRIPTION:	Reacts this complex with another reactant (passed in as a
*		parameter) according to the specified reaction. This complex
*		and the reactant are destroyed and new product complexes
*		are created if they are not conserved, according to the
*		product list in the specified reaction.
*
* PARAMETERS:	C_Complex*  P_Reactant	- Second reactant in reaction
*		C_Reaction* P_Reaction	- Pointer to reaction in reaction
*					  matrix which is occurring.
*
* RETURNS:	Bool	TRUE		- Reaction was tested successfully.
*			FALSE		- An error occurred while testing
*					  for a reaction.
*
*************************************************************************/

Bool
C_Complex::Bimolecular_React(C_Complex* P_Reactant, C_Reaction* P_Reaction)
{
  int			i;
  C_Complex_Type**	P_P_Products;

// Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "\tC_Complex::Bimolecular_React\n");
#endif

  // First, create new products

  i = 0;
  // Get the null terminated list of products from the reaction
  P_P_Products = P_Reaction->Get_Products();
  // While the end of the product list is not reached...
  while (P_P_Products[i] != NULL)
    {
      // If this product is not conserved
      if (!P_Reaction->Product_Conserved(i)) 
	// Create a new complex of the product type
	if (!P_P_Products[i]->Create(P_Reaction))
	  return FALSE;
      i ++;
    }

  // Second, destroy substrate 2

  // If the second reactant is not conserved
  if (!P_Reaction->Substrate_2_Conserved())
    {

// If this is a build with averaging enabled...
#ifdef _DYNAMIC_UPDATE
      // Decrement the level of this complex type now
      P_Reactant->P_Type->Dec_Level(P_Reactant);
#endif

      // Destroy the second reactant
      if (!P_Reactant->P_Type->Destroy(P_Reactant))
	return FALSE;
    }
  else  // Otherwise,
    // Update the second reactant (only relevant for special types)
    P_Reactant->Update(P_Reaction);

  // Finally, destroy substrate 1
  // (we do this last as this object may not be valid after destruction)

  // If the first reactant is not conserved
  if (!P_Reaction->Substrate_1_Conserved())
    {

// If this is a build with averaging enabled...
#ifdef _DYNAMIC_UPDATE
      // Decrement the level of this complex type now
      P_Type->Dec_Level(this);
#endif /* _DYNAMIC_UPDATE */

      // Destroy the first reactant
      if (!P_Type->Destroy(this))
	return FALSE;
    }
  else  // Otherwise,
    // Update the first reactant (only relevant for special types)
    this->Update(P_Reaction);

  return TRUE;  // Return indicating success
}


/*************************************************************************
*
* METHOD NAME:	Unimolecular_React
*
* DESCRIPTION:	Reacts this complex according to the reaction
*		specified. This complex is destroyed and new product
*		complexes are created if they are not conserved, according 
*		to the product list in the specified reaction.
*
* PARAMETERS:	C_Reaction* P_Reaction 	- Pointer to reaction in reaction
*					  matrix which is occurring.
*
* RETURNS:	Bool	TRUE		- Reaction was tested successfully.
*			FALSE		- An error occurred while testing
*					  for a reaction.
*
*************************************************************************/

Bool 
C_Complex::Unimolecular_React(C_Reaction* P_Reaction)
{
  int			i;
  C_Complex_Type**	P_P_Products;

// Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\tC_Complex::Unimolecular_React\n");	
#endif
#ifdef _NS_R_DEBUG  // For debugging neighbour-sensitive reactions
  P_Type->Inc_Count(P_Reaction);
#endif /* _NS_R_DEBUG */

  // First, create new products

  i = 0;
  // Get the null terminated list of products from the reaction
  P_P_Products = P_Reaction->Get_Products();
  // While the end of the product list is not reached...
  while (P_P_Products[i] != NULL)
    {
      // If this product is not conserved
      if (!P_Reaction->Product_Conserved(i))
	// Create a new complex of the product type
	if (!P_P_Products[i]->Create(P_Reaction))
	  return FALSE;
      i ++;
    }

  // Second, destroy substrates

  // If the first reactant is not conserved
  if (!P_Reaction->Substrate_1_Conserved())
    {

// If this is a build with averaging enabled...
#ifdef _DYNAMIC_UPDATE
      // Decrement the level of this complex type now
      P_Type->Dec_Level(this);
#endif /* _DYNAMIC_UPDATE */

      // Destroy this complex
      if (!P_Type->Destroy(this))
	return FALSE;
    }
  else  // Otherwise
    // Update the first reactant (only relevant for special types)
    this->Update(P_Reaction);

  return TRUE;  // Return indicating success
}
