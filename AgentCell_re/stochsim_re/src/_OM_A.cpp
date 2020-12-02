/*************************************************************************
*
* FILENAME:	OM_A.cpp
*
* CLASS:	C_Output_Manager_Array
*
* DESCRIPTION:	This class stores and manages the values of all the array
*               variables in the system. The C_Output_Variable_Array
*               objects are responsible for maintaining their current
*               values. This class saves these values at certain times to
*               output files.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Sets member variables and resets resets all the variables
*               to zero. Sets the minimum column width based on the
*               floating point accuracy required.
*
* PARAMETERS:	C_Application*  p_App          - Pointer to parent application
*
*        :	C_System*  P_Parent           - Pointer to parent system.
*
*************************************************************************/

C_Output_Manager_Array::C_Output_Manager_Array(C_Application* p_App,
					       C_System* P_Parent)
  : C_Output_Manager(p_App, P_Parent)
{
  int	i;

  // Initialise data members

  m_nNumSnapsVar = 0;
  m_nNumDumpVar = 0;
  m_nColumnWidth = 0;
  m_nFPPrecision = 0;
  m_nDumpInterval = 0;
  m_nDumpItCount = 0;

  for (i = 0; i < MAX_NUM_ARRAY_SNAPS_VARIABLES; i ++)
    P_Snaps_Var[i] = NULL;

  for (i = 0; i < MAX_NUM_ARRAY_DUMP_VARIABLES; i ++)
    {
      P_Dump_Var[i] = NULL;
      m_pnDumpColumnWidth[i] = 0;
    }
}


/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the output manager. The files used to store
*               the values are initialised and their headers written.
*
* PARAMETERS:   long   nTimeUnits       - 0 for iterations; 1 for seconds
*				   
*		double flTimeInc - The size of the time increment being used.
*
*		double flNumIterations - The number of iterations in this
*                                        simulation.
*
* RETURNS:	Bool	TRUE		- Object initialised successfully.
*			FALSE		- An error occurred while 
*					  initialising this object.
*
*************************************************************************/

