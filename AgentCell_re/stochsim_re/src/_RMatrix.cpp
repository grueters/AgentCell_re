/*************************************************************************
*
* FILENAME:	RMatrix.cpp
*
* CLASS:	C_Reaction_Matrix
*
* DESCRIPTION:	This class represents a matrix of reactions - all the
*		reactions within the system. All reactions are defined
*		by their substrates. A reaction is either a unimolecular
*		(one substrate) or a bimolecular reaction (two substrates).
*		It is assumed that any reaction containing more than two
*		substrates can be broken down into multiple reactions
*		containing, at most, two substrates.
*
*		The matrix is two dimensional; row represents the first
*		substrate in the reaction and the column represents
*		the second substrate (or zero if the reaction is
*		unimolecular). Each element within
*		the matrix is a reaction set (see C_Reaction_Set for more
*		details). Each set specifies the number of reactions 
*		within that particular set and an array of reactions (see
*		C_Reaction for more details). These relationships can
*		be summarised as follows:
*
*				  C_Reaction_Matrix  
*					  |
*					  | 
*					 /|\
*				    C_Reaction_Set
*					  |      \
*					  |       \ 
*					 /|\       \ 
*				      C_Reaction    Number of Reactions
*					 / \
*					/   \
*				       /     \
*				  Products  Probability
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Initialises reaction matrix and set member variables.
*
* PARAMETERS:   C_Application*  p_App  - Pointer to parent application
*
*           	C_Complex_Type** P_P_Complex_Type_System
*                                                 - Pointer to the complex
*						    type array used in the
*						    system.
*		long		 nNumComplexTypes - Number of complex types
*						    used in the system.
*
*************************************************************************/

C_Reaction_Matrix::C_Reaction_Matrix(C_Application* p_App,
				     C_Complex_Type** P_P_Complex_Type_System,
				     long nNumComplexTypes)
{
  int i,j;
  m_pApp = p_App;
  m_nSize = nNumComplexTypes;
  P_P_Complex_Type = P_P_Complex_Type_System;

  for (i=0;i < MAX_COMPLEX_TYPES + 1; i++)
    for (j=0;j < MAX_COMPLEX_TYPES + 1; j++)
      P_P_Set[i][j] = new C_Reaction_Set(m_pApp);
}


/*************************************************************************
*
* METHOD NAME:	New_Reaction
*
* DESCRIPTION:	Creates a new reaction in the reaction matrix. The number
*		of products and substrates are first validated, then
*		a list of reactants which are created and destroyed in
*		reaction is made. A reaction is created between Substrate 1 and
*		Substrate 2 by calling C_Reaction_Set::New_Reaction. 
*		Then, if the reaction is bimolecular, another reaction is 
*		created between Substrate 2 and Substrate 1 (ie reverse
*		order).
*
* PARAMETERS:	long			 nNumSubstrates	- Number of substrates
*		long			 nNumProducts	- Number of products
*		C_Complex_Type**         P_P_Substrates - Pointer to array of
*							  substrate complex 
*                                                         types
*		C_Complex_Type**         P_P_Products	- Pointer to array of
*							  product complex types
*		double			 flPr		- Probability of
*                                                         reaction
*		long			 nID		- ID for this reaction
*		C_Reaction**	         P_P_Reaction1	- Pointer to reaction
*                                                         created.
*		C_Reaction**	         P_P_Reaction2	- Pointer to reaction
*                                                         created in
*							  bimolecular reaction.
*
* RETURNS:	C_Reaction**	 P_P_Reaction1	- Pointer to reaction created.
*		C_Reaction**	 P_P_Reaction2	- Pointer to reaction created
*						  in bimolecular reaction.
*		Bool		 TRUE			- Reactions created
*							  successfully.
*		Bool		 FALSE			- An error occurred
*                                                         creating the
*                                                         reactions.
*
*************************************************************************/

