/*************************************************************************
*
* FILENAME:	OV.cpp
*
* CLASS:	C_Output_Variable
*
* DESCRIPTION:	Abstract class inherited by various output variable objects.
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
*		is set.
*
* PARAMETERS:   C_Application*  p_App  	    - Pointer to parent application.
*
*         :	char*	         lpszName   - Name of this output variable.
*
*************************************************************************/

C_Output_Variable::C_Output_Variable(C_Application* p_App, char* lpszName)
{
  m_pApp = p_App;
  strcpy(m_lpszName, lpszName);
}
