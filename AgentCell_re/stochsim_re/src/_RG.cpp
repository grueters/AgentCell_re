/*************************************************************************
*
* FILENAME:	RG.cpp
*
* CLASS:	C_Random_Generator
*
* DESCRIPTION:	This abstract class is used as an ancestor for all random
*		number generator objects. It contains complete functionality
*		to retrieve and store seeds. When a random number generator
*		is required elsewhere in the program, a method in this
*		ancestor class is called, which passes the call onto the
*		specific descendent random number generator. The ancestor
*		method also calls a descendent method to see when reseeding
*		is necessary, and reseeds appropriately. This class also
*		contains a method to test the random number generator being
*		used.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"

/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	This method initialises the event manager. All the
*		attributes are cleared.
*
* PARAMETERS:   C_Application*  p_App  - Pointer to parent application
*
*************************************************************************/

C_Random_Generator::C_Random_Generator(C_Application* p_App)
{
  m_pApp = p_App;
}

/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	If a filename containing seeds has been defined, it is
*		opened with appropriate error handling. If a filename
*		for storing seeds has been defined, it is opened with
*		appropriate error handling. Initialises random number 
*		generator by reseeding it. Note that Reseed cannot be 
*		called in the constructor because it calls a method 
*		in the descendent which will not have been created by
*		the time the random number generator ancestor object
*		is constructed.
*
*************************************************************************/

Bool
C_Random_Generator::Init(void)
{
  char lpszInputSeedFile[MAX_FILE_NAME_LENGTH];
  char lpszInputSeedPath[MAX_FILE_NAME_LENGTH];
  char lpszOutputSeedFile[MAX_FILE_NAME_LENGTH];
  char lpszOutputSeedPath[MAX_FILE_NAME_LENGTH];
  P_Input_Seeds = NULL;
  P_Output_Seeds = NULL;
	
  // Attempt to get input and output file names
  if ((!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_INPUT_SEED_FILE,
		      lpszInputSeedFile))
      || (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_OUTPUT_SEED_FILE,
			 lpszOutputSeedFile)))
    return FALSE;

//  //  AgentCell / REPAST modification of file names
//  char runIDString[128], *cPos;
//  int i;
//  for (i = 0; i < MAX_NUM_RUN_IDS; i++) runIDString[i] = '0';
//  if (strcmp(cPos=strrchr(lpszOutputSeedFile,'.'),".OUT")==0){*cPos='\0';}
//  if (m_pApp->m_nRunID > 0)
//    {sprintf(runIDString, "%d", m_pApp->m_nRunID);}
//  else
//    {runIDString[0]='\0';}
//  strcat(lpszOutputSeedFile, runIDString);
//  strcat(lpszOutputSeedFile, ".OUT");

  // If an input filename for seeds has been defined...
  if (strcmp(lpszInputSeedFile, "") != 0)
    {
      // Construct full path name for seed input 
      sprintf(lpszInputSeedPath,"%s%s%s",m_pApp->m_lpszWorkingDir,
	      FILE_PATH_SEPARATOR, lpszInputSeedFile);
      // Open input stream for seeds (only if it exists)
      P_Input_Seeds = new ifstream(lpszInputSeedPath, ios::in);

#if ( defined(_ALPHA) || defined(_AIX) )
      // Ensure stream was opened successfully
      if (!(P_Input_Seeds->rdbuf()->is_open()))
#else
	// Ensure stream was opened successfully
	if (!(P_Input_Seeds->is_open()))
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
	  {
	    m_pApp->Message(MSG_TYPE_ERROR, 36);
	    return FALSE;
	  }
    }
  // If an output filename for seeds has been defined...
  if (strcmp(lpszOutputSeedFile, "") != 0)
    {
      // Open output stream for seeds
      strcpy(lpszOutputSeedPath, m_pApp->m_lpszWorkingDir);
      strcat(lpszOutputSeedPath, FILE_PATH_SEPARATOR);
      strcat(lpszOutputSeedPath, lpszOutputSeedFile);
      P_Output_Seeds = new ofstream(lpszOutputSeedPath);
      if (P_Output_Seeds == NULL)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 37);
	  return FALSE;
	}
    }
  // Reseed random number generator
  Reseed();
  return TRUE;
}
		



