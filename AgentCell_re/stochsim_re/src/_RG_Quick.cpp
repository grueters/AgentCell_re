/*************************************************************************
*
* FILENAME:	RG_Quick.cpp
*
* CLASS:	C_Random_Generator_Quick
*
* DESCRIPTION:	This class is derived from the abstract class
*		C_Random_Generator. This is an implementation of a quick
*		random number generator. To generate a new random number,
*		a register is multiplied by a large constant and another
*		large constant is added. Because of overflow, this generates
*		a (relatively) random number. A special technique is used
*		to convert the integer to a float (so it can be scaled)
*		and back to an integer. This technique is highly
*		platform dependent!
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"

#define RAN_GEN_NAME	"Quick Random Number Generator"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	The random number generator name is defined.
*
*************************************************************************/

C_Random_Generator_Quick::C_Random_Generator_Quick(C_Application* p_App)
 : C_Random_Generator(p_App)
{
  m_lpszName = new char[strlen(RAN_GEN_NAME) + 1];
  strcpy(m_lpszName, RAN_GEN_NAME);
}
	

/*************************************************************************
*
* METHOD NAME:	Raw_Reseed
*
* DESCRIPTION:	The generator is reseeded by copying the seed into the
*		register.
*
*************************************************************************/

void
C_Random_Generator_Quick::Raw_Reseed(void)
{
  m_nReg = m_nSeed;
}
	
	
/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Generates a random number within a given range by
*		multiplying the register by a large constant and
*		adding another large constant. Note that these
*		constants are specific and have been calculated
*		(see Numerical Recipes). A specially formatted
*		bit string is constructed from this which resembles
*		the formatting of a floating point number (ie exponent
*		and mantissa) - note that this format is highly platform
*		dependent! The random number is scaled by converting
*		the variable type to a float (not the same as a normal
*		conversion!), multiplying by the range and converting
*		back to a float.
*
* PARAMETERS:	long nRange     - The range of the number to be generated.
*
* RETURNS:	long		- A number between 0 and (nRange - 1).
*
*************************************************************************/

long
C_Random_Generator_Quick::New_Num (long nRange)
{
  long nRanLong;
  long nNewNum;
  // Calculate new register
  m_nReg = 1664525 * m_nReg + 1013904223;
  // Create specially formatted (bit string) number
  nRanLong = 0x3F800000 | (0x007FFFFF & m_nReg);
  // Scale random number
  nNewNum = (long) ((*(float *)&nRanLong - 1.0) * nRange);

// If validation is required, call validation method.
#ifdef __RAN_NUM_GEN_VALIDATION
  // This is implemented as conditional compilation for performance reasons
  Validate(nNewNum, nRange);
#endif
  return nNewNum;  // Scale and return random number
}


/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Generates a random number between 0 and 1 by
*		multiplying the register by a large constant and
*		adding another large constant. Note that these
*		constants are specific and have been calculated
*		(see Numerical Recipes). A specially formatted
*		bit string is constructed from this which resembles
*		the formatting of a floating point number (ie exponent
*		and mantissa) - note that this format is highly platform
*		dependent! The random number is scaled by converting
*		the variable type to a float (not the same as a normal
*		conversion!).
*
* RETURNS:	double		- A number between 0 and 1
*
*************************************************************************/

double
C_Random_Generator_Quick::New_Num (void)
{
  long nRanLong;
  // Calculate new register
  m_nReg = 1664525 * m_nReg + 1013904223;
  // Create specially formatted (bit string) number
  nRanLong = 0x3F800000 | (0x007FFFFF & m_nReg);
  // Return random number
  return (*(float *)&nRanLong - 1.0);
}


/*************************************************************************
*
* METHOD NAME:	Repeat_Sequence
*
* DESCRIPTION:	Indicates whether or not sequence could be repeated (in
*		which case generator requires reseeding). The sequence
*		repeats if the register is equal to the original seed.
*
* PARAMETERS:	long nNextNum	-	Not used
*
* RETURNS:	Bool		-	TRUE	Sequence has repeated
*					FALSE	Sequence has not repeated
*
*************************************************************************/

Bool
C_Random_Generator_Quick::Repeat_Sequence(long)
{
  return (m_nSeed ==  m_nReg);
}
