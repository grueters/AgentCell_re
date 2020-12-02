/*************************************************************************
*
* FILENAME:	_OM_C.cpp
*
* CLASS:	C_Output_Manager_Complex
*
* DESCRIPTION:	This class organises the output of complex levels in the
*               system.  It stores all output variables and executes
*               output events at appropriate times during the simulation.
*               It is a descendant of C_Output_Manager.
*
*
* TYPE:		Core
*
*************************************************************************/


#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	This method initialises the output manager. All the
*		attributes are cleared.
*
* PARAMETERS:   C_Application*  p_App   - Pointer to parent application
*
*************************************************************************/

C_Output_Manager_Complex::C_Output_Manager_Complex
(C_Application* p_App, C_System* pSystem)
  : C_Output_Manager(p_App, pSystem)
{
  long i;

  P_Output_Variable_Disp_Store = NULL;
  P_Output_Variable_Dump_Store = NULL;
  for (i = 0; i < MAX_NUM_DUMP_VARIABLES; i ++)
    P_Dump_Var[i] = NULL;
  for (i = 0; i < MAX_NUM_DISPLAY_VARIABLES; i ++)
    P_Display_Var[i] = NULL;
  m_nStoreInterval = 0;
  m_nDumpStoreInterval = 0;
  m_nDisplayInterval = 0;
  m_nNumDumpVar = 0;
  m_nNumDisplayVar = 0;
  m_bAveraging = FALSE;
  m_nSampleInterval = 0;
  m_nStoreItCount = 0;
  m_nDumpStoreItCount = 0;
  m_nDisplayItCount = 0;
  m_nSampleItCount = 0;
}


/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	This method initialises the output manager's time-dependent
*               parameters.  Note that this method must be called after
*               the time increment has been optimised.
*
* PARAMETERS:	long  nTimeUnits - 0 for iterations; 1 for seconds
*				   
*		double flTimeInc - The size of the time increment being used.
*
*		double flNumIterations - The number of iterations in this
*                                        simulation.
*
* RETURNS:	Bool TRUE	 - Initialisation was successful.
*		     FALSE	 - Initialisation failed.
*
*************************************************************************/

