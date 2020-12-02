/*************************************************************************
*
* FILENAME:	CT_Spec.cpp
*
* CLASS:	C_Complex_Type_Special
*
* DESCRIPTION:	This class is the base class from which all special complex
*		types are inherited.
*
* TYPE:		Core
*
*************************************************************************/


#include "_Stchstc.hh"




/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Initialises the special complex type. Information is copied 
*		from the C_Complex_Type	the special complex type is replacing.
*
* PARAMETERS:
*             C_Application*  p_App  	        - Pointer to parent application
*             C_Complex_Type* P_Old_Complex_Type - Complex type this special
*					           type is based upon
*	      C_System*       P_System	         - Pointer to the system 
*						   containing the complex
*						   type (ie the parent system).
*
*************************************************************************/

C_Complex_Type_Special::C_Complex_Type_Special (C_Application* p_App, C_Complex_Type* P_Old_Complex_Type, C_System* P_System)
	: C_Complex_Type (p_App, P_Old_Complex_Type->Get_Composition(),
			  P_Old_Complex_Type->Get_Index(), P_System)
{
}
