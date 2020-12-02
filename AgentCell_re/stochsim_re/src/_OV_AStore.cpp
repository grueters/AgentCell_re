/*************************************************************************
*
* FILENAME:	OV_AStore.cpp
*
* CLASS:	C_Output_Variable_Array_Store
*
* DESCRIPTION:	This class stores the values of all the array variables in
*		the system. The C_Output_Variable_Array objects are responsible
*		for maintaining their current values. This class stores
*		these values at certain times and saves the values in a file.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Sets member variables and resets resets all the stored
*		values for all the variables to zero. Sets the minimum
*		column width based on the floating point accuracy required.
*
* PARAMETERS:	C_Output_Variable_Array**
                                    P_P_OV_System  - Pointer to array of
*						     variable pointers.
*		long		    nNumVariables  - Number of variables
*						     in the system.
*		char*		    lpszFilePrefix - Name of file to store
*						     the values in.
*		double		  flNumDigitsPreDP - Num digits before decimal
*						     point in float point
*						     number (times).
*		double		  flNumDigitsPstDP - Num digits after decimal
*						     point in float point
*						     number (times).
*
*************************************************************************/

C_Output_Variable_Array_Store::C_Output_Variable_Array_Store(C_Output_Variable_Array** P_P_OV_System,long nNumVariables, char* lpszFilePrefix, double flNumDigitsPreDP, double flNumDigitsPstDP)
{
  int	i;
  char	lpszFileSuffix[MAX_FILE_NAME_LENGTH];

  // Initialise data members

  m_nNumVariables = nNumVariables;
  P_P_Variable = P_P_OV_System;
  // Set floating point precision (number of digits after decimal point)
  m_nFPPrecision = (int) flNumDigitsPstDP;
  // Set minimum column width based on number of FP accuracy
  m_nColumnWidth 
    = (int) MAX(flNumDigitsPstDP + flNumDigitsPreDP + 1, MIN_COLUMN_WIDTH);
  m_flTime = 0;
  m_flCoordTime = 0;
  m_bAveraging = FALSE;
  m_nNumSampled = 0;
  m_bIsDumpStore = FALSE;

  for (i = 0; i < MAX_NUM_ARRAY_VARIABLES; i++){
    P_P_P_nValues[i] = NULL;
    P_P_P_nCoordValues[i] = NULL;
  }
	
  // If this is a dump variable store...
  if (m_bIsDumpStore)
    {
      // For each variable...
      for (i = 0; i < m_nNumVariables; i ++)
	{
	  // Format file name
	  
	  // First, copy the output path
	  strcpy(m_lpszValuesFileName[i], m_pApp->m_lpszWorkingDir);
	  strcat(m_lpszValuesFileName[i], FILE_PATH_SEPARATOR);
	  // Append the filename prefix
	  strcat(m_lpszValuesFileName[i], lpszFilePrefix);
	  // Append the entire variable name
	  strcat(m_lpszValuesFileName[i], P_P_Variable[i]->Get_Name());
	  // Terminate at separator to strip off bitstring
	  *(strstr(m_lpszValuesFileName[i], ARRAY_NAME_SEPARATOR)) = NULL_CHAR;
	  // Append suffix (numbered)
	  sprintf(lpszFileSuffix, "_%s%d%s", ARRAY_OUTPUT_DUMP_SUFFIX, i + 1, 
		  ARRAY_OUTPUT_FILE_SUFFIX);
	  strcat(m_lpszValuesFileName[i], lpszFileSuffix);
	}
    }
  else  // Otherwise, this is a snaps variable store
    {
      // For each variable...
      for (i = 0; i < m_nNumVariables; i ++)
	{
	  // Format file name

	  // First, copy the output path
	  strcpy(m_lpszValuesFileName[i], m_pApp->m_lpszWorkingDir);
	  strcat(m_lpszValuesFileName[i], FILE_PATH_SEPARATOR);
	  // Append the filename prefix
	  strcat(m_lpszValuesFileName[i], lpszFilePrefix);
	  // Append the entire variable name
	  strcat(m_lpszValuesFileName[i], P_P_Variable[i]->Get_Name());
	  // Terminate at separator to strip off bitstring
	  *(strstr(m_lpszValuesFileName[i], ARRAY_NAME_SEPARATOR)) = NULL_CHAR;
	  // Append suffix (numbered)
	  sprintf(lpszFileSuffix, "_%s%d%s", ARRAY_OUTPUT_SNAPS_SUFFIX, i + 1, 
		  ARRAY_OUTPUT_FILE_SUFFIX);
	  strcat(m_lpszValuesFileName[i], lpszFileSuffix);
	}

      // For each variable...
      for (i = 0; i < m_nNumVariables; i ++)
	{
	  // Copy prefix of file name
	  strcpy(m_lpszCoordFileName[i], P_P_Variable[i]->Get_Name());
	  // Append suffix (numbered)
	  sprintf(lpszFileSuffix, "_%s%d%s", ARRAY_OUTPUT_COORD_SUFFIX, i + 1, 
		  ARRAY_OUTPUT_FILE_SUFFIX);
	  strcat(m_lpszCoordFileName[i], lpszFileSuffix);
	}
    }
}