Bool
C_Output_Manager_Complex::Init(long nTimeUnits, double flTimeInc,
			       unsigned long long nNumIterations)
{
  double flDisplayInterval;
  double flStoreInterval;
  double flDumpStoreInterval;
  double flSampleInterval;
  char   lpszArrayFile[MAX_INI_VALUE_LENGTH];
  double flNumDigitsPreDP;
  double flNumDigitsPstDP;
  long	 nAveraging;
  long	 nBinaryStore;
  Bool	 bBinaryStore;

  m_nNumIterations = nNumIterations;
  m_flSimDuration = nNumIterations * flTimeInc;

  // Check whether output should be binary
  bBinaryStore = FALSE;
  if (m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_BINARY_STORE,
		     &nBinaryStore, NULL, TRUE)
      && nBinaryStore == 1)
    {
      bBinaryStore = TRUE;
      m_pApp->Message(MSG_TYPE_STATUS,197);
    }

  // Check whether output should be averaged
  // (This information can be stored in the Array INI file to avoid the GUI)
  
  // First, look in the system INI file
  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_AVERAGED_OUTPUT,
		     &nAveraging, NULL, TRUE))
    {
      // Get name of Array INI file (for backwards compatibility)
      if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE,
			 lpszArrayFile, NULL, TRUE)
	  || strcmp(lpszArrayFile, "") == 0)
	{
	  // If the array file does not exist either, we are not averaging
	  nAveraging = 0;
	}
      else if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_AVERAGED_OUTPUT,
			      &nAveraging, lpszArrayFile, TRUE))
	{
	  // If the parameter does not exist, we are not averaging
	  nAveraging = 0;
	}
    }
  if (nAveraging != 0)
    {
      m_pApp->Message(MSG_TYPE_STATUS, 143);
      m_bAveraging = TRUE;
    }

  // Load the time-dependent parameters

  // Get the display interval
  if (!m_pApp->Get_INI(INI_SIMULATION_SECTION, INI_DISPLAY_INTERVAL,
		     &flDisplayInterval, NULL, TRUE))
    {
      // For backwards compatibility
      if (!m_pApp->Get_INI(INI_SIMULATION_SECTION, INI_DISPLAY_FREQ,
			 &flDisplayInterval))
	return FALSE;
    }

  // Get the store interval
  if (!m_pApp->Get_INI(INI_SIMULATION_SECTION, INI_STORE_INTERVAL,
		     &flStoreInterval, NULL, TRUE))
    {
      // For backwards compatibility
      if (!m_pApp->Get_INI(INI_SIMULATION_SECTION, INI_STORE_FREQ,
			 &flStoreInterval))
      return FALSE;
    }

  // Get the dump interval
  if (!m_pApp->Get_INI(INI_SIMULATION_SECTION, INI_DUMP_INTERVAL,
		     &flDumpStoreInterval, NULL, TRUE))
    {
      // Dump interval is zero by default, indicating dumps only at
      // beginning and end of simulation
      flDumpStoreInterval = 0;
    }

  // If we are averaging output...
  if (m_bAveraging)
    {
      // Load the length of the sampling interval for averaged variables 

      // First try the system INI file
      if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_SAMPLE_INTERVAL,
			 &flSampleInterval, NULL, TRUE))
	{
	  // Then check the Array INI file (for backwards compatibility)
	  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE,
			     lpszArrayFile))
	    return FALSE;
	  if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_SAMPLE_INTERVAL,
			     &flSampleInterval, lpszArrayFile, TRUE))
	    {
	      // For backwards compatibility
	      if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_SAMPLE_FREQ,
				 &flSampleInterval, lpszArrayFile))
		return FALSE;
	    }
	}
    }
  else
    {
      flSampleInterval = 0;
    }

  // If the unit of time is iterations, set the member variables directly;
  // otherwise convert using the time increment

  // If unit of time is iterations ...
  if (nTimeUnits == 0)
    {
      if (fabs(flDisplayInterval) > (double) MAX_UNSIGNED_LONG_LONG)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 40);
	  return FALSE;
	}
      if (fabs(flStoreInterval) > (double) MAX_UNSIGNED_LONG_LONG)				
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 41);
	  return FALSE;
	}
      if (fabs(flDumpStoreInterval) > (double) MAX_UNSIGNED_LONG_LONG)				
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 182);
	  return FALSE;
	}
      if (fabs(flSampleInterval) > (double) MAX_UNSIGNED_LONG_LONG)				
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 144);
	  return FALSE;
	}
      m_nDisplayInterval = (unsigned long long) flDisplayInterval;
      m_nStoreInterval = (unsigned long long) flStoreInterval;
      m_nDumpStoreInterval = (unsigned long long) flDumpStoreInterval;
      m_nSampleInterval = (unsigned long long) flSampleInterval;
    }
  else  // Otherwise, unit of time is seconds ...
    {
      if (fabs(flDisplayInterval / flTimeInc) > (double) MAX_UNSIGNED_LONG_LONG)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 40);
	  return FALSE;
	}
      if (fabs(flStoreInterval / flTimeInc) > (double) MAX_UNSIGNED_LONG_LONG)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 41);
	  return FALSE;
	}
      if (fabs(flDumpStoreInterval / flTimeInc) > (double) MAX_UNSIGNED_LONG_LONG)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 182);
	  return FALSE;
	}
      if (fabs(flSampleInterval / flTimeInc) > (double) MAX_UNSIGNED_LONG_LONG)				
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 144);
	  return FALSE;
	}
      // Convert to iterations (minimum 1 iteration)
      m_nDisplayInterval = MAX((unsigned long long) (flDisplayInterval / flTimeInc), 1);
      m_nStoreInterval = MAX((unsigned long long) (flStoreInterval / flTimeInc), 1);
      m_nSampleInterval = MAX((unsigned long long) (flSampleInterval / flTimeInc), 1);
      // A dump interval of zero signifies dumps only at start and end of sim
      if (flDumpStoreInterval == 0)
	{
	  m_nDumpStoreInterval = 0;
	  // Output message indicating only two dumps
	  m_pApp->Message(MSG_TYPE_STATUS, 184);
	}
      else
	{
	  m_nDumpStoreInterval
	    = MAX((unsigned long long) (flDumpStoreInterval / flTimeInc), 1);

	  // Format parameter list
	  ostringstream ParaListStream;
	  ParaListStream << m_nDumpStoreInterval;
	  ParaListStream << PARAMETER_LIST_SEPARATOR;
	  ParaListStream << setprecision(PREC_SIMULATION_TIME);
	  ParaListStream << fixed;
	  ParaListStream << m_nDumpStoreInterval * flTimeInc << ends;

	  // Output display frequency and size of interval
	  m_pApp->Message(MSG_TYPE_STATUS, 183, ParaListStream.str().c_str());
 
	}

    }

  // Format parameter list
  ostringstream ParaListStream;
  ParaListStream << m_nStoreInterval;
  ParaListStream << PARAMETER_LIST_SEPARATOR;
  ParaListStream << setprecision(PREC_SIMULATION_TIME) << fixed;
  ParaListStream << (m_nStoreInterval * flTimeInc) << ends;

  // Output display frequency and size of interval
  m_pApp->Message(MSG_TYPE_STATUS, 157, ParaListStream.str().c_str());

  // Validate time-dependent parameters

  if (m_nDisplayInterval <= 0) 
    {
      m_pApp->Message(MSG_TYPE_ERROR, 102);
      return FALSE;
    }
  if (m_nStoreInterval <= 0)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 103);
      return FALSE;
    }

  if (m_nSampleInterval <= 0)						
    {
      m_pApp->Message(MSG_TYPE_ERROR, 145);
      return FALSE;
    }


  // Calculate FP precision for times:
  // If time between storing each set of variables is eg 0.0123456
  // and simulation time is eg 50
  // Number of digits before decimal point = ceil(log10(50)) = 2
  // Number of digits after decimal point  = ceil(-log10(0.0123456)) + 3 = 5
  //   (extra 3 digits after most significant digit)
  // So times will be stored as XX.XXXXX

  // Calculate number of digits to left of decimal point at max time
  flNumDigitsPreDP = MAX(ceil(log10(flTimeInc * m_nNumIterations)), 1);
  // Calculate number of digits to right of decimal point for time increment
  flNumDigitsPstDP = (MAX(ceil(-log10(flTimeInc * m_nStoreInterval)), 0)
		      + PREC_FP_POST_DP);

  // Initialise objects to store variables in system
  // (must be done after reaction and time-dependent parameter initialisation)
  if (!Init_Variable_Store(flNumDigitsPreDP, flNumDigitsPstDP, bBinaryStore))
    {
      m_pApp->Message(MSG_TYPE_ERROR, 97);
      return FALSE;
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
C_Output_Manager_Complex::Handle_Output(double flCurrentTime)
{

#ifdef _DYNAMIC_UPDATE
  long			i;
  m_nSampleItCount ++;
  // If we are averaging output
  if (m_bAveraging)
    {
      // If display values are to be sampled for averaging in this iteration
      if (m_nSampleItCount == m_nSampleInterval)
	{
	  // Loop through all the display variables and update
	  for(i=0; i < m_nNumDisplayVar; i++)
	    P_Display_Var[i]->Update();
	  m_nSampleItCount = 0;
	}
    }
#endif /* _DYNAMIC_UPDATE */

  // Increment iteration counters
  m_nStoreItCount ++;
  m_nDumpStoreItCount ++;
  m_nDisplayItCount ++;

  // If we have reached an iteration in which
  // the variable values must be stored...
  if (m_nStoreItCount == m_nStoreInterval)
    {
      
#ifdef _NSDEBUG
      ofstream Debug_Stream;
      ostringstream DBTimeStream;
      long     nStart;
      
      // Start clock running
      nStart = clock();
      // Repeatedly try to open file for appending output,
      // until either the file opens successfully or maximum time
      // limit is exceeded. This is necessary in case another program
      // (eg Excel) is opening file (for eg graphing)
      do
	Debug_Stream.open("NSDEBUG.txt",
			  ios::app);
      while ((!Debug_Stream.is_open())
	     && ((clock() - nStart) / CLOCKS_PER_SEC
		 < RETRY_TIME_VALUE_FILE_OPEN));
      // Ensure file was opened successfully (eventually)
      if (Debug_Stream == NULL)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 2, "NSDEBUG");
	  return FALSE;
	}
      Debug_Stream << P_Parent->EQUIL_COUNT << endl;
      P_Parent->EQUIL_COUNT = 0;
      DBTimeStream << flCurrentTime << TAB_CHAR << std::ends;

      Debug_Stream << DBTimeStream.str();
      Debug_Stream.close();
#endif

#ifndef _DYNAMIC_UPDATE
      // Calculate the current levels of each complex type
      P_Parent -> Calc_Complex_Type_Levels();
#endif
#ifdef _DYNAMIC_UPDATE
      // If we ARE NOT averaging output, update variable here
      if (!m_bAveraging)
	for(i=0; i < m_nNumDisplayVar; i++)
	  {
	    P_Display_Var[i]->Reset();
	    P_Display_Var[i]->Update();
	  }
#endif /* _DYNAMIC_UPDATE */

      // If there is an error storing the display variable values,
      // exit
      if (!P_Output_Variable_Disp_Store
	  -> Store(flCurrentTime))
	return FALSE;

#ifdef _DYNAMIC_UPDATE
      // If we ARE averaging output, update variable here
      if (m_bAveraging)
	for(i=0; i < m_nNumDisplayVar; i++)
	  {
	    P_Display_Var[i]->Reset();
	    P_Display_Var[i]->Update();
	  }
#endif /* _DYNAMIC_UPDATE */

      // Reset iteration counter
      m_nStoreItCount = 0;
    }

    // If we are dumping variable values and have reached an
    // iteration in which the variable values must be dumped...
    if (P_Output_Variable_Dump_Store != NULL
	&& m_nDumpStoreItCount == m_nDumpStoreInterval)
      {
#ifndef _DYNAMIC_UPDATE
      // Calculate the current levels of each complex type
	// Calculate the current levels of each complex type
	P_Parent -> Calc_Complex_Type_Levels();
#endif /* _DYNAMIC_UPDATE */
	
	// If there was an error storing the dump variable values, exit
	if (!(P_Output_Variable_Dump_Store
	      -> Store(flCurrentTime)))
	  return FALSE;
	// Reset iteration counter
	m_nDumpStoreItCount = 0;
      }

  // If we have reached an iteration in which
  // the variable values must be displayed...
  if (m_nDisplayItCount == m_nDisplayInterval)
    {
#ifndef _DYNAMIC_UPDATE
      // Calculate the current levels of each complex type
      P_Parent -> Calc_Complex_Type_Levels();
#endif
      m_pApp->Display_Variable_Values(flCurrentTime,
				    (C_Output_Variable**) P_Display_Var,
				    100 * flCurrentTime / m_flSimDuration,
				    m_nNumDisplayVar);
      m_nDisplayItCount = 0;
    }
 
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	This method clears up all the memory allocated for the
*		output manager
*
*************************************************************************/

