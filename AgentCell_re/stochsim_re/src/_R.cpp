/*************************************************************************
*
* FILENAME:	R.cpp
*
* CLASS:	C_Reaction
*
* DESCRIPTION:	This class represents a particular reaction within the
*		system (see also C_Reaction_Matrix and C_Reaction_Set).
*		Each reaction is not 'aware' of its substrates, only
*		its products (substrates are represented at a higher
*		level, ie the reaction matrix).
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"

/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Passes pointer to parent application object.
*
* PARAMETERS:	C_Application* p_App;
*
*************************************************************************/

C_Reaction::C_Reaction(C_Application* p_App)
{
  m_pApp=p_App;
}


/*************************************************************************
*
* METHOD NAME:	New_Reaction
*
* DESCRIPTION:	Sets the attributes for a new reaction based on the
*		parameters passed in.
*
* PARAMETERS:	long		 nNumProducts -	The number of products
*							in the reaction.
*		C_Complex_Type** P_P_Products -	Pointer to an array of pointers
*						to product complex types.
*		double		 flPr	      -	Probability of reaction.
*		long		 nID	      - Reaction ID
*
*		Bool		 bSubst1Conserved 
*					      - Flag indicating if substrate 1
*						is conserved in reaction.
*
*		Bool		 bSubst2Conserved 
*					      - Flag indicating if substrate 2
*						is conserved in reaction.
*		Bool*		 bProductsConserved
*					      - Pointer to array of flags
*						indicating if products
*						are conserved in reaction.
*
* RETURNS:	Bool	 TRUE		  - Reaction created successfully.
*			 FALSE		  -	Reaction creation failed.
*
*************************************************************************/

Bool 
C_Reaction::New_Reaction(long nNumProducts, C_Complex_Type** P_P_Products,
			 double flPr, long nID, Bool bSubst1Conserved,
			 Bool bSubst2Conserved, Bool* bProductsConserved)
{
  int nNewProduct;
  for (nNewProduct = 0; nNewProduct < nNumProducts; nNewProduct ++)	
    // Add products to reaction
    P_Products[nNewProduct] = P_P_Products[nNewProduct];
  // Null terminate product list
  P_Products[nNewProduct] = NULL;
  // Add probability to reaction
  p = flPr;
  m_nID = nID;  // Set the identifier for this reaction
  // Set the flag indicating if substrate one is conserved
  m_bSubstrate_1_Conserved = bSubst1Conserved;
  // Set the flag indicating if substrate two is conserved
  // l(only valid for bimolecular reactions)
  m_bSubstrate_2_Conserved = bSubst2Conserved;
  for (nNewProduct = 0; nNewProduct < nNumProducts; nNewProduct ++)	
    // Add flags indicating which products are conserved
    m_bProducts_Conserved[nNewProduct] = bProductsConserved[nNewProduct];
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Products_String
*
* DESCRIPTION:	Creates a string containing the product complex types
*		with appropriate symbols (eg "TTWW + AA (Pr = 0.20)").
*
* PARAMETERS:	char* lpszProducts - Pointer to an existing string
*
* RETURNS:	char*		   - Pointer to the string (ie lpszProducts)
*
*************************************************************************/

char*
C_Reaction::Products_String(char* lpszProducts)
{
  int  i;
  ostringstream PrStream;

  // Add first complex type to string
  strcpy(lpszProducts, P_Products[0]->Display_String());
  i = 1;
  // While the end of the product list is not reached...
  while (P_Products[i] != NULL)
    {
      strcat(lpszProducts, " + ");
      // Add subsequent complex types to string
      strcat(lpszProducts, P_Products[i]->Display_String());
      i ++;
    }
  strcat(lpszProducts, " (Pr = ");	

  // Convert floating point probability to string
  //  sprintf(lpszPr, PREC_PROBABILITY, p);
  PrStream << setw(PREC_PROBABILITY_WIDTH);
  PrStream << setprecision(PREC_PROBABILITY) << fixed;
  PrStream << p << ends;
  strcat(lpszProducts, PrStream.str().c_str());  // Add probability to string
  strcat(lpszProducts, ")");

  return lpszProducts;  // Return pointer to string
}


/*************************************************************************
*
* METHOD NAME:	Validate
*
* DESCRIPTION:	Ensures the probability is not too small for the resolution
*		of the random number generator and not too large (ie must
*		be significantly less than 1.0 for accuracy).
*
* PARAMETERS:	double flPr - Probability to validate.
*
* RETURNS:	Bool		- TRUE	Probability passed validation
*				  FALSE Probability failed validation
*
*************************************************************************/

Bool
C_Reaction::Validate(double flPr)
{
  long nAbortOnResErr;
  
  // Check probability is not greater than the maximum allowed
  // probability for the reaction 
  // (This checked must be carried out before the following one, because
  //  if flPr > 1, (flPr*RAN_NUM_RESOLUTION) can end up being less than
  //  MIN_PR_ACCURACY)
  if (flPr > MAX_PROBABILITY_REACTION)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 5);
      return FALSE;
    }
  // Check probability is below that required for the resolution
  // of the random number generator
  if ((flPr > 0) && ((long) (flPr * RAN_NUM_RESOLUTION) < MIN_PR_ACCURACY))
    {
      m_pApp->Message(MSG_TYPE_ERROR, 4);  // Output error message

      // Retrieve abort on resolution error flag from INI file
      if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_ABORT_ON_RES_ERR,
			 &nAbortOnResErr))
	return FALSE;
      
      // If the resolution error flag has been set...
      if (nAbortOnResErr)
	return FALSE; // ...abort!!
    }
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Validate_Dynamic_Values
*
* DESCRIPTION:	Validates a dynamic value during initialisation.
*
* PARAMETERS:	int	nNumValues  - Number of data items in array
*				      (Will be 1 for this dyn object)
*		double* p_flPr	    - Pointer to probabilities to validate.
*
* RETURNS:	Bool		    TRUE       - Probability passed validation
*				    FALSE      - Probability failed validation
*
*************************************************************************/

Bool 
C_Reaction::Validate_Dynamic_Values(int, double* p_flPr)
{
  return Validate(p_flPr[0]);
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Free any memory allocated by this object.
*
*************************************************************************/

C_Reaction::~C_Reaction(void)
{
}