/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the output variable storage object.
*		The file used to store the values in is
*		initialised and column headers are added to the file.
*		The maximum variable name length is calculated, so that
*		all columns will be equal to this length.
*
* PARAMETERS:   Bool bAveraging         - Indicates whether or not averaged
*                                         values are being used
*               Bool bIsDumpStore       - Indicates whether or not this is
*                                         a dump variable store
*
* RETURNS:	Bool	TRUE		- Object initialised successfully.
*			FALSE		- An error occurred while 
*					  initialising this object.
*
*************************************************************************/

Bool
C_Output_Variable_Array_Store::Init(Bool bAveraging, Bool bIsDumpStore)
{
  int				i;
  int				j;
  int				nXCount;
  int				nYCount;
  int				nCountWC;
  C_Integer_Pair	Dimensions;
  C_Integer_Pair*	P_WCList;
  char			lpszCoordName[MAX_ARRAY_COORD_STRING_LENGTH];

  m_bAveraging = bAveraging;
  m_bIsDumpStore = bIsDumpStore;

  // Loop through each variable we want to store
  for (i = 0; i < m_nNumVariables; i++)
    {
      // Get dimensions of current variable
      P_P_Variable[i]->Get_Dimensions(&Dimensions);
      // Allocate memory for a pointer for all values to be stored
      P_P_P_nValues[i] = new long* [Dimensions.nX];
      // Allocate memory for a pointer for all coordinate values to be stored
      P_P_P_nCoordValues[i] = new long* [Dimensions.nX];
		
      for (nXCount = 0; nXCount < Dimensions.nX; nXCount++)
	{
	  // Allocate memory to store values for each row in complex array
	  P_P_P_nValues[i][nXCount] = new long [Dimensions.nY];
	  // Allocate memory to store values for each row in complex array
	  P_P_P_nCoordValues[i][nXCount] = new long [Dimensions.nY];
			
	  // If the array was not created correctly...
	  if (P_P_P_nValues[i][nXCount] == NULL
	      || P_P_P_nCoordValues[i][nXCount] == NULL)
	    {
	      // Output error
	      m_pApp->Message(MSG_TYPE_ERROR, 8, "Output Variable values");
	      return FALSE;
	    }
	}

      // Reset variable values
      for (nXCount = 0; nXCount < Dimensions.nX; nXCount++)
	for (nYCount = 0; nYCount < Dimensions.nY; nYCount++)
	  P_P_P_nValues[i][nXCount][nYCount] = 0;
    }

  // Initialise output streams

  // For each variable...
  for (i = 0; i < m_nNumVariables; i++)
    {
      // Get dimensions of current variable
      P_P_Variable[i]->Get_Dimensions(&Dimensions);

      // Open output file for array snapshot files
      ofstream Value_Stream(m_lpszValuesFileName[i]);
      // Ensure no errors occurred opening file
      if (!Value_Stream)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 2, "Array Snapshot");
	  return FALSE;
	}
	
      Value_Stream << setiosflags(ios::right) << setw(m_nColumnWidth);
      // Output name of variable
      Value_Stream << P_P_Variable[i]->Get_Name() << endl;
      // Close output file
      Value_Stream.close();

      //	...for specific coordinate dump files
      if (P_P_Variable[i]->Get_Num_Watch_Coordinates() > 0)
	{
	  ofstream Coord_Value_Stream(m_lpszCoordFileName[i]);
	  // Ensure no errors occurred opening file
	  if (!Coord_Value_Stream)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 2, "Watch Coordinates");
	      return FALSE;
	    }
	  // Set formatting parameters for output stream
	  Coord_Value_Stream << setiosflags(ios::right)
			     << setw(m_nColumnWidth);
	  // Output name of variable
	  Coord_Value_Stream << P_P_Variable[i]->Get_Name() << endl;
	  
	  P_WCList = new C_Integer_Pair [(P_P_Variable[i]
					  -> Get_Num_Watch_Coordinates())];
	  P_P_Variable[i]->Get_WCList(P_WCList);
	  
	  for (nCountWC = 0;
	       nCountWC < P_P_Variable[i]->Get_Num_Watch_Coordinates();
	       nCountWC++)
	    {
	      sprintf(lpszCoordName, ARRAY_COORDINATE_FORMAT_STRING,
		      P_WCList[nCountWC].nX, P_WCList[nCountWC].nY);
	      m_nCoordColumnWidth = MAX(m_nCoordColumnWidth,
					(signed) strlen(lpszCoordName));
	    }
	  Coord_Value_Stream << setiosflags(ios::right)
			     << setw(m_nCoordColumnWidth) << TIME_COLUMN_TITLE;

	  // Loop through all the variables and display their names;
	  // this will form the column headings
	  for (nCountWC = 0;
	       nCountWC < P_P_Variable[i]->Get_Num_Watch_Coordinates();
	       nCountWC++)
	    {
	      sprintf(lpszCoordName, ARRAY_COORDINATE_FORMAT_STRING,
		      P_WCList[nCountWC].nX, P_WCList[nCountWC].nY);
	      Coord_Value_Stream << VALUES_COLUMN_SEPARATOR;
	      for (j = 0;
		   j < (m_nCoordColumnWidth - (signed) strlen(lpszCoordName));
		   j++)
		// Right align column headings by left padding them
		// with spaces and then output name
		Coord_Value_Stream << SPACE_STR;
	      Coord_Value_Stream << lpszCoordName;
	    }
	  Coord_Value_Stream << endl;
	  Coord_Value_Stream.close();  // Close output file
	  
	  // Free memory allocated for storing watched coordinates
	  delete[] P_WCList;	
	}
    }
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Store
*
* DESCRIPTION:	Stores the current values of variables in a member
*		variable array if required, along with the current time. 
*		These are then saved in a file by calling Save_Values().
*
* PARAMETERS:	double	flCurrentTime	- Current simulation time
*
* RETURNS:	Bool	TRUE		- Values were stored successfully.
*			FALSE		- An error occurred while storing
*					  current values.		
*
*************************************************************************/