C_Output_Manager_Complex::~C_Output_Manager_Complex(void)
{
  long			i;
  // Delete object used to store display variable values
  if (P_Output_Variable_Disp_Store != NULL)
    delete P_Output_Variable_Disp_Store;
  // Delete object used to store dump variable values
  if (P_Output_Variable_Dump_Store != NULL)
    delete P_Output_Variable_Dump_Store;
  // Delete dump variables
  for (i = 0; i < MIN(m_nNumDumpVar, MAX_NUM_DUMP_VARIABLES); i ++)
    if (P_Dump_Var[i] != NULL)
      delete P_Dump_Var[i];
  // Delete display variables
  for (i = 0; i < MIN(m_nNumDisplayVar, MAX_NUM_DISPLAY_VARIABLES); i ++)
    if (P_Display_Var[i] != NULL)
      delete P_Display_Var[i];
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
C_Output_Manager_Complex::Add_DumpVariable(C_Output_Variable* pNewVar)
{
  // Ensure that too many dump variables have not been allocated
  if (m_nNumDumpVar >= MAX_NUM_DUMP_VARIABLES)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 57);
      return FALSE;
    }
  // Insert new variable at the end of the list
  P_Dump_Var[m_nNumDumpVar] = (C_Output_Variable_Complex*) pNewVar;
  // Update the number of dump variables
  m_nNumDumpVar ++;

  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Add_DisplayVariable
