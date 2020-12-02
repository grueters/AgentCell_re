/*************************************************************************
*
* FILENAME:	RG_Shuff.cpp
*
* CLASS:	C_Random_Generator_Shuff
*
* DESCRIPTION:	This class is derived from the abstract class
*		C_Random_Generator. This is an implementation of a 
*		random number generator using a shuffle table
*		(see Numerical Recipes P280). Random numbers
*		are generated using a simple algorithm, then
*		pushed into a table and other pulled out so that
*		any patterns between consecutive numbers are broken up.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"

// The following parameters are defined for calculating random number.
// These constants have been calculated specifically.

#define RAN_GEN_NAME	"Shuffle Random Number Generator"
#define IA 		16807
#define IM 		2147483647L
#define AM 		4.656612875246e-10F	// 1.0F / IM
#define IQ 		127773
#define IR 		2836
#define NTAB 		32
#define NDIV 		(1+(IM-1)/NTAB)
#define EPS 		1.2e-7
#define RNMX 		0.99999988		// 1.0 - EPS


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	The random number generator name is defined and shuffle
*				table initialised.
*
*************************************************************************/

C_Random_Generator_Shuffle::C_Random_Generator_Shuffle(C_Application* p_App)
 : C_Random_Generator(p_App)
{
  m_p_nP = new long[NTAB];
  m_lpszName = new char[strlen(RAN_GEN_NAME) + 1];
  strcpy(m_lpszName, RAN_GEN_NAME);
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
C_Random_Generator_Shuffle::Raw_Reseed(void)
{
  int 	j;
  long	k;
  // Copy seed into register
  m_nReg = m_nSeed;
  // For every value in the shuffle table...
  for (j = NTAB + 7; j >= 0; j --)
    {
      k = m_nReg / IQ;
      // Calculate new random number
      m_nReg = IA * (m_nReg - k * IQ) - IR * k;
      // Make correction if negative
      if (m_nReg < 0)
	m_nReg += IM;
      // Fill table
      if (j < NTAB)
	m_p_nP[j] = m_nReg;
    }
  // Set parameter (used later)
  m_nP = m_p_nP[0];
}
	
	
/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Generates a random number within a given range by
*		multiplying the register by calculating a new
*		random number and adding it to the shuffle table.
*		Another number from the shuffle table is extracted
*		and returned. 
*
* PARAMETERS:	long nRange - The range of the number to be generated.
*
* RETURNS:	long	    - A number between 0 and (nRange - 1).
*
*************************************************************************/

long
C_Random_Generator_Shuffle::New_Num (long nRange)
{
  int j;
  long k;
  long nNewNum;
  float flTemp;
  k = m_nReg / IQ;
  // Calculate new random number
  m_nReg = IA * (m_nReg - k * IQ) - IR * k;
  // Make correction if negative
  if(m_nReg < 0)
    m_nReg += IM;
  j = (int) (m_nP / NDIV);
  // Extract old random number
  m_nP = m_p_nP[j];
  // Insert new random number into shuffle table (replacing old random number)
  m_p_nP[j] = m_nReg;
  // If random number exceeds maximum allowed...
  if ((flTemp = AM * (float) m_nP) > RNMX)
    // Return maximum random number
    nNewNum = (long) (RNMX * nRange);
  else  // Otherwise...
    // Return old random number extracted above
    nNewNum = (long) (flTemp * nRange);

// If validation is required, call validation method.
#ifdef __RAN_NUM_GEN_VALIDATION
  // This is implemented as conditional compilation for	performance reasons.
  Validate(nNewNum, nRange);
#endif
  return nNewNum;
}

	
/*************************************************************************
*
* METHOD NAME:	New_Num
*
* DESCRIPTION:	Generates a random number between 0.0 and 1.0 by
*		multiplying the register by calculating a new
*		random number and adding it to the shuffle table.
*		Another number from the shuffle table is extracted
*		and returned. 
*
* RETURNS:	double		- A number between 0 and 1
*
*************************************************************************/

double
C_Random_Generator_Shuffle::New_Num (void)
{
  int   j;
  long  k;
  float flTemp;
  k = m_nReg / IQ;
  // Calculate new random number
  m_nReg = IA * (m_nReg - k * IQ) - IR * k;
  // Make correction if negative
  if(m_nReg < 0)
    m_nReg += IM;
  j = (int) (m_nP / NDIV);
  // Extract old random number
  m_nP = m_p_nP[j];
  // Insert new random number into shuffle table (replacing old random number)
  m_p_nP[j] = m_nReg;
  // If random number exceeds maximum allowed...
  if ((flTemp = AM * (float) m_nP) > RNMX)
    // Return maximum random number
    return RNMX;
  else  // Otherwise...
    // Return old random number extracted above
    return flTemp;
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
C_Random_Generator_Shuffle::Repeat_Sequence(long nNextNum)
{
  return (nNextNum == m_nSeed);
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Delete the shuffle table.
*
*************************************************************************/

C_Random_Generator_Shuffle::~C_Random_Generator_Shuffle(void)
{              
  delete[] m_p_nP;
}