Bool
C_Reaction_Matrix::New_Reaction(long nNumSubstrates, long nNumProducts,
				C_Complex_Type** P_P_Substrates,
				C_Complex_Type** P_P_Products, double flPr,
				long nID, C_Reaction** P_P_Reaction1,
				C_Reaction** P_P_Reaction2)
{
  int  nRow;
  int  nCol;
  int  nError;
  char lpszReaction[MAX_MESSAGE_LENGTH];
  Bool bSubst1Conserved;
  Bool bSubst2Conserved;
  Bool bProductsConserved[MAX_PRODUCTS_IN_REACTION];
  int  nReactantNum;

  *P_P_Reaction1 = NULL;
  *P_P_Reaction2 = NULL;
  // Assume both substrates will not be conserved
  bSubst1Conserved = FALSE;
  bSubst2Conserved = FALSE;
  // Loop through each possible reactant
  for (nReactantNum = 0; nReactantNum < MAX_PRODUCTS_IN_REACTION;
       nReactantNum ++)
    // Assume reactant will not be conserved
    bProductsConserved[nReactantNum] = FALSE;

  // Validation

  // Validate number of substrates
  if (nNumSubstrates > 2)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 14);
      return FALSE;
    }
  // Validate number of products
  if (nNumProducts > MAX_PRODUCTS_IN_REACTION)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 15);
      return FALSE;
    }

  // Work out which substrates and products get consumed / created
  // in reaction, and which are conserved

  nReactantNum = 0;
  // Loop through each product until one matches first substrate or end of list
  while ((nReactantNum < nNumProducts)
	 && (P_P_Substrates[0] != P_P_Products[nReactantNum]))
    nReactantNum ++;
  // If a match was found before the end of the list was reached
  if (nReactantNum < nNumProducts)
    {
      // Indicate first substrate is conserved
      bSubst1Conserved = TRUE;
      // Indicate matching product is conserved
      bProductsConserved[nReactantNum] = TRUE;
    }
  // If there is a second substrate
  if (nNumSubstrates == 2)
    {
      nReactantNum = 0;
      // Loop through each product until one matches 2nd substrate
      // (which is not already conserved) or end of list
      while ((nReactantNum < nNumProducts)
	     && ((P_P_Substrates[1] != P_P_Products[nReactantNum])
		 || (bProductsConserved[nReactantNum])))
	nReactantNum ++;
      // If a match was found before the end of the list was reached
      if (nReactantNum < nNumProducts)
	{
	  // Indicate second substrate is conserved
	  bSubst2Conserved = TRUE;
	  // Indicate matching product is conserved
	  bProductsConserved[nReactantNum] = TRUE;
	}
    }

  // Find out which element in reaction matrix this reaction must be added to

  nRow = P_P_Substrates[0]->Get_Index() + 1;
  nCol = (nNumSubstrates == 1 ? 0 : (P_P_Substrates[1]->Get_Index() + 1));
	
  // Add reaction to reaction matrix

  nError = P_P_Set[nRow][nCol]->New_Reaction(nNumProducts, P_P_Products, flPr, nID,
					bSubst1Conserved, bSubst2Conserved,
					bProductsConserved, P_P_Reaction1);
  // Ensure no errors occurred
  if (nError != 0)
    {
      if (nError > 0)
	m_pApp->Message(MSG_TYPE_ERROR, 16,
		      Reaction_String(lpszReaction, nRow, nCol, nError - 1));
      return FALSE;
    }

  // If bimolecular reaction, create second entry 
  // (representing the substrates in reverse order)

  if (nNumSubstrates == 2)
    {
      // Calculate row for second entry
      nRow = P_P_Substrates[1]->Get_Index() + 1;
      // Calculate column for second entry
      nCol = P_P_Substrates[0]->Get_Index() + 1;
      nError = P_P_Set[nRow][nCol]->New_Reaction(nNumProducts, P_P_Products, flPr,
					    nID, bSubst2Conserved,
					    bSubst1Conserved,
					    bProductsConserved, P_P_Reaction2);
      // Ensure no errors occurred
      if (nError != 0)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 16,
			Reaction_String(lpszReaction, nRow, nCol, nError - 1));
	  return FALSE;
	}
    }

  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Dump
*
* DESCRIPTION:	Outputs all the reactions within the reaction matrix to
*		a file, in the form eg TT + WWAA = TTWWAA (Pr = 0.8000).
*		The method Reaction_String is called for each non-zero
*		element in the matrix, which converts the element into a
*		string.
*
* RETURNS:	Bool	TRUE	- Reaction matrix was saved successfully.
*			FALSE	- An error occurred while saving matrix.
*
*************************************************************************/

