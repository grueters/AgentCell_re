/*************************************************************************
*
* FILENAME:	RG_Int.cpp
*
* CLASS:	C_Random_Generator_Internal
*
* DESCRIPTION:	This class is derived from the abstract class
*		C_Random_Generator. This is an implementation of a internal
*		random number generator. The in-built C functions are used
*		to seed and generate random numbers. Note that this generator
*		is platform dependent.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"

#define RAN_GEN_NAME	"Internal Random Number Generator"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	The random number generator name is defined.
*
*************************************************************************/

C_Random_Generator_Internal::C_Random_Generator_Internal(C_Application* p_App)
  : C_Random_Generator(p_App)
{
  m_lpszName = new char[strlen(RAN_GEN_NAME) + 1];
  strcpy(m_lpszName, RAN_GEN_NAME);
}
	

/*************************************************************************
*
* METHOD NAME:	Raw_Reseed
*
* DESCRIPTION:	The generator is reseeded by calling the C function to
*		reseed the random number generator.
*
*************************************************************************/

void
C_Random_Generator_Internal::Raw_Reseed(void)
{
  srand((int) m_nSeed);
}

	
/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Generates a random number within a given range by calling
*		the C function for a new random (floating point) number
*		and scaling it appropriately. The random number must be
*		constructed from two random numbers because an integer
*		of type LONG is required, and the internal random number
*		generator only generates numbers of half this precision
*		(ie int).
*
* PARAMETERS:	long nRange     - The range of the number to be generated.
*
* RETURNS:	long		- A number between 0 and (nRange - 1).
*
*************************************************************************/

long
C_Random_Generator_Internal::New_Num (long nRange)
{
  long	nLongRanNum;
  float	flRangeRanNum;
  // Calculate random number as integer between 0 & RAND_MAX 
  // (in-built constant) using 2 random numbers
  nLongRanNum = (long) (rand() * (RAND_MAX + 1) ) + rand();
  flRangeRanNum = ((((float) nLongRanNum) / ((RAND_MAX + 1) * (RAND_MAX + 1)))
		   * nRange);

// If validation is required, call validation method.
#ifdef __RAN_NUM_GEN_VALIDATION
  // This is implemented as conditional compilation for performance reasons
  Validate((long) flRangeRanNum, nRange);
#endif
  // Convert integer number to floating point and scale to nRange.
  return ((long) flRangeRanNum);
}


/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Generates a random number between 0 and 1 by calling
*		the C function for a new random (floating point) number
*		and scaling it appropriately. The random number must be
*		constructed from two random numbers because an integer
*		of type LONG is required, and the internal random number
*		generator only generates numbers of half this precision
*		(ie int).
*
* RETURNS:	double		- A number between 0 and 1
*
*************************************************************************/

double
C_Random_Generator_Internal::New_Num (void)
{
  long	nLongRanNum;
  double	flRangeRanNum;
  // Calculate random number as integer between 0 & RAND_MAX
  // (in-built constant) using 2 random numbers
  nLongRanNum = (long) (rand() * (RAND_MAX + 1) ) + rand();
  flRangeRanNum = ((double) nLongRanNum) / ((RAND_MAX + 1) * (RAND_MAX + 1));
  // Convert integer number to floating point
  return ((double) flRangeRanNum);
}


/*************************************************************************
*
* METHOD NAME:	Repeat_Sequence
*
* DESCRIPTION:	Indicates whether or not sequence could be repeated (in
*		which case generator requires reseeding). The sequence
*		repeats if the number which has just been generated
*		equals the original seed used.
*
* PARAMETERS:	long nNextNum	- The last random number to be generated
*
* RETURNS:	Bool		- TRUE	Sequence has repeated
*				  FALSE	Sequence has not repeated
*
*************************************************************************/
                
Bool
C_Random_Generator_Internal::Repeat_Sequence(long nNextNum)
{
  return ((int) m_nSeed == (int) nNextNum);
}