*
* DESCRIPTION:	This method adds one member to the output manager's list
*               of display variables. (Used at initialisation time)
*
* PARAMETERS:	C_Output_Variable* pNewVar - Pointer to newly
*                                            created variable.
*
*************************************************************************/

Bool
C_Output_Manager_Complex::Add_DisplayVariable(C_Output_Variable* pNewVar)
{
  // Ensure that too many snapshot variables have not been allocated
  if (m_nNumDisplayVar >= MAX_NUM_DISPLAY_VARIABLES)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 99);
      return FALSE;
    }
  // Insert new variable at the end of the list
  P_Display_Var[m_nNumDisplayVar] = (C_Output_Variable_Complex*) pNewVar;
  // Update the number of display variables
  m_nNumDisplayVar ++;

  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Init_Variable_Store
*
* DESCRIPTION:	Initialises the objects used to store variables.
*		Two sets of variables are stored: dump variables (a complete
*		set of all possible variables) and display variables
*		(a set of variables specified in an INI file).
*		A check is made to see if the variable values can be stored
*		in memory during the simulation. Also calculates
*		the precision necessary for storing floating point
*		numbers (times) based on the time incremements and
*		simulation time.
*
* RETURNS:	Bool	TRUE	- Objects were created successfully
*			FALSE	- An error occurred creating objects
*
*************************************************************************/

