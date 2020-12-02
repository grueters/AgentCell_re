/*************************************************************************
*
* FILENAME:	_Stch_clr.hh
*
* DESCRIPTION:	Class definitions for random number generator classes.
*
* TYPE:		Core
*
*************************************************************************/


#ifndef __STCH_CLR_HH
#define __STCH_CLR_HH


// Abstract class to be inherited as a base class
// by all random number generators

class C_Random_Generator
{
  // Attributes
private:	
  
  // Pointer to output stream, where seeds are stored
  ofstream*		P_Output_Seeds;

  // Pointer to input stream, where seeds can be retrieved from
  ifstream*		P_Input_Seeds;

protected:
  // Pointer to parent application object
  C_Application* m_pApp;
  

  // Initial seed used in the random number generation
  long			m_nSeed;

  // Name of the specific random number generator
  char*			m_lpszName;

  // Methods
private:
  
  // Reseeds the number generator (using the clock or a file of seeds)
  void
  Reseed(void);
  
  // Reseeds the generator algorithm (must be overriden in descendant)
  virtual void
  Raw_Reseed(void) = 0;

  // Detects whether the random number sequence is going to repeat
  // (in which case, reseeding is necessary)
  virtual Bool
  Repeat_Sequence(long) = 0;
  
public:

  // Constructor (passes application pointer from parent
  C_Random_Generator(C_Application*);

  // Initialises the random generator
  Bool
  Init(void);

  // Returns name of the random number generator
  char*
  Identify(void);

  // Tests generator and output errors / statistics to ensure it is
  // truly random (only used when debugging)
  Bool
  Test(long, long, int);

  // Returns a random number from a specific generation algorithm
  // (must be overriden)
  virtual double
  New_Num(void) = 0;
  
  // Returns a random number from a specific generation algorithm
  // (must be overriden) in a specified range
  virtual long
  New_Num(long) = 0;
  
  // Performs validation of newly generated random numbers
  long
  Validate(long, long);
  
  // Destructor to clear object and close files
  virtual
  ~C_Random_Generator(void);
};


// Random number generator using shuffle table
// (from Numerical Recipes - P280 ran1)

class C_Random_Generator_Shuffle : public C_Random_Generator
{
  // Attributes
private:	

  // Parameter used in generator
  long			m_nReg;

  // Parameter used in generator
  long			m_nP;

  // Parameter used in generator (shuffle table)
  long*			m_p_nP;

  // Methods
private:

  // Reseeds the generator algorithm
  void
  Raw_Reseed(void);
  
  // Detects whether the random number sequence is going to repeat
  // (in which case, reseeding is necessary)
  Bool
  Repeat_Sequence(long);
  
public:
  
  // Returns a random number between 0 and 1
  double
  New_Num(void);

  // Returns a random number in a specific range
  long
  New_Num(long);
  
  // Constructor to initialise parameters
  C_Random_Generator_Shuffle(C_Application*);

  // Destructor to clear up parameters
  ~C_Random_Generator_Shuffle(void);
};


// Long-period random number generator using shuffle table
// (from Numerical Recipes - P282 ran2)

class C_Random_Generator_Shuffle2 : public C_Random_Generator
{
  // Attributes
private:	

  // Parameter used in generator
  long			m_nReg;

  // Parameter used in generator
  long			m_nReg2;

  // Parameter used in generator
  long			m_nP;

  // Parameter used in generator (shuffle table)
  long*			m_p_nP;

  // Methods
private:

  // Reseeds the generator algorithm
  void
  Raw_Reseed(void);
  
  // Detects whether the random number sequence is going to repeat
  // (in which case, reseeding is necessary)
  Bool
  Repeat_Sequence(long);
  
public:
  
  // Returns a random number between 0 and 1
  double
  New_Num(void);

  // Returns a random number in a specific range
  long
  New_Num(long);
  
  // Constructor to initialise parameters
  C_Random_Generator_Shuffle2(C_Application*);

  // Destructor to clear up parameters
  ~C_Random_Generator_Shuffle2(void);
};