Bool
C_Output_Manager_Array::Init(long nTimeUnits, double flTimeInc,
			     unsigned long long nNumIterations)
{
  int	  i;
  char    lpszFileName[MAX_FILE_NAME_LENGTH];
  char    lpszFilePrefix[MAX_INI_VALUE_LENGTH];
  char    lpszArrayINIFile[MAX_INI_VALUE_LENGTH];
  double  flDumpInterval;
  double  flNumDigitsPreDP;
  double  flNumDigitsPstDP;
  C_Integer_Pair Dimensions;

  // Get array output file name prefix from INI file
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_OUT_PREFIX,
		     lpszFilePrefix))
    return FALSE;

  // If any array dump variables have been created
  if (m_nNumDumpVar != 0)
    {
      // Get array output file name prefix from INI file
      if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE,
			 lpszArrayINIFile))
	return FALSE;
      // Get array output file name prefix from INI file
      if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_DUMP_INTERVAL,
			 &flDumpInterval, lpszArrayINIFile))
	return FALSE;
      // Convert dump interval to iterations (minimum 1 iteration)
      m_nDumpInterval = MAX((unsigned long long)(flDumpInterval / flTimeInc), 1);
    }
    

  // Calculate FP precision for times:
  // If time between storing each set of variables is eg 0.0123456
  // and simulation time is eg 50
  // Number of digits before decimal point = ceil(log10(50)) = 2
  // Number of digits after decimal point  = ceil(-log10(0.0123456)) + 3 = 5
  //   (extra 3 digits after most significant digit)
  // So times will be stored as XX.XXXXX

  // Calculate number of digits to left of decimal point at max time
  flNumDigitsPreDP = MAX(ceil(log10(flTimeInc * nNumIterations)), 1);
  // Calculate number of digits to right of decimal point for time increment
  flNumDigitsPstDP = (MAX(ceil(-log10(flTimeInc * m_nDumpInterval)), 0)
		      + PREC_FP_POST_DP);

  // Set floating point precision (number of digits after decimal point)
  m_nFPPrecision = (int) flNumDigitsPstDP;
  // Set minimum column width based on number of FP accuracy
  m_nColumnWidth 
    = (int) MAX(flNumDigitsPstDP + flNumDigitsPreDP + 1, MIN_COLUMN_WIDTH);

  // Initialise all snapshot variables
  for (i = 0; i < m_nNumSnapsVar; i++)
    {
#ifdef _AIX  // Requirement for the AIX compiler reported by David C. Sussilo
      m_pnDumpColumnWidth[i]
	= (int) ceil(log10((long double) PowerOfTwo(P_Snaps_Var[i]->Get_Array()->Get_Type()
#else
      m_pnDumpColumnWidth[i]
	= (int) ceil(log10((double) PowerOfTwo(P_Snaps_Var[i]->Get_Array()->Get_Type()
#endif /* _AIX */
				      ->Get_Num_Flags())));
      if(!P_Snaps_Var[i]->Init())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 161, 1, i);
	  return FALSE;
	}
    }
  // Initialise all dump variables
  for (i = 0; i < m_nNumDumpVar; i++)
    {
      if(!P_Dump_Var[i]->Init())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 167, 1, i);
	  return FALSE;
	}
    }

  // Initialise snapshot output streams

  // Loop through each snapshot variable we want to store
  for (i = 0; i < m_nNumSnapsVar; i++)
    {
      // Format file name
      sprintf(lpszFileName,"%s%s%s%s_%s%d%s", m_pApp->m_lpszWorkingDir,
	      FILE_PATH_SEPARATOR, lpszFilePrefix,
	      INI_ARRAY_OUTPUT_FILE_PREFIX, ARRAY_OUTPUT_SNAPS_SUFFIX, i + 1,
	      ARRAY_OUTPUT_FILE_SUFFIX);
      // Set variable's output file name
      P_Snaps_Var[i] -> Set_FileName(lpszFileName);

      // Open output file for array snapshot files
      ofstream Snapshot_Stream(lpszFileName);
      // Ensure no errors occurred opening file
      if (!Snapshot_Stream)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 2, "Array Snapshot");
	  return FALSE;
	}
	
      // Output name of variable
      Snapshot_Stream << P_Snaps_Var[i]->Get_Name() 
		      << " (" << P_Snaps_Var[i]->Display_String() << ")"
		      <<endl;
      // Output geometry
      Snapshot_Stream << INI_ARRAY_GEOMETRY << " = "
		      << P_Snaps_Var[i]->Get_Array()->Get_Geometry()
		      <<endl;
      // Close output file
      Snapshot_Stream.close();
    }
  
  // Initialise dump output streams
  for (i = 0; i < m_nNumDumpVar; i++)
    {
      // Format the output file name
      // First, copy the path
      strcpy(lpszFileName, m_pApp->m_lpszWorkingDir);
      strcat(lpszFileName, FILE_PATH_SEPARATOR);
      // Append the filename prefix
      strcat(lpszFileName, lpszFilePrefix);
      // Append the entire variable name
      strcat(lpszFileName, P_Dump_Var[i]->Get_Name());
      // Append suffix
      strcat(lpszFileName, ARRAY_OUTPUT_FILE_SUFFIX);
      // Set variable's output file name
      P_Dump_Var[i] -> Set_FileName(lpszFileName);

      ofstream Dump_Stream(lpszFileName);
      // Ensure no errors occurred opening file
      if (!Dump_Stream)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 2, "Array Dump");
	  return FALSE;
	}

      // Output name of variable
      Dump_Stream << setiosflags(ios::left) << setw(m_nColumnWidth);
      Dump_Stream << P_Dump_Var[i]->Get_Name() << endl;
      // Output geometry
      Dump_Stream << INI_ARRAY_GEOMETRY << " = "
		      << P_Dump_Var[i]->Get_Array()->Get_Geometry()
		      <<endl;
      // Output dimensions of array
      P_Dump_Var[i]->Get_Dimensions(&Dimensions);
      Dump_Stream << Dimensions.nX << "x" << Dimensions.nY<< endl;
      // Close output file
      Dump_Stream.close();
    }

  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Handle_Output
*
* DESCRIPTION:	Handles all output operations that need to be done in the
*               specified iteration of the simulation. 
*
* PARAMETERS:   double	flCurrentTime	- Current simulation time
*
* RETURNS:	Bool	TRUE		- Output operations were successful
*			FALSE		- An error occurred during output
*					  operations.
*
*************************************************************************/