/*************************************************************************
*
* METHOD NAME:	Validate
*
* DESCRIPTION:	This method ensures the random number sequence is not
*		repeating and the number is in a valid range. If it
*		isn't, the generator is reseeded and a new number
*		generated.
*
* PARAMETERS:	long nRanNum - Random number 
*		long nRange  - Maximum number the random number can be
*
* RETURNS:	long	     - A random number between 0 and nRange - 1
*
*************************************************************************/

long
C_Random_Generator::Validate(long nRanNum, long nRange)
{
  // If there is a range error or sequence repeat...
  if  ((Repeat_Sequence(nRanNum))  || (nRanNum >= nRange))
    {
      // Reseed generator
      Reseed();
      // And call the generator to get a new number
      nRanNum = New_Num(nRange);
    }
  return nRanNum;
}

	
/*************************************************************************
*
* METHOD NAME:	Reseed
*
* DESCRIPTION:	Reseeds the random number generator. If there is a valid
*		input stream for seeds, a number is extracted from there;
*		otherwise, the internal clock is used to generate a new
*		seed. If the output stream for seeds is open, the seed
*		is stored there.
*
*************************************************************************/

void
C_Random_Generator::Reseed(void)
{
  // If seed input stream is not open...
  if (P_Input_Seeds == NULL)
    // Create new seed from internal clock
#ifdef _AIX  // Requirement for the AIX compiler reported by David C. Sussilo
    time((int *) &m_nSeed);
#else
    time(&m_nSeed);
#endif /* _AIX */
  else  // Otherwise...
    {
      // Retrieve new seed from input stream
      *P_Input_Seeds >> m_nSeed;
      // Check for seed retrieval errors
      if (!P_Input_Seeds->good())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 6, "Random Number Generator Seeds");
	  P_Input_Seeds->close();
	  //  P_Input_Seeds == NULL;    CHANGED BY NLN
	  P_Input_Seeds = NULL;
	}
    }
  // If seed output stream is open...
  if (P_Output_Seeds != NULL)
    {
      // Output seed to stream
      *P_Output_Seeds << m_nSeed << endl;
      P_Output_Seeds->flush();
      // Check for seed storage errors
      if (!P_Output_Seeds->good())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 7, "Random Number Generator Seeds");
	  P_Output_Seeds->close();
	  //   P_Input_Seeds == NULL;    CHANGED BY NLN
	  P_Output_Seeds = NULL;
	}
    }
  // Call descendent method to reseed specific generator using new seed
  Raw_Reseed();
}
	
	
/*************************************************************************
*
* METHOD NAME:	Test
*
* DESCRIPTION:	Tests the random number generator. nSample random numbers
*		are generated and their distribution stored. Various
*		statistics are compiled including mean, minimum, maximum,
*		range and Chi-squared. Errors are displayed if the random
*		number generator is not generating uniformly generated
*		numbers within the required range. The floating point
*		random number generator is also tested (in terms of its
*		mean and variance). The level of output can
*		be controlled with the third parameter. This method is 
*		only for testing purposes.
*
* PARAMETERS:	long	nSample		- Number of random numbers to use
*						for test (ie sample size)
*		long	nRange		- Range of random numbers to use
*						for test (from 0 to nRange - 1)
*		int	lpszDisplay	- Controls what should be displayed:
*		   RG_TEST_NO_OUTPUT	 No output
*		   RG_TEST_STATISTICS	 Test statistics displayed only
*		   RG_TEST_DISTRIBUTION  Distribution displayed as well
*
* RETURNS:	Bool			-	TRUE	Successful test
*						FALSE	An error occurred
*
*************************************************************************/