Bool
C_Reaction_Matrix::Dump(void)
{
  char		lpszRMFile[MAX_FILE_NAME_LENGTH];
  char		lpszRMPath[MAX_FILE_NAME_LENGTH];
  char		lpszReaction[MAX_MESSAGE_LENGTH];
  ofstream*	P_R_Matrix_Stream;
  int		nRow;
  int		nCol;
  int		nReactionNum;

  // Get file name to output reaction matrix to and open stream

  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_R_M_OUT, lpszRMFile))
    return FALSE;

  //Construct full path name for Reaction Matrix Output
  sprintf(lpszRMPath,"%s%s%s",m_pApp->m_lpszWorkingDir, FILE_PATH_SEPARATOR,
	      lpszRMFile);
	      
  // Create new stream
  P_R_Matrix_Stream = new ofstream(lpszRMPath);
  // Check validity of new stream
  if (P_R_Matrix_Stream == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 2, "Reaction Matrix");
      return FALSE;
    }

  // Scan through reaction matrix and output each reaction to stream

  for (nRow = 0; nRow <= m_nSize ; nRow ++)
    for (nCol = 0; nCol <= m_nSize; nCol ++)
      for (nReactionNum = 0;
	   nReactionNum < P_P_Set[nRow][nCol]->Get_Num_Reactions();
	   nReactionNum ++)
	*P_R_Matrix_Stream
	  << Reaction_String(lpszReaction, nRow, nCol, nReactionNum) << endl;

  P_R_Matrix_Stream->close();  // Close stream
  delete P_R_Matrix_Stream;

  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Reaction_String
*
* DESCRIPTION:	Converts a reaction, stored within a Set at a given element
*		within the reaction matrix, to a string. This function
*		creates a string containing the substrates, then calls
*		a method in the reaction set (C_Reaction_Set) to display 
*		the products. This latter method cannot be used to display
*		the substrates, because the reaction set object is not
*		'aware' of the substrates; only the reaction matrix is
*		aware of these substrates, in terms of the row and column
*		being used.
*
* PARAMETERS:	char* lpszReaction  - Pointer to empty string where
*				      reaction string will be stored.
*		long  nRow	    - Row containing reaction being output.
*		long  nCol	    - Column containing reaction being output.
*		int   nReactionNum  - Reaction number within Set of reaction
*				      being output.
*
* RETURNS:	char*		    - Pointer to string containing reaction
*				      string (ie lpszReaction)
*
*************************************************************************/

char*
C_Reaction_Matrix::Reaction_String(char* lpszReaction, long nRow, long nCol,
				   int nReactionNum)
{
  char lpszProducts[MAX_COMPONENTS_IN_COMPLEX
		   * MAX_COMPONENT_SYMBOL_LENGTH * MAX_PRODUCTS_IN_REACTION];
  long nID;
  // Get reaction ID
  nID = P_P_Set[nRow][nCol]->Get_ID(nReactionNum);
  // If reaction ID is a reverse reaction...
  if (nID > REVERSE_REACTION_ID_OFFSET)
    // Output copy corrected ID to string
    sprintf(lpszReaction, "[%ld - Reverse]  ",
	    nID - REVERSE_REACTION_ID_OFFSET);
  else  // Otherwise...
    // Output copy ID 
    sprintf(lpszReaction, "[%ld - Forward]  ", nID);
  // Copy first substrate complex type to string			
  strcat(lpszReaction, P_P_Complex_Type[nRow - 1]->Display_String());
  // If there is a second substrate...
  if (nCol > 0)
    {
      strcat(lpszReaction, " + ");
      // Copy second substrate complex type to string
      strcat(lpszReaction, P_P_Complex_Type[nCol - 1]->Display_String());
    }
  strcat(lpszReaction, " -> ");
  // Add products to string
  strcat(lpszReaction,
	 P_P_Set[nRow][nCol]->Products_String(lpszProducts, nReactionNum));
  return lpszReaction;
}


/*************************************************************************
*
* METHOD NAME:	Optimise
*
* DESCRIPTION:	Recalculates probabilities and the time increment in order
*		to maximise the value of the time increment, which will
*		speed up the simulation. The largest probability
*		of reaction is found within the matrix, and this is used
*		to calculate a scaling factor, such that the largest
*               sum of probabilities for any set of reactions is as close to
*		the maximum probability allowed (MAX_PR_FOR_SCALING) while
*		not exceeding it. The probabilities in the matrix are then
*		scaled accordingly, and the time increment is also 
*		scaled. Note that the probabilities can be scaled up
*		or down.
*
* PARAMETERS:	double*     p_flTimeInc	- Pointer to the time increment used
*					  in the simulation.
*		C_Complex** P_P_CT	- Pointer to array of complex types
*					  in C_System.
*
* RETURNS:	Bool	    FALSE	- An error occurred while optimising.
*			    TRUE	- Matrix was optimised successfully.
*
*************************************************************************/