Bool
C_Output_Manager_Array::Handle_Output(double flCurrentTime)
{
  long			i;
  long			j;

  // Loop through the snapshot variables
  for (i = 0; i < m_nNumSnapsVarSets; i++)
    {
      // If we are within an interval in which the array values
      // should be sampled for averaging...
      if (P_SnapsVarSet[i].bAveraging &&
	  P_SnapsVarSet[i].nStoreItCount >= P_SnapsVarSet[i].nSampleStartIt)
	{
	  // If values are to be sampled for averaging in this iteration
	  if (P_SnapsVarSet[i].nSampleItCount == P_SnapsVarSet[i].nSampleItvl)
	    {
	      // Update values of all the variables in set
	      for (j = 0; P_SnapsVarSet[i].P_Var[j] != NULL; j++)
		{
		  P_SnapsVarSet[i].P_Var[j] -> Update();
		}
	      // Reset sampling interval counter
	      P_SnapsVarSet[i].nSampleItCount = 0;
	    }
	  // Increment sampling interval counter
	  P_SnapsVarSet[i].nSampleItCount ++;
	}
    }

  // Increment dump interval counter
  m_nDumpItCount ++;
  // If values are to be dumped in this iteration
  if (m_nDumpItCount == m_nDumpInterval)
    {
      // Save values to file
      if(!Save_Dump(flCurrentTime))
	return FALSE;
      // Reset dump interval counter
      m_nDumpItCount = 0;
    }


  // For each set of variables with common store intervals
  for (i = 0; i < m_nNumSnapsVarSets; i++)
    {
      // Increment storage iteration counter
      P_SnapsVarSet[i].nStoreItCount ++; 
      // Check if values need to be stored in this iteration
      if (P_SnapsVarSet[i].nStoreItCount == P_SnapsVarSet[i].nStoreItvl)
	{
	  // Save snapshots in this set to file
	  if(!Save_Snaps(i, flCurrentTime))
	    return FALSE;
	  // Reset storage iteration counter
	  P_SnapsVarSet[i].nStoreItCount = 0; 
	}
    }

  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Save_Snaps
*
* DESCRIPTION:	Saves a single snapshot of the complex array stored within
*		the object by appending the values to an output file.
*
* PARAMETERS:   long    nSet            - Numerical identifer indicating
*                                         which set of snapshot variables
*                                         is to be saved.
*                                       - Pointer to the output variable
*                                         which is to be saved.
*               double  flCurrentTime   - The current time in the simulation.
*
* RETURNS:	Bool	TRUE		- Snapshot was saved successfully.
*			FALSE		- An error occurred while saving
*					  the snapshot.
*
*************************************************************************/

Bool
C_Output_Manager_Array::Save_Snaps(long nSet, double flCurrentTime)
{
  long           i;
  int            nXCount;
  int            nYCount;
  char           lpszValueOutput[10];
  C_Integer_Pair Dimensions;
  ofstream*	 P_Snaps_Stream;
  long**         P_P_nValues;
  long           nNumSampled;

  // Create output stream
  P_Snaps_Stream = new ofstream;

  // Loop through all variables in set
  for (i = 0; P_SnapsVarSet[nSet].P_Var[i] != NULL; i++)
    {
      // Update variable before saving values to file
      P_SnapsVarSet[nSet].P_Var[i] -> Update();
      // Get values from variable
      P_P_nValues = P_SnapsVarSet[nSet].P_Var[i] -> Get_Values();
      nNumSampled = P_SnapsVarSet[nSet].P_Var[i] -> Get_NumSampled();

#ifdef _OM_A_DEBUG  // For debugging array output manager
      if (!P_SnapsVarSet[nSet].bAveraging && nNumSampled != 1)
	{
	  cerr << "Fatal Error: The number of sampled values is larger than 1 for non-averaging Snapshot Variable " << P_Snaps_Var[i]->Get_Name() << "!!" << endl;
	  return FALSE;
	}
#endif /* _OM_A_DEBUG */

      // Open output stream for this variable
      if(!Safe_Open(m_pApp, P_Snaps_Stream,
		    P_SnapsVarSet[nSet].P_Var[i]->Get_FileName(),
		    ios::app, "Complex Array Variable Values"))
	{
	  return FALSE;
	}
      // Get dimensions of current variable
      P_SnapsVarSet[nSet].P_Var[i] -> Get_Dimensions(&Dimensions);

      // Output XPM header for 16-level greyscale
      
      *P_Snaps_Stream << "/* XPM */" << endl;
      // Output time
      *P_Snaps_Stream << setiosflags(ios::right | ios::fixed)
		      << setw(m_nColumnWidth)
		      << "/* TIME: "
		      << setprecision(m_nFPPrecision)
		      << flCurrentTime
		      << " */"
		      << endl << endl;
		
      *P_Snaps_Stream << "static char * array_xpm[] = {" << endl
		      << "\"" << Dimensions.nX
		      << " " << Dimensions.nY
		      << " " << MAX_ARRAY_AVERAGE_VALUE + 1
		      << " " << 1 << "\"," << endl
		      << "\"" << "0\tc " << "#000000" << "\"," << endl
		      << "\"" << "1\tc " << "#111111" << "\"," << endl
		      << "\"" << "2\tc " << "#222222" << "\"," << endl
		      << "\"" << "3\tc " << "#333333" << "\"," << endl
		      << "\"" << "4\tc " << "#444444" << "\"," << endl
		      << "\"" << "5\tc " << "#555555" << "\"," << endl
		      << "\"" << "6\tc " << "#666666" << "\"," << endl
		      << "\"" << "7\tc " << "#777777" << "\"," << endl
		      << "\"" << "8\tc " << "#888888" << "\"," << endl
		      << "\"" << "9\tc " << "#999999" << "\"," << endl
		      << "\"" << "a\tc " << "#aaaaaa" << "\"," << endl
		      << "\"" << "b\tc " << "#bbbbbb" << "\"," << endl
		      << "\"" << "c\tc " << "#cccccc" << "\"," << endl
		      << "\"" << "d\tc " << "#dddddd" << "\"," << endl
		      << "\"" << "e\tc " << "#eeeeee" << "\"," << endl
		      << "\"" << "f\tc " << "#ffffff" << "\"," << endl;

      // Loop through all of the coordinates in the variable and output value
      for (nYCount = 0; nYCount < Dimensions.nY; nYCount++)
	{
	  *P_Snaps_Stream << "\"";
	  for (nXCount = 0; nXCount < Dimensions.nX; nXCount++)
	    {
	      sprintf(lpszValueOutput, "%lx",
		      (long) ((P_P_nValues[nXCount][nYCount]
			       / (double) nNumSampled)
			      * MAX_ARRAY_AVERAGE_VALUE));
	      *P_Snaps_Stream << lpszValueOutput;
	      if (nXCount == Dimensions.nX -1)
		*P_Snaps_Stream << "\"";
	    }
	  if(nYCount == Dimensions.nY - 1)
	    *P_Snaps_Stream << "};";
	  else
	    *P_Snaps_Stream << ",";
	  *P_Snaps_Stream << endl;
	}
      *P_Snaps_Stream << endl;
      P_Snaps_Stream->close();  // Close output file
      // Reset this snapshot variable
      P_SnapsVarSet[nSet].P_Var[i] -> Reset();
    }
  delete P_Snaps_Stream;
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Save_Dump
*
* DESCRIPTION:	Saves a dump of each of the defined dump variables for
*               complex arrays.
*
* PARAMETERS    double  flCurrentTime   - The current simulation time.
*
* RETURNS:	Bool	TRUE		- Values were dumped successfully.
*			FALSE		- An error occurred while dumping
*					  the values.
*
*************************************************************************/

Bool
C_Output_Manager_Array::Save_Dump(double flCurrentTime)
{
  long           i;
  int            nXCount;
  int            nYCount;
  C_Integer_Pair Dimensions;
  ofstream*	 P_Dump_Stream;
  long**         P_P_nValues;

  // Create the output stream
  P_Dump_Stream = new ofstream;

  // Loop through all the dump variables
  for (i = 0; i < m_nNumDumpVar; i++)
    {
      // Update the values of this variable before saving
      P_Dump_Var[i] -> Update();
      P_P_nValues = P_Dump_Var[i] -> Get_Values();

      if(!Safe_Open(m_pApp, P_Dump_Stream, P_Dump_Var[i]->Get_FileName(),
		    ios::app,"Complex Array Variable Values"))
	  return FALSE;

      // Get dimensions of current variable
      P_Dump_Var[i] -> Get_Dimensions(&Dimensions);
      
      // Output header for simple ascii format (time only)
      *P_Dump_Stream << setiosflags(ios::right | ios::fixed)
		     << setw(m_pnDumpColumnWidth[i])
		     << "TIME: " << setprecision(m_nFPPrecision)
		     << flCurrentTime << endl << endl;

      // Loop through all of the coordinates in the variable and output value
      for (nYCount = 0; nYCount < Dimensions.nY; nYCount++)
	{
	  for (nXCount = 0; nXCount < Dimensions.nX; nXCount++)
	    {
	      *P_Dump_Stream << ARRAY_DUMP_COLUMN_SEPARATOR
			     << setiosflags(ios::right | ios::fixed)
			     << setw(m_pnDumpColumnWidth[i])
			     << P_P_nValues[nXCount][nYCount];
	    }
	  *P_Dump_Stream << endl;
	}
      P_Dump_Stream->close();  // Close output file
    }
  delete P_Dump_Stream;
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	This method clears up the memory allocated
*
*************************************************************************/

C_Output_Manager_Array::~C_Output_Manager_Array(void)
{
  int i;

  for (i = 0; i < MAX_NUM_ARRAY_SNAPS_VARIABLES; i ++)
      if (P_Snaps_Var[i] != NULL)
	delete P_Snaps_Var[i];

  for (i = 0; i < MAX_NUM_ARRAY_DUMP_VARIABLES; i ++)
      if (P_Dump_Var[i] != NULL)
	delete P_Dump_Var[i];
}

/*************************************************************************
*
* METHOD NAME:	Add_DumpVariable
*
* DESCRIPTION:	This method adds one member to the output manager's list
*               of dump variables. (Used at initialisation time)
*
* PARAMETERS:	C_Output_Variable* pNewVar - Pointer to newly created
*               variable.
*
*************************************************************************/

Bool
C_Output_Manager_Array::Add_DumpVariable(C_Output_Variable* pNewVar)
{
  // Ensure that too many dump variables have not been allocated
  if (m_nNumDumpVar >= MAX_NUM_DUMP_VARIABLES)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 162);
      return FALSE;
    }
  // Insert new variable at the end of the list
  P_Dump_Var[m_nNumDumpVar] = (C_Output_Variable_Array*) pNewVar;
  
  // Update the number of dump variables
  m_nNumDumpVar ++;
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Add_SnapsVariable
*
* DESCRIPTION:	This method adds one member to the output manager's list
*               of snapshot variables. (Used at initialisation time)
*
* PARAMETERS:	C_Output_Variable* pNewVar - Pointer to newly
*                                            created variable.
*               long  nStoreItvl     - Interval between storage times.
*
*               Bool  bAveraging     - Flag indicating whether or not output
*                                      should be averaged
*
*               long  nSampleItvl    - Interval between sampling times for
*                                      averaging.
*       	long  nAveItvl       - Interval over which output is averaged.
*
*************************************************************************/

Bool
C_Output_Manager_Array::Add_SnapsVariable(C_Output_Variable* pNewVar,
					  unsigned long long nStoreItvl,
					  Bool bAveraging,
					  unsigned long long nSampleItvl,
					  unsigned long long nAveItvl)
{
  int i;
  int j;

  // Ensure that too many snapshot variables have not been allocated
  if (m_nNumSnapsVar >= MAX_NUM_ARRAY_SNAPS_VARIABLES)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 128);
      return FALSE;
    }
  // Insert new variable at the end of the list
  P_Snaps_Var[m_nNumSnapsVar] = (C_Output_Variable_Array_Snaps*) pNewVar;

  // Update the number of snapshot variables
  m_nNumSnapsVar ++;

  // Loop through all the sets of snapshot variables
  for (i = 0; i < m_nNumSnapsVarSets; i++)
    {
      // If all intervals match this set
      if (P_SnapsVarSet[i].nStoreItvl == nStoreItvl
	  && P_SnapsVarSet[i].nSampleItvl == nSampleItvl
	  && P_SnapsVarSet[i].nAveItvl == nAveItvl)
	{
	  // Add this variable to the set and return
	  for(j = 0; P_SnapsVarSet[i].P_Var[j] != NULL; j++);
	  P_SnapsVarSet[i].P_Var[j] = (C_Output_Variable_Array_Snaps*) pNewVar;
	  return TRUE;
	}
    }
  // If the end of list was reached without a match,
  // create a new set based on this variable and return
  P_SnapsVarSet[i].P_Var[0] = (C_Output_Variable_Array_Snaps*) pNewVar;
  P_SnapsVarSet[i].nStoreItvl = nStoreItvl;
  P_SnapsVarSet[i].bAveraging = bAveraging;
  P_SnapsVarSet[i].nSampleItvl = nSampleItvl;
  P_SnapsVarSet[i].nAveItvl = nAveItvl;
  if (bAveraging)
    {
      P_SnapsVarSet[i].nSampleStartIt = nStoreItvl - nAveItvl;
    }
  else
    {
      P_SnapsVarSet[i].nSampleStartIt = MAX_UNSIGNED_LONG_LONG;
    }
  m_nNumSnapsVarSets ++;
  return TRUE;
}

  
/*************************************************************************
*
* METHOD NAME:	Display_And_Store
*
* DESCRIPTION:	Displays and stores all variables.
*
* PARAMETERS:	double flTime - The time at which these values are
*				being displayed.
*
*************************************************************************/

Bool
C_Output_Manager_Array::Display_And_Store(double flTime)
{
  long i;
  
  for (i = 0; i < m_nNumSnapsVarSets; i++)
    {
      // Save snapshots in this set to file
      if(!Save_Snaps(i, flTime))
	return FALSE;
      // Reset storage iteration counter
      P_SnapsVarSet[i].nStoreItCount = 0; 
    }
  // Save values to file
  if(!Save_Dump(flTime))
    return FALSE;

  return TRUE;
}

