/*************************************************************************
*
* FILENAME:	OVStore.cpp
*
* CLASS:	C_Output_Variable_Store
*
* DESCRIPTION:	This class stores the values of all the variables in
*		the system. The C_Output_Variable objects are responsible
*		for maintaining their current values. This class stores
*		these values at certain times and saves the values in a file.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"
#include "Binfstream.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Sets member variables and resets resets all the stored
*		values for all the variables to zero. Sets the minimum
*		column width based on the floating point accuracy required.
*
* PARAMETERS:   C_Application*  p_App  	        - Pointer to parent application
*
*       	C_Output_Variable** P_P_OV_System  - Pointer to array of
*						     variable pointers.
*		long		  nNumVariables	   - Number of variables
*						     in the system.
*		char*		  lpszFileName	   - Name of file to store
*						     the values in.
*		Bool		  bMemoryStore	   - Flag indicating if
*						     values should be stored
*						     in memory as well.
*		double		  flNumDigitsPreDP - Num digits before decimal
*						     point in float point
*						     number (times).
*		double		  flNumDigitsPstDP - Num digits after decimal
*						     point in float point
*						     number (times).
*		Bool		  bBinaryStore    - Flag indicating if
*						     values should be saved
*						     in binary format.
*						     (default=FALSE)
*
*************************************************************************/