Bool
C_Output_Variable_Array_Store::Store(double flCurrentTime, long* P_VarID)
{
  long			i;

  // Store the current simulation time
  m_flTime = flCurrentTime;

  i = 0;
  // Loop through the variables until rogue value is reached
  while (P_VarID[i] != -1)
    {
      P_P_Variable[P_VarID[i]] -> Get_Values(P_P_P_nValues[P_VarID[i]]);
      // Save value of specified variable
      if (!Save_Values(P_VarID[i]))
	{
	  return FALSE;
	}
      i ++;
    }
	
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Update
*
* DESCRIPTION:	Updates the value of all variables in store.
*
* PARAMETERS:	NONE
*
* RETURNS:	NONE
*
*************************************************************************/

void
C_Output_Variable_Array_Store::Update(void)
{
  int i;

  // Loop through each of the variables in the system and increment values
  for (i = 0; i < m_nNumVariables; i ++)
    // Increment values for current variable
    P_P_Variable[i] -> Inc_Values(P_P_P_nValues[i]);
  // Increment number of values averaged
  m_nNumSampled ++;
}

/*************************************************************************
*
* METHOD NAME:	Reset
*
* DESCRIPTION:	Resets the value of all variables in store.
*
* PARAMETERS:	NONE
*
* RETURNS:	NONE
*
*************************************************************************/

void
C_Output_Variable_Array_Store::Reset(void)
{
  int            i;
  long		 nXCount;
  long		 nYCount;
  C_Integer_Pair Dimensions;

  // Reset values to zero
  for (i = 0; i < m_nNumVariables; i++)
    {
      P_P_Variable[i] -> Get_Dimensions(&Dimensions);
      for (nXCount = 0; nXCount < Dimensions.nX; nXCount ++)
	for (nYCount = 0; nYCount < Dimensions.nY; nYCount ++)
	  P_P_P_nValues[i][nXCount][nYCount] = 0;
    }
  m_nNumSampled = 0;
}

/*************************************************************************
*
* METHOD NAME:	Store_Coordinates
*
* DESCRIPTION:	Stores the current values of all coordinates within the
*               variables in memory if required, along with the current time. 
*		These are then saved in a file by calling
*               Save_Coordinate_Values().
*
* PARAMETERS:	double	flCurrentTime	- Current simulation time
*
* RETURNS:	Bool	TRUE		- Values were stored successfully.
*			FALSE		- An error occurred while storing
*					  current values.		
*
*************************************************************************/

Bool
C_Output_Variable_Array_Store::Store_Coordinates(double flCurrentTime)
{
  int            i;
  long           nXCount;
  long           nYCount;
  C_Integer_Pair Dimensions;

  
  // Ensure there is sufficient memory to store a new value
  if (m_nNumCoordValuesStored >= MAX_NUM_VALUES_STORED)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 1);
      return FALSE;
    }

  // Loop through each of the variables in the system and store values
  for (i = 0; i < m_nNumVariables; i ++)
    {
      P_P_Variable[i] -> Get_Dimensions(&Dimensions);
      // Reset variable values to 0
      for (nXCount = 0; nXCount < Dimensions.nX; nXCount++)
	for (nYCount = 0; nYCount < Dimensions.nY; nYCount++)
	  P_P_P_nCoordValues[i][nXCount][nYCount] = 0;
      // Increment values for current variable
      P_P_Variable[i] -> Inc_Values(P_P_P_nCoordValues[i]);
    }
  
  // Store the current simulation time
  m_flCoordTime = flCurrentTime;
      
  if (!Save_Coordinate_Values())
    {
      m_pApp->Message(MSG_TYPE_ERROR, 134, P_P_Variable[i]->Get_Name());
      return FALSE;
    }
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Save_Values
*
* DESCRIPTION:	Saves a single set of variable values stored within 
*		the object by appending the values to an output file.
*
* PARAMETERS:   long    nVarID          - The ID of the variable to save.
*
* RETURNS:	Bool	TRUE		- Values were saved successfully.
*			FALSE		- An error occurred while saving
*					  the values.		
*
*************************************************************************/

