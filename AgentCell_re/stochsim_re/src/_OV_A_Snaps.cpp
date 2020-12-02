/*************************************************************************
*
* FILENAME:	OV_A_Snaps.cpp
*
* CLASS:	C_Output_Variable_Array_Snaps
*
* DESCRIPTION:	This class is used to store snapshots of the state of
*               complex arrays.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Initialises the complex array output variable. The states
*		that this variable represents are set (using wildcard mask
*               and value). 
*
* PARAMETERS:	C_Application*  p_App          - Pointer to parent application
*
*         	char*		 lpszName      - Name of this output variable.
*
*		C_Complex_Array* pArray	       - Pointer to the C_Complex_Array
*					         object this variable
*					         represents.
*		long	       nWildcardMask   - Wildcard mask for states to
*                                                highlight 
*               long           nWildcardVallue - Wildcard value for states to
*                                                highlight
*               long      nNumWatchCoordinates - Number of coordinates to be
*                                                watched
*               C_Integer_Pair*  P_WC          - List of watch coordinate
*                                                values
*             
*************************************************************************/

C_Output_Variable_Array_Snaps::C_Output_Variable_Array_Snaps
(C_Application* p_App, char* lpszName, char* lpszDisplayString,
 C_Complex_Array* pArray, long* p_nWildcardMask, long* p_nWildcardValue,
 long nNumWildcards)
  : C_Output_Variable_Array(p_App, lpszName, pArray)
{
  int		i;

  m_nNumWildcards = nNumWildcards;

  for (i = 0; i < m_nNumWildcards; i++)
    {
      m_pnWildcardMask[i] = p_nWildcardMask[i];
      m_pnWildcardValue[i] = p_nWildcardValue[i];
    }

  // Allocate memory for and nitialise display string
  m_lpszDisplayString = new char [strlen(lpszDisplayString) + 1];
  strcpy(m_lpszDisplayString, lpszDisplayString);
}

/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the snapshot variable
*
* PARAMETERS:	Bool	bAveraging	- Flag indicating whether or not
*                                         output for this variable is being
*                                         averaged.
*
* RETURNS:	TRUE                    - Initialisation was successful
*               FALSE                   - Initialisation failed
*
*************************************************************************/

Bool
C_Output_Variable_Array_Snaps::Init(void)
{
  // Call initialisation method of base class
  if (!C_Output_Variable_Array::Init())
    {
      m_pApp->Message(MSG_TYPE_ERROR, 161, 1, m_lpszName);
      return FALSE;
    }
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Update
*
* DESCRIPTION:	Updates the current values of this variable according to
*		the states of the complexes in the array.  Each value is 
*		incremented if the corresponding complex's state matches the
*               criteria for this variable.
*
* PARAMETERS:	long**	P_P_nValues	-  Pointer to memory location to
*					   which values are stored.
*
* RETURNS:		none.
*
*************************************************************************/

void
C_Output_Variable_Array_Snaps::Update(void)
{
  long	i;
  long  j;
  long  k;

  // Loop through all of the coordinates in the array...
  for (i = 0; i < m_pDimensions->nX; i++)
    {
      for (j = 0; j < m_pDimensions->nY; j++)
	{
	  for (k = 0; k < m_nNumWildcards; k++)
	    {
	      // If the state of the complex at this address
	      // matches one of the highlighted states of this variable...
	      if ((((C_Complex_Multistate*)(m_pArray->Get_Complex(i,j)))
		   -> Get_State() & m_pnWildcardMask[k])
		  == m_pnWildcardValue[k])
		{
		  // Increment the value of this coordinate
		  P_P_nValue[i][j]++;
		}
	    }
	}
    }
  m_nNumSampled ++;
}

/*************************************************************************
*
* METHOD NAME:	Get_Values
*
* DESCRIPTION:	Gets the current state values of all complexes in the
*               array that this variable represents.
*
* PARAMETERS:	NONE
*
* RETURNS:	long**	P_P_nValues	-  Pointer to memory location at
*					   which values are stored.
*
*************************************************************************/
/*
long**
C_Output_Variable_Array_Snaps::Get_Values(void)
{
  long			i;
  long                  j;

  // Loop through all of the coordinates in the array...
  for (i = 0; i < m_pDimensions->nX; i++)
    {
      for (j = 0; j < m_pDimensions->nY; j++)
	{
	  // Convert to averaged value
	  P_P_nValues[i][j]
	    = (long)((double) P_P_nValues[i][j] / m_nNumSampled);
	}
    }
  return P_P_nValues;
}
*/

/*************************************************************************
*
* METHOD NAME:	Reset
*
* DESCRIPTION:	Resets all values of the variable to zero.
*
* PARAMETERS:	NONE
*
* RETURNS:	NONE
*
*************************************************************************/

void
C_Output_Variable_Array_Snaps::Reset(void)
{
  long			i;
  long                  j;

  // Loop through all of the coordinates in the array...
  for (i = 0; i < m_pDimensions->nX; i++)
    {
      for (j = 0; j < m_pDimensions->nY; j++)
	{
	  // Reset to zero
	  P_P_nValue[i][j] = 0;
	}
    }
  m_nNumSampled = 0;
}

/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Frees memory allocated in constructor
*
*************************************************************************/

C_Output_Variable_Array_Snaps::~C_Output_Variable_Array_Snaps(void)
{
  delete[]      m_lpszDisplayString;
}
