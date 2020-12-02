/*************************************************************************
*
* FILENAME:	RG_Dummy.cpp
*
* CLASS:	C_Random_Generator_Dummy
*
* DESCRIPTION:	This class is derived from the abstract class
*		C_Random_Generator. This is an implementation of a dummy
*		generator used for testing purposes only. Whenever a
*		random number is requested, a constant is returned.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"

#define RAN_GEN_NAME	"Dummy Random Number Generator"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	The random number generator name is defined.
*
*************************************************************************/

C_Random_Generator_Dummy::C_Random_Generator_Dummy(C_Application* p_App)
  : C_Random_Generator(p_App)
{
  m_lpszName = new char[strlen(RAN_GEN_NAME) + 1];
  strcpy(m_lpszName, RAN_GEN_NAME);
}
	

/*************************************************************************
*
* METHOD NAME:	Raw_Reseed
*
* DESCRIPTION:	Placeholder only (it is called by ancestor class).
*
*************************************************************************/

void
C_Random_Generator_Dummy::Raw_Reseed(void)
{
}


/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Returns a fixed number
*
* PARAMETERS:	long nRange - The range of the number to be generated.
*
* RETURNS:	long	    - [nRange / 2]
*
*************************************************************************/

long
C_Random_Generator_Dummy::New_Num (long nRange)
{
  return (nRange / 2);
}


/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Returns a fixed number
*
* RETURNS:	double	- 0.5
*
*************************************************************************/

double
C_Random_Generator_Dummy::New_Num (void)
{
  return 0.5;
}


/*************************************************************************
*
* METHOD NAME:	Repeat_Sequence
*
* DESCRIPTION:	Placeholder only.
*
* PARAMETERS:	long nNextNum	- Not used
*
* RETURNS:	Bool		- Always returns FALSE (indicating sequence
*				  sequence is not repeating.
*
*************************************************************************/

Bool
C_Random_Generator_Dummy::Repeat_Sequence(long)
{
  return FALSE;
}