Bool
C_Output_Manager_Complex::Init_Variable_Store(double flNumDigitsPreDP, double flNumDigitsPstDP, Bool bBinaryStore)
{
  long	 nNumValuesToStore;
  long	 nCreateDumpFile;
  Bool	 bMemoryStore;
  char	 lpszFileName[MAX_INI_VALUE_LENGTH];

  // Calculate how many values need storing

  // Calculate the number of time points that will need storing
  nNumValuesToStore = (long) (m_nNumIterations / m_nStoreInterval) + 2;

  // Do not allow storage of output variable values in memory
  bMemoryStore = FALSE;

  // If required, values of output variables can be stored in memory
  // for processing (eg regression calculation, moving average output etc).
  // In this implementation, we will avoid storing the values of output
  // variables in memory to reduce memory over-head.
  // To allow memory storage, uncomment the following lines:
  //
  //	bMemoryStore = TRUE;	// Assume variables will be stored in memory
  //    // Check sufficient memory has been allocated to store the number
  //    // of values specified
  //	if (nNumValuesToStore > MAX_NUM_VALUES_STORED)
  //	  {
  //	    // If there isn't, output message
  //	    m_pApp->Message(MSG_TYPE_STATUS, 42, 1, nNumValuesToStore);
  //        // And indicate variables should not be stored in memory
  //	    bMemoryStore = FALSE;
  //	  }
	
  // Attempt to create object to store the display variable values
  // at different time intervals

  // Get output file name from INI file
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_VALUES_OUT, lpszFileName))
    return FALSE;
//  //  AgentCell / REPAST modification of file names
//  char runIDString[128], *cPos;
//  int i;
//  for (i = 0; i < MAX_NUM_RUN_IDS; i++) runIDString[i] = '0';
//  if (strcmp(cPos=strrchr(lpszFileName,'.'),".OUT")==0){*cPos='\0';}
//  if (m_pApp->m_nRunID > 0)
//    {sprintf(runIDString, "%d", m_pApp->m_nRunID);}
//  else
//    {runIDString[0]='\0';}
//  strcat(lpszFileName, runIDString);
//  strcat(lpszFileName, ".OUT");
  
  P_Output_Variable_Disp_Store
    = new C_Output_Variable_Store(m_pApp, (C_Output_Variable**) P_Display_Var,
				  m_nNumDisplayVar, lpszFileName, bMemoryStore,
				  flNumDigitsPreDP, flNumDigitsPstDP,
				  bBinaryStore);
  if (P_Output_Variable_Disp_Store == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8, "new Display Variable Store");
      return FALSE;
    }
  if (!P_Output_Variable_Disp_Store->Init(nNumValuesToStore))
    return FALSE;

  // Check if dump files for complexes need to be created...

  // Retrieve create dump file flag from INI file
  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_CREATE_DUMP_FILE,
		     &nCreateDumpFile))
    return FALSE;
  // If no dump file is going to be created,
  // return without setting up the dump file
  if (!nCreateDumpFile)
    {
      m_nDumpStoreInterval = 0;
      return TRUE;
    }

  // Attempt to create object to store the dump variable values
  // at different time intervals

  // Get output file name from INI file
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_DUMP_OUT, lpszFileName))
    return FALSE;
  
  P_Output_Variable_Dump_Store
    = new C_Output_Variable_Store(m_pApp, (C_Output_Variable**) P_Dump_Var,
				  m_nNumDumpVar, lpszFileName, bMemoryStore,
				  flNumDigitsPreDP, flNumDigitsPstDP);
  if (P_Output_Variable_Dump_Store == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8, "new Dump Variable Store");
      return FALSE;
    }
  if (!P_Output_Variable_Dump_Store->Init(nNumValuesToStore))
    return FALSE;

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
C_Output_Manager_Complex::Display_And_Store(double flTime)
{
  /*#ifndef _DYNAMIC_UPDATE*/
  // Calculate the current levels of each complex type
  P_Parent -> Calc_Complex_Type_Levels();
  /*#endif*/
#ifdef _DYNAMIC_UPDATE
      for(int j=0; j < m_nNumDisplayVar; j++)
	{
	  P_Display_Var[j]->Update();
	}
#endif /* _DYNAMIC_UPDATE */

  m_pApp->Display_Variable_Values(flTime,
				(C_Output_Variable**) P_Display_Var,
				100 * flTime / m_flSimDuration,
				m_nNumDisplayVar);

  if (!P_Output_Variable_Disp_Store -> Store(flTime))
    {
      return FALSE;
    }

  // If we are storing dump variables, try to store them...
  if (P_Output_Variable_Dump_Store != NULL)
    {
      // If there was an error storing the dump variable values, exit
      if (!(P_Output_Variable_Dump_Store -> Store(flTime)))
	{
	  return FALSE;
	}
    }
  return TRUE;
}