Bool
C_Random_Generator::Test(long nSample, long nRange, int nDisplay)
{
  double  flChiSquared;
  double  flExpected;
  double  flFPSum;
  double  flFPSumSquares;
  double  flFPMean;
  double  flFPRanNum;
  long	  nRanNum;
  long	  nMin;
  long	  nMax;
  long	  nTotal;
  long	  nRangePerElement;
  long*	  p_nDistribution;
  int	  nIndexElement;
  int	  nNumElements;
  int 	  i;
//  char	  lpszFormatStr[MAX_FORMAT_STR_LENGTH];
  ostringstream ParaStream;

  // If the range < 1000 count frequency of each number generated,
  // otherwise count frequency of random numbers within ranges.
  // Calculate number of counters required and the range of reach.

  nRangePerElement = (nRange < 1000) ? 1 : nRange / 1000;

  nNumElements = nRange / nRangePerElement + 1;
  p_nDistribution = new long[nNumElements];
  // Attempt to allocate memory for an array to store the distribution of
  // random numbers. If memory cannot be generated output error.
  if (p_nDistribution == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8,
		    "Random Number Generator Test Distribution");
      return FALSE;
    }

  nMin = MAX_LONG;
  nMax = 0;
  nTotal = 0;
  flChiSquared = 0;
  flFPSum = 0;
  flFPSumSquares = 0;

  // Calculated expected random number (for calculating Chi-squared)
  flExpected = ((double) nRange - 1) / 2;
  // Initialise distribution array
  for (i = 0; i < nNumElements; i++)
    p_nDistribution[i] = 0;

  // Generate and process nSample random numbers
  for (i = 0; i < nSample; i++)
    {
      // Generate floating point random number
      flFPRanNum = New_Num();
      // Calculate statistics for floating point number
      flFPSum += flFPRanNum;
      flFPSumSquares += (flFPRanNum * flFPRanNum);
      // Generate random number
      nRanNum = New_Num(nRange);
      // Ensure random number is positive
      if (nRanNum < 0)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 9, 1, nRanNum);
	  return FALSE;
	}
      // Ensure random number does not exceed range specified
      if (nRanNum > nRange - 1)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 10, 1, nRanNum);
	  return FALSE;
	}
      // Work out which element in the distribution array
      // has a range containing this number
      nIndexElement = nRanNum / nRangePerElement;
      // If an overflow error has occurred for this element
      if (p_nDistribution[nIndexElement] > MAX_LONG)
	{
	  //  Output error.  The exact error message depends on whether
	  //  each element in the distribution is counting a range of
	  //   random numbers (in which case message must be
	  // 'Too many numbers between 75 and 80' or just a single random
	  //   number (in which case, message must be
	  // 'Too many numbers = 78 generated).
	  if (nRangePerElement == 1)
	    m_pApp->Message(MSG_TYPE_ERROR, 11, 2, MAX_LONG, nRanNum);  
	  else
	    m_pApp->Message(MSG_TYPE_ERROR, 12, 3, MAX_LONG,
			  nIndexElement * nRangePerElement, 
			  ((nIndexElement + 1) * nRangePerElement) - 1);
	  return FALSE;
	}
      // Increment count for this range
      p_nDistribution[nIndexElement] ++;
      // Calculate running minimum
      if (nRanNum < nMin)
	nMin = nRanNum;
      // Calculate running maximum
      if (nRanNum > nMax)
	nMax = nRanNum;
      // Ensure running total will not overflow
      if (nTotal > MAX_LONG - nRanNum)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 13, 1, MAX_LONG);
	  return FALSE;
	}
      // Recalculate running total and chi-squared
      nTotal += nRanNum;
      flChiSquared += pow(nRanNum - flExpected, 2) / flExpected;
    }
  // Calculate mean of floating point random numbers
  flFPMean = flFPSum / nSample;
  // Display statistics unless no output is required
  if (nDisplay != RG_TEST_NO_OUTPUT)
    {
      m_pApp->Message(MSG_TYPE_STATUS, 58, 1, nSample);
      m_pApp->Message(MSG_TYPE_STATUS, 59, 1, nRange);
      m_pApp->Message(MSG_TYPE_STATUS, 60, 1, nMin);
      m_pApp->Message(MSG_TYPE_STATUS, 61, 1, nMax);

      ParaStream << setprecision(PREC_RAN_GEN_SAMPLE_STATS) << fixed;
      ParaStream << flChiSquared << ends;
      m_pApp->Message(MSG_TYPE_STATUS, 62, ParaStream.str().c_str());

      ParaStream.str("");
      ParaStream << (double) nTotal / nSample << ends;
      m_pApp->Message(MSG_TYPE_STATUS, 63, ParaStream.str().c_str());

      ParaStream.str("");
      ParaStream << flFPMean << ends;
      m_pApp->Message(MSG_TYPE_STATUS, 64, ParaStream.str().c_str());

      ParaStream.str("");
      ParaStream << (flFPSumSquares / nSample) - (flFPMean * flFPMean) << ends;
      m_pApp->Message(MSG_TYPE_STATUS, 65, ParaStream.str().c_str());

    }
  // If distribution output is required, output distribution
  // of random numbers...
  if (nDisplay == RG_TEST_DISTRIBUTION)
    {
      m_pApp->Message(MSG_TYPE_STATUS, 66);
      // Form of output depends on whether or not each array element
      // represents a range or a single random number
      if (nRangePerElement == 1)
	{
	  // Print column headings
	  m_pApp->Message(MSG_TYPE_STATUS, 67);
	  m_pApp->Message(MSG_TYPE_STATUS, 68);
	  // Print each element of the distribution
	  for (i = 0; i < nRange; i++)
	    {
	      ParaStream.str("");
	      ParaStream << setprecision(PREC_RAN_GEN_SAMPLE_INDEX);
	      ParaStream << i;
	      ParaStream << setprecision(PREC_RAN_GEN_SAMPLE_FREQ);
	      ParaStream << p_nDistribution[i] << ends;
	      m_pApp->Message(MSG_TYPE_STATUS, 56, ParaStream.str().c_str());
	    }
	}
      else
	{
	  // Print column headings
	  m_pApp->Message(MSG_TYPE_STATUS, 69);
	  m_pApp->Message(MSG_TYPE_STATUS, 70);
	  // Print each element of the distribution
	  for (i = 0; i < nNumElements; i++)
	    {
	      ParaStream.str("");
	      ParaStream << setprecision(PREC_RAN_GEN_SAMPLE_INDEX);
	      ParaStream << i * nRangePerElement;
	      ParaStream << ((i + 1) * nRangePerElement) - 1;
	      ParaStream << setprecision(PREC_RAN_GEN_SAMPLE_FREQ);
	      ParaStream << p_nDistribution[i] << ends;
	      m_pApp->Message(MSG_TYPE_STATUS, 56, ParaStream.str().c_str());
	    }
	}
    }

  delete[] p_nDistribution;
  return TRUE;  // Return without error
}
	
	

	
/*************************************************************************
*
* METHOD NAME:	Identify
*
* DESCRIPTION:	The descendent random number generator defines a name for
*		the specific random number generator being used. This method
*		returns a pointer to this name.
*
* RETURNS:	char*	- Pointer to the name of the random number generator.
*
*************************************************************************/

char*
C_Random_Generator::Identify (void)
{
  return m_lpszName;
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	If the seed input and output file streams are open, they
*		must be closed and deleted. The name of the generator
*		must also be deleted.
*
*************************************************************************/

C_Random_Generator::~C_Random_Generator(void)
{
  // If the seed output stream is open...
  if (P_Output_Seeds != NULL)
    {
      // Close and delete the stream
      P_Output_Seeds->close();
      delete P_Output_Seeds;
    }
  // If the seed input stream is open...
  if (P_Input_Seeds != NULL)
    {
      // Close and delete the stream
      P_Input_Seeds->close();
      delete P_Input_Seeds;
    }
  // Delete the random number generator name
  if (m_lpszName != NULL)
    delete[] m_lpszName;
}
