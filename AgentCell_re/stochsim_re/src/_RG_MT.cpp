/*************************************************************************
*
* FILENAME:	RG_MT.cpp
*
* CLASS:	C_Random_Generator_MT
*
* DESCRIPTION:	This class is derived from the abstract class
*		C_Random_Generator to wrap an implementation of the 
*               "Mersenne Twiseter algorithm" described in 
*               (Matsumoto & Nishimura, 1998, ACM Trans Mod Comp Sim 8:3-30).
*               The implementation is by R.J. Wagner (rjwagner@writeme.com)
*               (for details, see the file MersenneTwister.h).
*               The algorithm provides a sequence with a period 2^(19937)-1
*               that is equidistributed in 623 dimensions.  These statistical
*               advantages come at a very low computational cost.  Benchmark
*               tests indicate that the speed of this algorithm is almost
*               equivalent to the Shuffle (Numerical Recipes ran1) generator.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"
#include "MersenneTwister.h"

// The following parameters are defined for calculating random number.
// These constants have been calculated specifically.

#define RAN_GEN_NAME	"Mersenne Twister Random Number Generator"

/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	The random number generator name is defined and shuffle
*				table initialised.
*
*************************************************************************/

C_Random_Generator_MT::C_Random_Generator_MT(C_Application * p_App)
 : C_Random_Generator(p_App)
{
  m_lpszName = new char[strlen(RAN_GEN_NAME) + 1];
  strcpy(m_lpszName, RAN_GEN_NAME);
  P_MTRand = new MTRand();
}
	

/*************************************************************************
*
* METHOD NAME:	Raw_Reseed
*
* DESCRIPTION:	The generator is reseeded by calculating the new register
*		value from the seed, and the shuffle table filled.
*
*************************************************************************/

void
C_Random_Generator_MT::Raw_Reseed(void)
{
  P_MTRand->seed(m_nSeed);
}
	
	
/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Generates a random number within a given range by
*		calling the appropriate method in the MTRand object.
*
* PARAMETERS:	long nRange - The range of the number to be generated.
*
* RETURNS:	long	    - A number between 0 and (nRange - 1).
*
*************************************************************************/

long
C_Random_Generator_MT::New_Num (long nRange)
{
  return P_MTRand->randInt(nRange-1);
}

	
/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Generates a random number between 0.0 and 1.0 by
*		calling the appropriate method in the MTRand object
*
* RETURNS:	double		- A number between 0 and 1
*
*************************************************************************/

double
C_Random_Generator_MT::New_Num (void)
{
  return P_MTRand->rand();
}


/*************************************************************************
*
* METHOD NAME:	Repeat_Sequence
*
*DESCRIPTION:	Indicates whether or not sequence could be repeated (in
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
C_Random_Generator_MT::Repeat_Sequence(long nNextNum)
{
  return (nNextNum == m_nSeed);
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Delete any allocated objects
*
*************************************************************************/

C_Random_Generator_MT::~C_Random_Generator_MT(void)
{              
  delete P_MTRand;
}