Bool
C_Reaction_Matrix::Optimise(double* p_flTimeInc, C_Complex_Type** P_P_CT)
{
  double	flMaxPr;
  double	flScale;
  double	flNewTimeInc;
  double	flMaxRowCTPr;
  double	flMaxColCTPr;
  int		nRow;
  int		nCol;

  // Step through reactions and find the largest probability
  // Note that this procedure of choosing the largest Row and
  // Col probability may not work for all special CTs

  // *** (i.d. This routine assumes that special CTs always
  // *** affect reactions using a scaling factor between 0 and 1

  flMaxPr = 0;

  for (nRow = 0; nRow <= m_nSize ; nRow ++)
    for (nCol = 0; nCol <= m_nSize; nCol ++)
      // If there are reactions in this reaction set
      if (P_P_Set[nRow][nCol]->Get_Num_Reactions() > 0)
	{
	  flMaxRowCTPr = 0;
	  flMaxColCTPr = 0;
	  // If this is a real reactant, get total probability
	  // from 1st reactant (special CTs may modify probabilities)
	  if (nRow != 0)
	    flMaxRowCTPr
	      = (P_P_CT[nRow - 1]		 
		 -> Calc_Effective_Total_Pr(P_P_Set[nRow][nCol]->Get_Reaction_Array(),
					    P_P_Set[nRow][nCol]->Get_Num_Reactions()));
	  // If this is a real reactant, get total probability
	  // from 2nd reactant (special CTs may modify probabilities)
	  if (nCol != 0)
	   {
	    flMaxColCTPr
	      = (P_P_CT[nCol - 1]
		 -> Calc_Effective_Total_Pr(P_P_Set[nRow][nCol]->Get_Reaction_Array(),
					    P_P_Set[nRow][nCol]->Get_Num_Reactions()));
	   }
	  // If there are probabilities for both row and column CT
	  if ((nRow != 0) && (nCol != 0))
	    // Store current max probability, using smaller
	    // of row or col probabilities
	    // (The smaller is max because effective Pr is the product)
	    flMaxPr = MAX(flMaxPr, MIN(flMaxRowCTPr, flMaxColCTPr));
	  // Otherwise, if there is only 1 row or column CT,
	  // one will be 0, so add them to get the one which is non-zero
	  else	
	    flMaxPr = MAX(flMaxPr, flMaxRowCTPr + flMaxColCTPr);
	}

  // Calculate scaling factor for new time scale

  // If the maximum probability was zero (ie if there are no reactions)
  if (flMaxPr == 0)
    // Assume there is a non-zero max probability, so we don't try
    // re-scaling to infinity!
    flMaxPr = MAX_PR_REACTION_FOR_SCALING;
  // Calculate scaling factor based on maximum probability
  flScale = MAX_PR_REACTION_FOR_SCALING / flMaxPr;
  // Calculate the new time increment
  flNewTimeInc = *p_flTimeInc * flScale;
  // Recalculate scaling factor based on new time increment
  // (to ensure accuracy)
  flScale = flNewTimeInc / *p_flTimeInc;
  *p_flTimeInc = flNewTimeInc;

  // Scale reaction sets using calculated scale factor

  for (nRow = 0; nRow <= m_nSize ; nRow ++)
    for (nCol = 0; nCol <= m_nSize; nCol ++)
      P_P_Set[nRow][nCol]->Scale(flScale);

  // Output time increment being used
  ostringstream NewTimeIncStream;
  NewTimeIncStream << setw(PREC_TIME_INC_WIDTH);
  NewTimeIncStream << setprecision(PREC_TIME_INC) << scientific;
  NewTimeIncStream << flNewTimeInc << ends;
  m_pApp->Message(MSG_TYPE_STATUS, 71, NewTimeIncStream.str().c_str());

  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Validate
*
* DESCRIPTION:	Validates the reaction matrix. A method in the reaction set
*		(C_Reaction_Set::Validate) is called to validate the set.
*		In particular, the probabilities are checked to ensure
*		they are neither too large nor too small.
*
* RETURNS:	Bool	FALSE		- A validation error occurred.
*			TRUE		- Matrix was validated successfully.
*
*************************************************************************/

Bool
C_Reaction_Matrix::Validate(void)
{
  int nCol;
  int nRow;
  // Loop through reaction matrix...
  for (nRow = 0; nRow <= m_nSize ; nRow ++)
    for (nCol = 0; nCol <= m_nSize; nCol ++)
      // Validate each reaction set
      if (!P_P_Set[nRow][nCol]->Validate())
	return FALSE;
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	This method clears up the memory used by this object.
*
*************************************************************************/

C_Reaction_Matrix::~C_Reaction_Matrix(void)
{
  int i,j;
  for (i = MAX_COMPLEX_TYPES; i >= 0; i--)
    for (j = MAX_COMPLEX_TYPES; j >= 0; j--)
      delete P_P_Set[i][j];
}
