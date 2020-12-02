/*************************************************************************
*
* FILENAME:	OV_C.cpp
*
* CLASS:	C_Output_Variable_Complex
*
* DESCRIPTION:	This class is used to store the output variables used for 
*		display and storage purposes. Each output variable can
*		represent a single complex type or a number of complex
*		types (indicating the variable is equal to the sum of
*		the levels of the complex types). This should be inherited
*		for variable descendant objects by Special Complex Types.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Initialises the output variable. The name of the variable
*		and list of complex types this variables represents is set.
*
* PARAMETERS:   C_Application*  p_App  	        - Pointer to parent application
*
*        :	char*	         lpszName	- Name of this output variable.
*
*		C_Complex_Type** P_CT_List	- Pointer to array of
*						  pointers this variable
*						  represents.
*		long		 nNumTypes	- Number of complex types
*						  this variable represents.
*
*************************************************************************/

C_Output_Variable_Complex::C_Output_Variable_Complex
(C_Application* p_App, char* lpszName, C_Complex_Type** P_CT_List,
 long nNumTypes)
  : C_Output_Variable(p_App, lpszName)
{
  long nCount;
  for (nCount = 0; nCount < nNumTypes; nCount ++)
    P_Complex_Type[nCount] = P_CT_List[nCount];
  m_nNumTypes = nNumTypes;
}


/*************************************************************************
*
* METHOD NAME:	Get_Value
*
* DESCRIPTION:	Calculates and returns the current value of this variable.
*		Each complex type this variable represents is accessed
*		and its level added to the value of this variable.
*
* RETURNS:	long - Current value of this variable
*
*************************************************************************/

long
C_Output_Variable_Complex::Get_Value(void)
{
  long nCount;
  long nValue;
  nValue = 0;
  // For each type this variable represents
  for (nCount = 0; nCount < m_nNumTypes; nCount ++)
    // Add the level of the state to the running total
    nValue += P_Complex_Type[nCount] -> Get_Level();
  return nValue;
}
