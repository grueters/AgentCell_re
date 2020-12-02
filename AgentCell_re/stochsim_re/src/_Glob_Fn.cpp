/*************************************************************************
*
* FILENAME:	Glob_Fn.cpp
*
* DESCRIPTION:	Global functions, including friends of classes. In
*		particular, any overloading of insertion / extraction 
*		stream operators must be defined here (because of scope 
*               consideration these functions must be declared as global 
*		functions and given friend status for the appropriate class).
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* FUNCTION NAME:	LongToBitString
*
* DESCRIPTION:		This function converts a long into a string containing
*			the binary equivalent.
*
* PARAMETERS:		long	nValue	    - Value to convert into binary
*			char*	lpszBinary  - Character array used to store
*                                             binary value
*			int	nDigits	    - Number of digits in bit string
* RETURNS:			lpszBinary  - Pointer to the bit string
*
*************************************************************************/

char*
LongToBitString(long nValue, char* lpszBinary, const int nDigits)
{
  long	nWorkingState;
  long	nWorkingMaxState;
  int	nDigit;
  char*	lpszBitString;
  
  lpszBitString = NULL;
  // Create empty bit string (include space for null terminator)
  lpszBitString = new char[nDigits + 1];
  // Ensure bit string was created
  if (lpszBitString == NULL)
    return NULL;
  nWorkingState = nValue;
  // Calculate the most significant bit
  nWorkingMaxState = PowerOfTwo(nDigits);
  nDigit = 0;
  // Start from the most significant digit and work towards the least
  while (nWorkingMaxState > 1)
    {
      // Move to the next binary digit
      nWorkingMaxState /= 2;
      // If the number is greater than the value of the current digit
      if (nWorkingState >= nWorkingMaxState)
	{	
	  // Store a '1' in the string
	  lpszBitString[nDigit] = ON_BIT_CHAR;
	  // And calculate the remainder
	  nWorkingState -= nWorkingMaxState;
	}
      else  // Otherwise...
	lpszBitString[nDigit] = OFF_BIT_CHAR;  // Store a '0'
      nDigit ++;
    }
  lpszBitString[nDigit] = NULL_CHAR;  // Null-terminate bit string
  strcpy(lpszBinary, lpszBitString);
  delete[] lpszBitString;
  return lpszBinary;
}


/*************************************************************************
*
* FUNCTION NAME:	PowerOfTwo
*
* DESCRIPTION:		This function returns an integer power of two.
*
* PARAMETERS:		int nExponent	- Exponent
*
* RETURNS:		long		- Two to the power of nExponent
*
*************************************************************************/

long
PowerOfTwo(int nExponent)
{
  long nResult;
  int  nCount;

  nResult = 1;
  for (nCount = 0; nCount < nExponent; nCount ++)
    nResult *= 2;
  return nResult;
}


/*************************************************************************
*
* FUNCTION NAME:	BitStringToLong
*
* DESCRIPTION:		This function converts a bit string into a long
*                       integer.
*
* PARAMETERS:		C_Application* p_App - Pointer to parent application
*         :		char*	lpszBinary  - Character array containing bit
*					      string.
*
* RETURNS:			long	Long integer equivalent of lpszBinary
*
*************************************************************************/

long
BitStringToLong (C_Application* p_App, char* lpszBinary)
{
  int	nDigit;
  long	nLongValue;
  long	nDigitValue;

  nLongValue = 0;
  nDigitValue = 1;
  // Loop through each digit in binary string
  for (nDigit = strlen(lpszBinary) - 1; nDigit >= 0; nDigit --)
    {
      // If the digit is not recognised, output error and return zero
      if ((lpszBinary[nDigit] != ON_BIT_CHAR)
	  && (lpszBinary[nDigit] != OFF_BIT_CHAR))
	{
	  p_App->Message(MSG_TYPE_ERROR, 101, lpszBinary);
	  return 0;
	}
      // If the digit is represents a bit, add the corresponding digit value
      if (lpszBinary[nDigit] == ON_BIT_CHAR)
	nLongValue += nDigitValue;
      // Update the current digit value
      nDigitValue <<= 1;
    }
  // Return the value of the binary string
  return nLongValue;
}

/*************************************************************************
*
* FUNCTION NAME:	Safe_Open
*
* DESCRIPTION:		This function attempts to safely open an output file
*                       stream by repeatedly trying to open the file for
*                       appending output, until either the file opens
*                       successfully or the maximum time limit is exceeded.
*                       This is necessary in case another program (eg Excel)
*                       is opening the file (for eg graphing).
* PARAMETERS:           C_Application* p_App - pointer to parent application
*       		char*	lpszBinary  - Character array containing bit
*					      string.
*
* RETURNS:			long	Long integer equivalent of lpszBinary
*
*************************************************************************/

Bool
Safe_Open(C_Application* p_App, ofstream* P_Stream, const char* lpszFileName, int nMode,
	 const char* lpszErrArg)
{
  long nStart;
  nStart = clock();  // Start clock running
  do
#ifdef _STD_CPP 
    P_Stream -> open(lpszFileName, (std::ios::openmode) nMode);	
#else
    P_Stream -> open(lpszFileName, nMode);	
#endif /* _STD_CPP */
#if ( defined(_ALPHA) || defined(_AIX) )
  while ((!P_Stream->rdbuf()->is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#else
  while ((!P_Stream->is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
  // Ensure file was opened successfully (eventually)
  if (P_Stream == NULL)
    {
      p_App->Message(MSG_TYPE_ERROR, 2, lpszErrArg);
      return FALSE;
    }
  return TRUE;
}