C_Output_Variable_Store::C_Output_Variable_Store
(C_Application* p_App, C_Output_Variable** P_P_OV_System, long nNumVariables,
 char* lpszFileName, Bool bMemoryStore, double flNumDigitsPreDP, 
 double flNumDigitsPstDP, Bool bBinaryStore)
{
  int i;

  // Initialise data members

  m_pApp = p_App;

  m_bMemoryStore = bMemoryStore;
  m_bBinaryStore = bBinaryStore;
  // Indicate that no values have been stored yet
  m_nNumValuesStored = 0;
  // Set number of variables to be stored
  m_nNumVariables = nNumVariables;
  // Set private pointer to variables array
  P_P_Variable = P_P_OV_System;
  // Save name of file as member variable
  strcpy(m_lpszValuesFileName, p_App->m_lpszWorkingDir);
  strcat(m_lpszValuesFileName, FILE_PATH_SEPARATOR);
  strcat(m_lpszValuesFileName, lpszFileName);
  // Set floating point precision (number of digits after decimal point)
  m_nFPPrecision = (int) flNumDigitsPstDP;
  // Set minimum column width based on number of FP accuracy
  m_nColumnWidth = (int) MAX(flNumDigitsPstDP + flNumDigitsPreDP + 1,
			     MIN_COLUMN_WIDTH);

  // Reset all pointers
  m_pflTime = NULL;
  for (i = 0; i < MAX_NUM_DUMP_VARIABLES; i ++)
    m_pnValue[i] = NULL;
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
* PARAMETERS:	long nNumValuesToStore  - Maximum number of values that
*					  will be stored.
* RETURNS:	Bool	TRUE		- Object initialised successfully.
*			FALSE		- An error occurred while 
*					  initialising this object.
*
*************************************************************************/

Bool
C_Output_Variable_Store::Init(long nNumValuesToStore)
{
  char		lpszParaList[MAX_MESSAGE_LENGTH];
  // For storing header for binary output
  char          lpszBinHeader[BINARY_HEADER_TEXT_LENGTH];
  int           i;
  int           j;

  // Allocate memory for storing values

  // If we're not storing values in memory, only disk
  if (!m_bMemoryStore)
    // Set parameter so we only allocate memory for one set of variables
    nNumValuesToStore = 1;

  // Create array for storing times for different sets of variable values
  m_pflTime = new double[nNumValuesToStore];
  // If the array was not created correctly...
  if (m_pflTime == NULL)
    {
      // Output error
      m_pApp->Message(MSG_TYPE_ERROR, 8, "Output Variable times");
      return FALSE;
    }
  // Reset time values
  for (i = 0; i < nNumValuesToStore; i ++)
    m_pflTime[i] = 0;

  // Loop through each variable we want to store
  for (i = 0; i < m_nNumVariables; i++)
    {
      // Create array to store all values for this variable at different times
      m_pnValue[i] = new long[nNumValuesToStore];
      // If the array was not created correctly...
      if (m_pnValue[i] == NULL)
	{
	  // Output error
	  m_pApp->Message(MSG_TYPE_ERROR, 8, "Output Variable values");
	  return FALSE;
	}
      // Reset variable values
      for (j = 0; j < nNumValuesToStore; j ++)
	(m_pnValue[i]) [j] = 0;
    }

  // Initialise output stream

  // If output is binary...
  if(m_bBinaryStore)
    {
      // Open output file
      Binfstream Value_Stream(m_lpszValuesFileName,ios::out);
      // Ensure no errors occurred opening file
      if (!Value_Stream)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 2, "Complex Type Variable Values");
	  return FALSE;
	}
      // Output column headers to output stream
      sprintf(lpszBinHeader,
	      "%%StochSim-%d.%d binary variable output format %d %d",
	      STOCHSIM_VERSION_MAJOR, STOCHSIM_VERSION_MINOR,
	      BINARY_HEADER_TEXT_LENGTH, BINARY_COLUMN_TITLE_LENGTH);
      Value_Stream.write(lpszBinHeader, BINARY_HEADER_TEXT_LENGTH);
      Value_Stream.write(m_nNumVariables);
      // Loop through each variable, and keep track of the widest column
      for (i = 0; i < m_nNumVariables; i++)
	if (strlen(P_P_Variable[i]->Get_Name()) > BINARY_COLUMN_TITLE_LENGTH)
	  {
	    // Format message and output error
	    sprintf(lpszParaList, "%s%s%d", P_P_Variable[i]->Get_Name(),
	      PARAMETER_LIST_SEPARATOR, BINARY_COLUMN_TITLE_LENGTH);
	    m_pApp->Message(MSG_TYPE_ERROR, 196, lpszParaList);
	    return FALSE;
	  }
      Value_Stream.write((long) BINARY_COLUMN_TITLE_LENGTH);
      Value_Stream.write(TIME_COLUMN_TITLE, BINARY_COLUMN_TITLE_LENGTH);
      // Loop through all the variables and display their names;
      // this will form the column headings
      for (i = 0; i < m_nNumVariables; i++)
	Value_Stream.write(P_P_Variable[i]->Get_Name(),
			   BINARY_COLUMN_TITLE_LENGTH);
      Value_Stream.close();  // Close output file
    }
  else // Otherwise, output is in ascii format (default)
    {
      // Open output file
      ofstream Value_Stream(m_lpszValuesFileName);
      // Ensure no errors occurred opening file
      if (!Value_Stream)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 2, "Complex Type Variable Values");
	  return FALSE;
	}

      // Output column headers to output stream

      // Loop through each variable, and keep track of the widest column
      for (i = 0; i < m_nNumVariables; i++)
	m_nColumnWidth
	  = MAX(m_nColumnWidth, (signed) strlen(P_P_Variable[i]->Get_Name()));
      Value_Stream << setiosflags(ios::right) << setw(m_nColumnWidth)
		   << TIME_COLUMN_TITLE;
      // Loop through all the variables and display their names;
      // this will form the column headings
      for (i = 0; i < m_nNumVariables; i++)
	{
	  Value_Stream << VALUES_COLUMN_SEPARATOR;
	  for (j = 0;
	       j < (m_nColumnWidth - (signed) strlen(P_P_Variable[i]->Get_Name()));
	       j++)
	    // Right align column headings by left padding them
	    // with spaces and then output name
	    Value_Stream << SPACE_STR;
	  Value_Stream << P_P_Variable[i]->Get_Name();
	}
      Value_Stream << endl;
      Value_Stream.close();  // Close output file
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
C_Output_Variable_Store::Store(double flCurrentTime)
{
  int i;
  // Ensure there is sufficient memory to store a new value
  if (m_nNumValuesStored >= MAX_NUM_VALUES_STORED)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 1);
      return FALSE;
    }
  // Loop through each of the variables in the system and store its value
  for (i = 0; i < m_nNumVariables; i ++)
    {
      m_pnValue [i] [m_nNumValuesStored] = P_P_Variable[i] -> Get_Value();
    }
  // Store the current simulation time
  m_pflTime[m_nNumValuesStored] = flCurrentTime;
  if(m_bBinaryStore)
    {
      if (!Save_Values_Bin(m_nNumValuesStored))
	return FALSE;
    }
  else
    {
      if (!Save_Values(m_nNumValuesStored))
	return FALSE;
    }
  // If variables are being stored in memory
  if (m_bMemoryStore)
    // Move to next variable storage area so these values
    // will not be overwritten next time the variables are stored
    m_nNumValuesStored ++;
  return TRUE;

  /* (CARL FIRTH 1998)
    // The following code can be used to simulate the thresholding model of the model

    static double flStartRunTime = 0;
    static Bool bRunning = FALSE;
    static ofstream IntervalsFile;
    if (bRunning && (P_P_Variable[0] -> Get_Value() > 1400))
    {
    double flDuration;
    flDuration = flCurrentTime - flStartRunTime;
    IntervalsFile.open("Intervals.out", ios::app);
    IntervalsFile << flDuration << endl;
    IntervalsFile.close();
    bRunning = FALSE;
    }
    if (!bRunning && (P_P_Variable[0] -> Get_Value() <= 1400))
    {
    bRunning = TRUE;
    flStartRunTime = flCurrentTime;
    }
  */
}