// Random number generator using bit string
// (from Numerical Recipes - P289 irbit1)

class C_Random_Generator_Bit_String : public C_Random_Generator
{
  // Attributes
private:

  // Parameter used in generator to store a bit string
  // from which other bits are generated
  long			m_nReg;

  // Methods
private:

  // Reseeds the generator algorithm
  void
  Raw_Reseed(void);

  // Generates a random bit using the bit string in the 'reg' attribute
  int
  Random_Bit(void);

  // Detects whether the random number sequence is going to repeat
  // (in which case, reseeding is necessary)
  Bool
  Repeat_Sequence(long);

public:

  // Constructor to initialise parameters
  C_Random_Generator_Bit_String(C_Application*);
  
  // Returns a random number between 0 and 1
  double
  New_Num(void);

  // Returns a random number in a specific range
  long
  New_Num(long);
};
	
	


// Random number generator using internal random number generator

class C_Random_Generator_Internal : public C_Random_Generator
{
  // Methods
private:
  
  // Reseeds the generator algorithm
  void 
  Raw_Reseed(void);
  
  // Detects whether the random number sequence is going to repeat
  // (in which case, reseeding is necessary)
  Bool
  Repeat_Sequence(long);

public:
  
  // Constructor to initialise parameters
  C_Random_Generator_Internal(C_Application*);
  
  // Returns a random number between 0 and 1
  double
  New_Num(void);

  // Returns a random number in a specific range
  long
  New_Num(long);
};
	

// Random number generator based on 'Quick' method (from Numerical Recipes)

class C_Random_Generator_Quick : public C_Random_Generator
{
  // Attributes
private:

  // Parameter used in generator to store cumulative remainder
  long			m_nReg;
	
  // Methods
private:
  
  // Reseeds the generator algorithm
  void
  Raw_Reseed(void);

  // Detects whether the random number sequence is going to repeat
  // (in which case, reseeding is necessary)
  Bool
  Repeat_Sequence(long);
  
public:

  // Constructor to initialise parameters
  C_Random_Generator_Quick(C_Application*);
  
  // Returns a random number between 0 and 1
  double
  New_Num(void);
  
  // Returns a random number in a specific range
  long
  New_Num(long);
};

	
// Dummy random number generator (returns {range / 2} all the time)

class C_Random_Generator_Dummy : public C_Random_Generator
{
  // Methods
private:
  
  // Reseeds the generator algorithm
  void
  Raw_Reseed(void);

  // Detects whether the random number sequence is going to repeat
  // (in which case, reseeding is necessary)
  Bool
  Repeat_Sequence(long);
  
public:
  
  // Constructor to initialise parameters
  C_Random_Generator_Dummy(C_Application*);
  
  // Returns a random number between 0 and 1
  double
  New_Num(void);

  // Returns a random number in a specific range
  long
  New_Num(long);
};

// Random number generator using the "Mersenne Twister" algorithm
// (Matsumoto & Nishimura, 1998, ACM Trans Mod Comp Sim 8:3-30)
// implementation by R.J. Wagner (rjwagner@writeme.com), May 2003
// (for details, see the file MersenneTwister.h)

#include "MersenneTwister.h"

class C_Random_Generator_MT : public C_Random_Generator
{
  // Attributes
private:	
  
  // Pointer to Mersenne Twister generator object
  MTRand* P_MTRand;

  // Methods
private:

  // Reseeds the generator algorithm
  void
  Raw_Reseed(void);
  
  // Detects whether the random number sequence is going to repeat
  // (in which case, reseeding is necessary)
  Bool
  Repeat_Sequence(long nNextNum);

public:
  
  // Returns a random number between 0 and 1
  double
  New_Num(void);

  // Returns a random number in a specific range
  long
  New_Num(long nRange);
  
  // Constructor to initialise parameters
  C_Random_Generator_MT(C_Application*);

  // Destructor to clear up parameters
  ~C_Random_Generator_MT(void);
};


#endif /* __STCH_CLR_HH */
