/*************************************************************************
*
* FILENAME:	_OutputMn.cpp
*
* CLASS:	C_Output_Manager
*
* DESCRIPTION:	This class organises the output of the system.  It stores
*               all output variables and executes output events at
*               appropriate times during the simulation.
*
*
* TYPE:		Core
*
*************************************************************************/


#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	This method initialises the output manager. All the
*		attributes are cleared.
*
* PARAMETERS:   C_Application*  p_App     - Pointer to parent application
*
*************************************************************************/

C_Output_Manager::C_Output_Manager(C_Application* p_App, C_System* pSystem)
{
  m_pApp = p_App;
  P_Parent = pSystem;
}

/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	This method clears up all the memory allocated for the
*		output manager
*
*************************************************************************/

C_Output_Manager::~C_Output_Manager(void)
{
}