/*************************************************************************
*
* METHOD NAME:	Save_Values
*
* DESCRIPTION:	Saves a single set of variable values stored within 
*		the object by appending the values to an output file.
*
* PARAMETERS:	long	nValueNumToSave - The number of the set of values
*					  stored within the object to be
*					  save to a file.
*
* RETURNS:	Bool	TRUE		- Values were saved successfully.
*			FALSE		- An error occurred while saving
*					  the values.		
*
*************************************************************************/

Bool
C_Output_Variable_Store::Save_Values(long nValueNumToSave)
{
  int		i;
  long		nStart;
  ofstream*	P_Value_Stream;

  P_Value_Stream = new ofstream;
  // Start clock running
  nStart = clock();

  // Repeatedly try to open file for appending output, until either the file
  // opens successfully or maximum time limit is exceeded. This is necessary
  // in case another program (eg Excel) is opening file (for eg graphing)
  do
    P_Value_Stream -> open(m_lpszValuesFileName, ios::app);	
#if ( defined(_ALPHA) || defined(_AIX) )
  while ((!P_Value_Stream->rdbuf()->is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#else
  while ((!P_Value_Stream->is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#endif /*  ( defined(_ALPHA) || defined(_AIX) ) */
  // Ensure file was opened successfully (eventually)
  if (P_Value_Stream == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 2, "Complex Type Variable Values");
      return FALSE;
    }
  // Output time
  *P_Value_Stream << setiosflags(ios::right | ios::fixed)
		  << setw(m_nColumnWidth) << setprecision(m_nFPPrecision)
		  << m_pflTime [nValueNumToSave];
  // Loop through the variables and display their values
  for (i = 0; i < m_nNumVariables; i ++)
    *P_Value_Stream << VALUES_COLUMN_SEPARATOR << setiosflags(ios::right)
		    << setw(m_nColumnWidth) << m_pnValue[i] [nValueNumToSave];
  *P_Value_Stream << endl;
  P_Value_Stream->close();  // Close output file
  delete P_Value_Stream;
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Save_Values_Bin
*
* DESCRIPTION:	Saves a single set of variable values stored within 
*		the object by appending the values to a BINARY output file.
*
* PARAMETERS:	long	nValueNumToSave - The number of the set of values
*					  stored within the object to be
*					  save to a file.
*
* RETURNS:	Bool	TRUE		- Values were saved successfully.
*			FALSE		- An error occurred while saving
*					  the values.		
*
*************************************************************************/

Bool
C_Output_Variable_Store::Save_Values_Bin(long nValueNumToSave)
{
  int		i;
  long		nStart;
  
  Binfstream P_Value_Stream;
  // Start clock running
  nStart = clock();

  // Repeatedly try to open file for appending output, until either the file
  // opens successfully or maximum time limit is exceeded. This is necessary
  // in case another program (eg Excel) is opening file (for eg graphing)
  do
    P_Value_Stream.open(m_lpszValuesFileName, ios::out | ios::app | ios::binary);	
#if ( defined(_ALPHA) || defined(_AIX) )
  while ((!P_Value_Stream.rdbuf()->is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#else
  while ((!P_Value_Stream.is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
  // Ensure file was opened successfully (eventually)
  if (!P_Value_Stream)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 2, "Complex Type Variable Values");
      return FALSE;
    }
  // Output time
  P_Value_Stream.write(m_pflTime[nValueNumToSave]);
  // Loop through the variables and display their values
  for (i = 0; i < m_nNumVariables; i ++)
    P_Value_Stream.write(m_pnValue[i] [nValueNumToSave]);
  P_Value_Stream.close();  // Close output file
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
C_Output_Variable_Store::Update(void)
{
  int i;
  for (i = 0; i < m_nNumVariables; i ++)
    {
      P_P_Variable[i]->Update();
    }
}


/*************************************************************************
*
* METHOD NAME:	Reset
*
* DESCRIPTION:	Resets the value of all variables in this store.
*
* PARAMETERS:	NONE
*
* RETURNS:	NONE
*
*************************************************************************/

void
C_Output_Variable_Store::Reset(void)
{
  int i;
  for (i = 0; i < m_nNumVariables; i ++)
    {
      P_P_Variable[i]->Reset();
    }
}

/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	This method clears up the memory used to store the variable
*		values and times
*
*************************************************************************/

C_Output_Variable_Store::~C_Output_Variable_Store(void)
{
  long nCount;
  if (m_pflTime != NULL)
    delete[] m_pflTime;
  for (nCount = 0; nCount < m_nNumVariables; nCount ++)
    if (m_pnValue[nCount] != NULL)
      delete[] m_pnValue[nCount];
}
