/*************************************************************************
*
* FILENAME:	OV_C_Av.cpp
*
* CLASS:	C_Output_Variable_Complex_Averaged
*
* DESCRIPTION:	This class is a descendant of the C_Output_Variable_Complex
*               class. This class should be used when an averaged output is
*               desired. The averaging is accomplished by storing a
*               cumulative value is updated when the method Update() is
*               called, and dividing by the number of samples when the
*               function Get_Value() is called.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Initialises the averaging output variable.  The name of the
*		variable and list of complex types this variables represents
*		is passed to the base class	constructor.
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
*
*************************************************************************/

C_Output_Variable_Complex_Averaged::C_Output_Variable_Complex_Averaged
(C_Application* p_App, char* lpszName, C_Complex_Type** P_CT_List,
 long nNumTypes)
  : C_Output_Variable_Complex (p_App, lpszName, P_CT_List, nNumTypes)
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
*		samples to obtain the average value.  Note that the
*		variables storing the cumulative value and the number of
*		sampled values must be reset by calling the Reset() method
*		after this method is called.
*
* RETURNS:	long                     - Current value of this variable
*
*************************************************************************/

long
C_Output_Variable_Complex_Averaged::Get_Value(void)
{
  long nValue;

  //First, update the value of this variable
  //  Update();

#ifdef _UNIX  // If we're building under UNIX-like OS's...
  // Use the BSD rint() function for rounding

  // Divide cumulative value by number of samples and round off to
  // nearest integer to obtain averaged value
  nValue = (long) rint (((double) m_nCumValue) / ((double) m_nNumSampled));
#else  // For other OS's (e.g. Microsoft does not provide an rint() function)
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
* DESCRIPTION:	Updates the current cumulative value for this variable.
*
*************************************************************************/

void
C_Output_Variable_Complex_Averaged::Update(void)
{
  long nCount;
  long nValue;
  nValue = 0;
  // For each type this variable represents
  for (nCount = 0; nCount < m_nNumTypes; nCount ++)
    // Add the level of the state to the running total
    nValue += P_Complex_Type[nCount] -> Get_Level();
  //***THIS CONDITIONAL SHOULD BE REMOVED (FOR EFFICIENCY)
  // IF OVERFLOW NEVER OCCURS!! 
  if (m_nCumValue + nValue >= MAX_LONG)
    {
      m_pApp->Message(MSG_TYPE_STATUS, 142, 1, m_lpszName);
    }
  m_nCumValue += nValue;
  m_nNumSampled ++;
}