Bool
C_Output_Variable_Array_Store::Save_Values(long nVarID)
{
  int            nXCount;
  int            nYCount;
  long		 nStart;
  char           lpszValueOutput[10];
  C_Integer_Pair Dimensions;
  ofstream*	 P_Value_Stream;

  P_Value_Stream = new ofstream;

  nStart = clock();  // Start clock running
  // Repeatedly try to open file for appending output, until either
  // the file opens successfully or maximum time limit is exceeded.
  // This is necessary in case another program (eg Excel) is opening
  // file (for eg graphing)
  do
    P_Value_Stream -> open(m_lpszValuesFileName[nVarID], ios::app);	
#if ( defined(_ALPHA) || defined(_AIX) )
  while ((!P_Value_Stream->rdbuf()->is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#else
  while ((!P_Value_Stream->is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
  // Ensure file was opened successfully (eventually)
  if (P_Value_Stream == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 2, "Complex Array Variable Values");
      return FALSE;
    }

  // Get dimensions of current variable
  P_P_Variable[nVarID]->Get_Dimensions(&Dimensions);

  // If array output is being averaged, output format is XPM...
  if (m_bAveraging)
    {
      // Output XPM header for 16-level greyscale
	  
      *P_Value_Stream << "/* XPM */" << endl;
      // Output time
      *P_Value_Stream << setiosflags(ios::right + ios::fixed)
		      << setw(m_nColumnWidth)
		      << "/* TIME: "
		      << setprecision(m_nFPPrecision)
		      << m_flTime
		      << " */"
		      << endl << endl;
		
      *P_Value_Stream << "static char * array_xpm[] = {" << endl
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
    }
  else // Otherwise, output is simple ascii format using
    // Output header for simple ascii format (time only)
    *P_Value_Stream << setiosflags(ios::right + ios::fixed)
		    << setw(m_nColumnWidth)
		    << "TIME: " << setprecision(m_nFPPrecision)
		    << m_flTime << endl << endl;

      // Loop through all of the coordinates in each variable and output value
  for (nYCount = 0; nYCount < Dimensions.nY; nYCount++)
    {
      if (m_bAveraging)
	*P_Value_Stream << "\"";
      for (nXCount = 0; nXCount < Dimensions.nX; nXCount++)
	if (!m_bAveraging)
	  {
	    if (P_P_P_nValues[nVarID][nXCount][nYCount] == 0)
	      *P_Value_Stream << setw(ARRAY_OUTPUT_COLUMN_WIDTH)
			      << ARRAY_COLUMN_SEPARATOR
			      << ARRAY_OUTPUT_OFF_STRING;
	    else if (P_P_P_nValues[nVarID][nXCount][nYCount] == 1)
	      *P_Value_Stream << setw(ARRAY_OUTPUT_COLUMN_WIDTH)
			      << ARRAY_COLUMN_SEPARATOR
			      << ARRAY_OUTPUT_ON_STRING;
	    else
	      *P_Value_Stream << setw(ARRAY_OUTPUT_COLUMN_WIDTH) 
			      << ARRAY_COLUMN_SEPARATOR 
			      << P_P_P_nValues[nVarID][nXCount][nYCount];
	  }
	else{
	  sprintf(lpszValueOutput, "%lx",
		  (long) (((double) P_P_P_nValues[nVarID][nXCount][nYCount]
			   / (double) m_nNumSampled)
			  * (double) MAX_ARRAY_AVERAGE_VALUE));
	  *P_Value_Stream << lpszValueOutput;
	  if (nXCount == Dimensions.nX -1)
	    *P_Value_Stream << "\"";
					
	  /*	      *P_Value_Stream << ARRAY_COLUMN_SEPARATOR
		      << setiosflags(ios::right)
		      << setw(ARRAY_AVERAGE_OUTPUT_COLUMN_WIDTH)
		      << (long) (((double) P_P_P_nValues[nVarID][nXCount][nYCount]
		      / (double) m_nNumSampled)
		      * (double) MAX_ARRAY_AVERAGE_VALUE);
		      */
	}
      if(m_bAveraging)
	if(nYCount == Dimensions.nY - 1)
	  *P_Value_Stream << "};";
	else
	  *P_Value_Stream << ",";
      *P_Value_Stream << endl;
    }
  *P_Value_Stream << endl;
  P_Value_Stream->close();  // Close output file
  delete P_Value_Stream;
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Save_Coordinate_Values
*
* DESCRIPTION:	Saves values of specific coordinates within the variable
*               stored within the object by appending the values to an
*               output file.
*
* PARAMETERS:   NONE
*
* RETURNS:	Bool	TRUE		- Values were saved successfully.
*			FALSE		- An error occurred while saving
*					  the values.		
*
*************************************************************************/

Bool
C_Output_Variable_Array_Store::Save_Coordinate_Values(void)
{

  int             i;
  long		  nStart;
  int             nCountWC;
  C_Integer_Pair* P_WCList;
  ofstream*	  P_Value_Stream;

  P_Value_Stream = new ofstream;
	
  for (i = 0; i < m_nNumVariables; i++)
    {
      if (P_P_Variable[i]->Get_Num_Watch_Coordinates() > 0)
	{
	  nStart = clock();
	  // Repeatedly try to open file for appending output, until either
	  // the file opens successfully or maximum time limit is exceeded.
	  //  This is necessary in case another program (eg Excel) is
	  // opening file (for eg graphing)
	  do
	    P_Value_Stream -> open(m_lpszCoordFileName[i], ios::app);	
#if ( defined(_ALPHA) || defined(_AIX) )
	  while ((!P_Value_Stream->rdbuf()->is_open())
		 && ((clock() - nStart) / CLOCKS_PER_SEC
		     < RETRY_TIME_VALUE_FILE_OPEN));
#else
	  while ((!P_Value_Stream->is_open())
		 && ((clock() - nStart) / CLOCKS_PER_SEC
		     < RETRY_TIME_VALUE_FILE_OPEN));
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
	  // Ensure file was opened successfully (eventually)
	  if (P_Value_Stream == NULL)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 2,
			    "Array Variable Coordinate Values");
	      return FALSE;
	    }
		
	  P_WCList = new C_Integer_Pair [(P_P_Variable[i]
					  -> Get_Num_Watch_Coordinates())];
	  P_P_Variable[i]->Get_WCList(P_WCList);

	  // Output time
	  *P_Value_Stream << setiosflags(ios::right + ios::fixed)
			  << setw(m_nCoordColumnWidth)
			  << setprecision(m_nFPPrecision)
			  << m_flCoordTime;
		
	  for (nCountWC = 0;
	       nCountWC < P_P_Variable[i]->Get_Num_Watch_Coordinates();
	       nCountWC++)
	    {
	      *P_Value_Stream << VALUES_COLUMN_SEPARATOR 
			      << setiosflags(ios::right)
			      << setw(m_nCoordColumnWidth)
			      << P_P_P_nCoordValues[i][P_WCList[nCountWC].nX][P_WCList[nCountWC].nY];
	    }
	  *P_Value_Stream << endl;
	  P_Value_Stream->close();  // Close output file
	  delete[] P_WCList;
	}
    }
  delete P_Value_Stream;
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	This method clears up the memory used to store the variable
*				values and times
*
*************************************************************************/

C_Output_Variable_Array_Store::~C_Output_Variable_Array_Store(void)
{
  long i, j;
  C_Integer_Pair Dimensions;

  for (i = m_nNumVariables - 1; i >= 0; i--)
    {
      P_P_Variable[i]->Get_Dimensions(&Dimensions);
      for (j = Dimensions.nX - 1; j >= 0; j--)
	{
	  if(P_P_P_nValues[i][j] != NULL)
	    delete[] P_P_P_nValues[i][j];
	  if(P_P_P_nCoordValues[i][j] != NULL)
	    delete[] P_P_P_nCoordValues[i][j];
	}

      delete[]	P_P_P_nValues[i];
      delete[]	P_P_P_nCoordValues[i];
    }
//    for (i = 0; i < m_nNumVariables; i++)
//      {
//        P_P_Variable[i]->Get_Dimensions(&Dimensions);
//        for (j = 0; j < Dimensions.nX; j++)
//  	{
//  	  if(P_P_P_nValues[i][j] != NULL)
//  	    delete[] P_P_P_nValues[i][j];
//  	  if(P_P_P_nCoordValues[i][j] != NULL)
//  	    delete[] P_P_P_nCoordValues[i][j];
//  	}

//        delete[]	P_P_P_nValues[i];
//        delete[]	P_P_P_nCoordValues[i];
//      }
}
