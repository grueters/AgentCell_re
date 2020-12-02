/*************************************************************************
*
* FILENAME:	OV_MS_Av.cpp
*
* CLASS:	C_Output_Variable_Multistate_Averaged
*
* DESCRIPTION:	This class is a descendant of the C_Output_Variable_Multistate
*		class.  This class should be used when an averaged output is 
*		desired. The averaging is accomplished by storing a cumulative
*		value is updated when the method Update() is called, and
*		dividing it by the number of samples when the function
*		Get_Value()	is called.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Initialises the output variable. All parameters are passed
*		to the base class constructor.
*
* PARAMETERS:   C_Application*  p_App  	        - Pointer to parent application
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

C_Output_Variable_Multistate_Averaged::C_Output_Variable_Multistate_Averaged
(C_Application* p_App, char* lpszName, C_Complex_Type** P_CT_List,
 long nNumTypes, long* p_nStates, long nNumStates)
  : C_Output_Variable_Multistate(p_App, lpszName, P_CT_List, nNumTypes, p_nStates, nNumStates)
{
  m_nCumValue = 0;
  m_nNumSampled = 0;
}


/*************************************************************************
*
* METHOD NAME:	Get_Value
*
* DESCRIPTION:	Calculates and returns the current value of this variable.
*		The stored cumulative value is divided by the number of
*		samples to obtain the average value.   Note that the
*		variables storing the cumulative value and the number of
*		sampled values must be reset by calling the Reset() method
*		after this method is called.
*
* RETURNS:	long - Current value of this variable
*
*************************************************************************/

long
C_Output_Variable_Multistate_Averaged::Get_Value(void)
{
  long nValue;
  
  // First, update the value of this variable
  // Update();

#ifdef _UNIX  // If we're building under UNIX-like OS's...
  // Use the BSD rint() function for rounding

  // Divide cumulative value by number of samples
  // to obtain averaged value
  nValue = (long) rint(((double) m_nCumValue)
		       / ((double) m_nNumSampled));
#else // For other OS's (e.g. Microsoft does not provide an rint() function)
  double flFracPortion;
  double flIntPortion;
  flFracPortion = modf((double) m_nCumValue / (double) m_nNumSampled,
		       &flIntPortion);
  nValue = (long) flIntPortion + (flFracPortion < 0.5 ? 0 : 1);
#endif
	
  return nValue;
}

/*************************************************************************
*
* METHOD NAME:	Update
*
* DESCRIPTION:	
*
* RETURNS:	long - Current value of this variable
*
*************************************************************************/

void
C_Output_Variable_Multistate_Averaged::Update(void)
{
  long						nCount;
  long						nValue;
  C_Complex_Type_Multistate*	P_CT_MS;
  nValue = 0;
  // Typecast pointer to complex type as pointer to multistate complex type
  P_CT_MS = (C_Complex_Type_Multistate*) P_Complex_Type[0];
  // For every state this variable represents
  for (nCount = 0; nCount < m_nNumStates; nCount ++)
    // Add the level of the state to the running total
    nValue += P_CT_MS -> Get_State_Level(m_pState[nCount]);
	
  if ((m_nCumValue + nValue) > MAX_LONG)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 142, 1, m_lpszName);
    }
	
  m_nCumValue += nValue;
  m_nNumSampled ++;
}
