/*************************************************************************
*
* FILENAME:	OV_A.cpp
*
* CLASS:	C_Output_Variable_Array
*
* DESCRIPTION:	This class is used to store the state of complex arrays.
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
*            	char*		 lpszName      - Name of this output variable.
*
*		C_Complex_Array* pArray	       - Pointer to the C_Complex_Array
*					         object this variable
*					         represents.
*             
*************************************************************************/

C_Output_Variable_Array::C_Output_Variable_Array(C_Application* p_App,
						 char* lpszName,
						 C_Complex_Array* pArray)
  : C_Output_Variable(p_App, lpszName)
{
  strcpy(m_lpszName,lpszName);
  m_pArray = pArray;
  m_pDimensions = new C_Integer_Pair;
  m_pArray->Get_Dimensions(m_pDimensions);
}

/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the complex array output variable object.
*		Memory is allocated for storing values
*
* PARAMETERS:   NONE
*
* RETURNS:	Bool	TRUE		- Object initialised successfully.
*			FALSE		- An error occurred while 
*					  initialising this object.
*
*************************************************************************/

Bool
C_Output_Variable_Array::Init(void)
{
  long         nXCount;
  long         nYCount;

  // Allocate memory for values to be stored

  P_P_nValue = new long* [m_pDimensions->nX];
  for (nXCount = 0; nXCount < m_pDimensions->nX; nXCount++)
    {
      // Allocate memory to store values for each row in complex array
      P_P_nValue[nXCount] = new long [m_pDimensions->nY];
      
      // If the array was not created correctly...
      if (P_P_nValue[nXCount] == NULL)
	{
	  // Output error
	  m_pApp->Message(MSG_TYPE_ERROR, 8, "Output Variable values");
	  return FALSE;
	}
    }
  
  // Reset variable values to zero
  for (nXCount = 0; nXCount < m_pDimensions->nX; nXCount++)
    for (nYCount = 0; nYCount < m_pDimensions->nY; nYCount++)
      P_P_nValue[nXCount][nYCount] = 0;

  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Update
*
* DESCRIPTION:	Updates the current values of this variable according to
*		the states of the complexes in the array.  
*
* PARAMETERS:	NONE
*
* RETURNS:	NONE
*
*************************************************************************/

void
C_Output_Variable_Array::Update(void)
{
  long			i;
  long                  j;

  // Loop through all of the coordinates in the array...
  for (i = 0; i < m_pDimensions->nX; i++)
    {
      for (j = 0; j < m_pDimensions->nY; j++)
	{
	  P_P_nValue[i][j]
	    = ((C_Complex_Multistate*)(m_pArray->Get_Complex(i, j)))
	       -> Get_State();
	}
    }
}

/*************************************************************************
*
* METHOD NAME:	Get_Dimensions
*
* DESCRIPTION:	returns dimensions of array
*
*************************************************************************/

void 
C_Output_Variable_Array::Get_Dimensions(C_Integer_Pair* pDimensions)
{
  m_pArray->Get_Dimensions(pDimensions);
}

/*************************************************************************
*
* METHOD NAME:	Set_FileName
*
* DESCRIPTION:	Sets the output file name for this array output variable.
*
*************************************************************************/

void 
C_Output_Variable_Array::Set_FileName(char* lpszFileName)
{
  strcpy(m_lpszFileName, m_pApp->m_lpszWorkingDir);
  strcat(m_lpszFileName, FILE_PATH_SEPARATOR);
  strcat(m_lpszFileName, lpszFileName);
}

/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Frees memory allocated in constructor
*
*************************************************************************/

C_Output_Variable_Array::~C_Output_Variable_Array(void)
{
  int  i;
  if (P_P_nValue != NULL)
    {
      for (i = m_pDimensions->nX - 1; i >= 0; i --)
	if (P_P_nValue[i] != NULL)
	  delete[] P_P_nValue[i];
      delete[] P_P_nValue;
    }
  delete	m_pDimensions;
}
