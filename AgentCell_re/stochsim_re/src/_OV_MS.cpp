/*************************************************************************
*
* FILENAME:	OV_MS.cpp
*
* CLASS:	C_Output_Variable_Multistate
*
* DESCRIPTION:	This class is used to store the output variables used for 
*		display and storage purposes with multistate complex
*		types. In addition to the information stored in the
*		base class, C_Output_Variable, this object can
*		represent a single state of a multistate complex type
*		or a number of states (indicating the variable is equal to 
*		the sum of the levels of the different states).
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
*		and list of complex types this variable represents is set
*		by the base class. The list of states this variables
*		represents is set.
*
* PARAMETERS:	C_Application*	 p_App  	- Pointer to parent application
*
*               char*		 lpszName	- Name of this output variable.
*
*		C_Complex_Type** P_CT_List	- Pointer to array of
*						  pointers this variable
*						  represents.
*		long		 nNumTypes	- Number of complex types
*						  this variable represents.
*		long*		 n_pStates	- Pointer to array of states
*						  this variable represents.
*		long		 nNumStates	- Number of states this
*						  variable represents.
*
*************************************************************************/

C_Output_Variable_Multistate::C_Output_Variable_Multistate
(C_Application* p_App, char* lpszName, C_Complex_Type** P_CT_List,
 long nNumTypes, long* p_nStates, long nNumStates)
  : C_Output_Variable_Complex(p_App, lpszName, P_CT_List, nNumTypes)
{
  long nCount;
  // Create array to hold state list in
  m_pState = new long[nNumStates];
  for (nCount = 0; nCount < nNumStates; nCount ++)
    m_pState[nCount] = p_nStates[nCount];
  m_nNumStates = nNumStates;
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
C_Output_Variable_Multistate::Get_Value(void)
{
  long				nCount;
  long				nValue;
  C_Complex_Type_Multistate*	P_CT_MS;

  nValue = 0;
  // Typecast pointer to complex type as pointer to multistate complex type
  P_CT_MS = (C_Complex_Type_Multistate*) P_Complex_Type[0];
  // For every state this variable represents
  for (nCount = 0; nCount < m_nNumStates; nCount ++)
    // Add the level of the state to the running total
    nValue += P_CT_MS -> Get_State_Level(m_pState[nCount]);
  return nValue;
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Cleans up memory allocated in the constructor.
*
*************************************************************************/

C_Output_Variable_Multistate::~C_Output_Variable_Multistate(void)
{
  if (m_pState != NULL)
    // Delete array used to hold the states to store
    delete[] m_pState;
}
