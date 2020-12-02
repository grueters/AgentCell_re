/*************************************************************************
*
* FILENAME:	System1.cpp
*
* CLASS:	C_System (see also System2.cpp)
*
* DESCRIPTION:	This class represents a simulation. It contains all the
*		components, complex type, complexes and reactions
*		necessary to fully define a reaction system. To run
*		the simulation, a method within this class is called
*		(C_System::Run). This file only contains the C_System
*		methods involved in initiation (ie the C_System constructor,
*		initialisor	and the methods called by them).
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"

/*************************************************************************
*
* METHOD NAME:	Constructor (standard)
*
* DESCRIPTION:	This is the standard constructor, which allows the rest
*               of the code to reference the parent application, and resets
*               all attributes.
*
* PARAMETERS:   C_Application*  p_App     - Pointer to parent application
*
*************************************************************************/

C_System::C_System(C_Application* p_App)
{
  long i;
  m_pApp = p_App;
  P_R_M = NULL;
  P_Complex = NULL;
  setlocale(LC_NUMERIC,"C");
  // Loop through every complex type...
  for (i = 0; i < MAX_COMPLEX_TYPES; i++)
    P_Complex_Type[i] = NULL;
  P_Free_Complex = NULL;
  P_Ran_Gen = NULL;
  P_Event_Manager = NULL;
  m_flReactionVolume = 0;
  m_flTimeInc = 0;
  m_nNumIterations = 0;
  m_nIterationsDone = 0;
  m_nMaxNumComplexes = 0;
  m_nNumUniPseudoComplexes = 0;
  m_nNumComponents = 0;
  m_nNumComplexTypes = 0;
  m_bUsingSpatialExtensions = FALSE;
  m_bUsingArrays = FALSE;
  m_nNumOutputManagers = 0;

  for (i = 0; i < MAX_NUM_OUTPUT_MANAGERS; i++)
    P_Output_Manager[i] = NULL;

  P_Complex_Array = new C_Complex_Array* [MAX_NUM_COMPLEX_ARRAYS];
  m_pnArrayEquilInterval = new unsigned long long [MAX_NUM_COMPLEX_ARRAYS];
  m_pnArrayEquilItCount = new unsigned long long [MAX_NUM_COMPLEX_ARRAYS];
  for (i = 0; i < MAX_NUM_COMPLEX_ARRAYS; i++)
  {
    P_Complex_Array[i] = NULL;
    m_pnArrayEquilInterval[i] = 0;
    m_pnArrayEquilItCount[i] = 0;
  }
}


/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	This method initialises the system. Various initialisation
*		parameters are retrieved from the system INI file and the
*		following objects are created:
*			Free complex stack
*			Complex types
*			Special complex types
*                       Neighbour-sensitive complex types (if being used)
*			Complexes
*			Reaction matrix
*                       Complex arrays (if being used)
*			Output managers
*			Output variables
*			Random number generator
*
* RETURNS:	Bool	TRUE	- System was initialised successfully
*			FALSE	- An error occurred initialising system
*
*************************************************************************/

Bool
C_System::Init(void)
{
  long		nOptimiseTimeInc;
  long		nRanNumGen;
  long		nTimeUnits;
  long		nCount;
  long		nNumDynReactions;
  double	flSimDuration;
  double	flTest;
  double	flOrigRateK[MAX_NUM_EVENTS];
  char		lpszParaList[MAX_MESSAGE_LENGTH];
  ostringstream ParaListStream;
  char		lpszDynCode[MAX_NUM_EVENTS][MAX_INI_VALUE_LENGTH];
  time_t	tStart;
  time_t	tEnd;
  C_Reaction*	P_Dynamic_Reactions[MAX_NUM_EVENTS];
  long          nUseSpatialExtensions;

  // Get initialisation settings from INI file
  // (see INI file for details of each parameter).
  // At this point, only the non-time related parameters are loaded;
  // the time-dependent parameters are loaded later.
  // (This is because the time increment may be optimised and changed
  // when initialising the reactions).

  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_RAN_NUM_GEN, &nRanNumGen))
    return FALSE;
  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_TIME_UNITS, &nTimeUnits))
    return FALSE;
  if (!m_pApp->Get_INI(INI_SIMULATION_SECTION, INI_REACTION_VOLUME,
		     &m_flReactionVolume))
    return FALSE;
  if (!m_pApp->Get_INI(INI_SIMULATION_SECTION, INI_TIME_INCREMENT, &m_flTimeInc))
    return FALSE;
  if (!m_pApp->Get_INI(INI_SIMULATION_SECTION, INI_MAX_NUM_COMPLEXES,
		     &m_nMaxNumComplexes))
    return FALSE;
  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_OPTIMISE_TIME_INC,
		     &nOptimiseTimeInc))
    return FALSE;

  // This parameter is optional, so don't return FALSE even if it's not there!
  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_USE_SPATIAL_EXTENSIONS,
		     &nUseSpatialExtensions, NULL, TRUE))
    {
      m_bUsingSpatialExtensions = FALSE;
    }
  else if (nUseSpatialExtensions == 1)
    {
      m_bUsingSpatialExtensions = TRUE;
    }
  // If value is neither 0 or 1, the value is invalid
  else if (nUseSpatialExtensions != 0)
    {
      // Format message and output error
      sprintf(lpszParaList, "%ld%s%s%s%s", nUseSpatialExtensions,
	      PARAMETER_LIST_SEPARATOR, INI_USE_SPATIAL_EXTENSIONS,
	      PARAMETER_LIST_SEPARATOR, INI_OPTIONS_SECTION);
      m_pApp->Message(MSG_TYPE_ERROR, 152, lpszParaList);
    }

  // Validate parameters

  if (m_flReactionVolume <= 0)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 38);
      return FALSE;
    }
  // If time increment is not being optimised and time increment is invalid,
  // output error
  if ((nOptimiseTimeInc == 0) && (m_flTimeInc <= 0))
    {
      m_pApp->Message(MSG_TYPE_ERROR, 39);
      return FALSE;
    }
  // If we're optimising time increment, set time increment to default value
  if (nOptimiseTimeInc == 1)
    m_flTimeInc = 1;
  // Check if maximum number of complexes defined in INI file is valid
  if (m_nMaxNumComplexes <= 0)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 43);
      return FALSE;
    }
  // Add two to maximum complexes to allow for creation of extra complexes
  // DURING a reaction (max two substrates)
  m_nMaxNumComplexes += 2;

  // Ensure maximum number of complexes in simulation does not
  // exceed memory constraints
  if (m_nMaxNumComplexes > MAX_LIMIT_COMPLEXES)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 44);
      return FALSE;
    }

  // Create array to store complex objects
  P_Complex = new C_Complex* [m_nMaxNumComplexes];

  // If memory allocation failed
  if (P_Complex == NULL)
    {
      m_pApp->Message(MSG_TYPE_STATUS, 8, "Complexes");  // Output error
      return FALSE;
    }

  // Loop through each complex pointer
  for (nCount = 0; nCount < m_nMaxNumComplexes; nCount ++)
    P_Complex[nCount] = NULL;  // Set pointer to NULL
	
  // Create event manager
  P_Event_Manager = new C_Event_Manager(m_pApp);

  // Create output manager for complexes
  P_Output_Manager[OM_COMPLEX] = new C_Output_Manager_Complex(m_pApp, this);
  if (P_Output_Manager[OM_COMPLEX] == NULL)
    {
      // Ouput error if creation failed
      m_pApp->Message(MSG_TYPE_ERROR, 154);
      return FALSE;
    }
  m_nNumOutputManagers ++;

  // Create random number generator object based on
  // the generator type specified in the INI file

  switch (nRanNumGen)
    {
    case 1:	P_Ran_Gen = new C_Random_Generator_Dummy(m_pApp);
      break;
    case 2:	P_Ran_Gen = new C_Random_Generator_Internal(m_pApp);
      break;
    case 3:	P_Ran_Gen = new C_Random_Generator_Shuffle(m_pApp);
      break;
    case 4:	P_Ran_Gen = new C_Random_Generator_Bit_String(m_pApp);
      break;
    case 5:	P_Ran_Gen = new C_Random_Generator_Quick(m_pApp);
      break;
    case 6:	P_Ran_Gen = new C_Random_Generator_Shuffle2(m_pApp);
      break;
    case 7:	P_Ran_Gen = new C_Random_Generator_MT(m_pApp);
      break;
    default:m_pApp->Message(MSG_TYPE_ERROR, 49);
      return FALSE;
    }
  
	
  // Initialise random number generator and ask generator to identify itself

  if (!P_Ran_Gen->Init())
    return FALSE;
  m_pApp->Message(MSG_TYPE_STATUS, 72, P_Ran_Gen->Identify());


  // If we are using spatial extensions, initialise them first
  if (m_bUsingSpatialExtensions)
    {
      m_pApp->Message(MSG_TYPE_STATUS, 155);
      if (!Init_Spatial_Extensions())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 151);
	  return FALSE;
	}
    }

  // Attempt to create object (stack) to store the empty complex objects
  // (must be done before reactions are initialised)

  P_Free_Complex = new C_Complex_Stack(m_pApp);
  if (!P_Free_Complex->Init(m_nMaxNumComplexes))
    return FALSE;

  // Initialise components in system
  if (!Init_Components())
    {
      m_pApp->Message(MSG_TYPE_ERROR, 45);
      return FALSE;
    }


  // Initialise complex types in system
  if (!Init_Complex_Types())
    {
      m_pApp->Message(MSG_TYPE_ERROR, 46);
      return FALSE;
    }

  // Initialise special complex types in system
  // (further initialisation is done after the reactions matrix is created)
  if (!Init_Complex_Types_Special())
    {
      m_pApp->Message(MSG_TYPE_ERROR, 79);
      return FALSE;
    }


  // Initialise neighbour_sensitive complex types in system
  // (further initialisation is done after the reactions matrix is created)
  if (m_bUsingArrays)
    if (!Init_Complex_Types_Neighbour_Sensitive())
      {
	m_pApp->Message(MSG_TYPE_ERROR, 140);
	return FALSE;
      }
	
  // Initialise complex levels in system
  if (!Init_Complex_Levels())
    {
      m_pApp->Message(MSG_TYPE_ERROR, 47);
      return FALSE;
    }


  // Initialise reactions (load in from file and create reaction matrix)
  if (!Init_Reactions(&nNumDynReactions, P_Dynamic_Reactions, lpszDynCode,
		      flOrigRateK))
    {
      m_pApp->Message(MSG_TYPE_ERROR, 48);
      return FALSE;
    }


  // Post-initialise complex types in system
  // (must be done after the reactions matrix is created)
  if (!Init_Complex_Types_Post())
    {
      m_pApp->Message(MSG_TYPE_ERROR, 46);
      return FALSE;
    }


  // Optimise the reaction matrix
  // (must be done after complex types have been fully initialised)
  if (!Init_Reactions_Optimise(nNumDynReactions, P_Dynamic_Reactions,
			       lpszDynCode, flOrigRateK))
    {
      m_pApp->Message(MSG_TYPE_ERROR,48);
      return FALSE;
    }

  // Now that the time increment has been optimised,
  // load the time-dependent parameters

  if (!m_pApp->Get_INI(INI_SIMULATION_SECTION, INI_SIMULATION_DURATION,
		     &flSimDuration))
    return FALSE;

  // If the unit of time is iterations, set the member variables directly;
  // otherwise convert using the time increment
  if (nTimeUnits == 0)
    {
      m_nNumIterations = (unsigned long long) floor(flSimDuration);
    }
  else
    {
      // Convert to iterations (minimum 1 iteration)
      m_nNumIterations =
	(unsigned long long) MAX(floor(flSimDuration / m_flTimeInc), 1);
    }
	  // Format parameter list
	  ParaListStream << m_nNumIterations;
	  ParaListStream << PARAMETER_LIST_SEPARATOR;
	  ParaListStream << setprecision(PREC_SIMULATION_TIME);
	  ParaListStream << std::fixed;
	  ParaListStream << flSimDuration << std::ends;

  // Output simulation length, number of iterations
  // and how long the simulation will take to run
  m_pApp->Message(MSG_TYPE_STATUS, 77, ParaListStream.str().c_str());
  // Validate time-dependent parameters

  if (m_nNumIterations <= 0)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 104);
      return FALSE;
    }

  // Initialise display variables in system
  if (!Init_Display_Variables())
    {
      m_pApp->Message(MSG_TYPE_ERROR, 98);
      return FALSE;
    }

  // Initialise display variables in application
  // (used in case application needs to set up things on basis
  //  of the variable name)
  m_pApp->Init_Display_Variables(P_Output_Manager[OM_COMPLEX]
			       -> Get_DisplayVarList(),
			       P_Output_Manager[OM_COMPLEX]
			       -> Get_NumDisplayVar());


  // Initialise dump variables in system
  if (!Init_Dump_Variables())
    {
      m_pApp->Message(MSG_TYPE_ERROR, 97);
      return FALSE;
    }

  // Create and initialise complex arrays and the array output manager
  // if we are using complex arrays
  if (m_bUsingArrays)
    {
      if (!Init_Complex_Arrays())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 124);
	  return FALSE;
	}

      P_Output_Manager[OM_ARRAY]
	= new C_Output_Manager_Array(m_pApp, this);
      if (P_Output_Manager[OM_ARRAY] == NULL)
	{
	  // Ouput error if creation failed
	  m_pApp->Message(MSG_TYPE_ERROR, 154);
	  return FALSE;
	}
      m_nNumOutputManagers ++;

      if (!Init_Array_Variables())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 130);
	  return FALSE;
	}
    }

  // Initialise the output managers
  // (must be done after time-dependent parameter initialisation)
  if (!Init_Output_Managers(nTimeUnits))
    {
      m_pApp->Message(MSG_TYPE_ERROR, 159);
      return FALSE;
    }

  // Initialise the event manager
  // (must be done after time-dependent parameter initialisation)
  if (!Init_Event_Manager(nTimeUnits))
    {
      m_pApp->Message(MSG_TYPE_ERROR, 112);
      return FALSE;
    }

  // Perform quick bench test (ave duration = 0.164 sec on Pentium 90 
  //                           when compiled in Debug mode)

  // Record start time of test
  tStart = clock();
  flTest = 0;
  // Quick bench test (retrieve a bunch of random numbers)
  for (nCount = 1; nCount < BENCH_TEST_ITERATIONS; nCount ++)
    flTest += P_Ran_Gen->New_Num();
  // Record ending time of test
  tEnd = clock();

  // Output result of bench test
  ParaListStream.str("");
  ParaListStream << setprecision(PREC_SIMULATION_TIME) << std::fixed;
  ParaListStream << (m_nNumIterations * 4 * (((double) (tEnd - tStart))/CLOCKS_PER_SEC)/BENCH_TEST_ITERATIONS) << std::ends;

  m_pApp->Message(MSG_TYPE_STATUS, 78, ParaListStream.str().c_str());

  return TRUE;  // Return indicating success
}


/*************************************************************************
*
* METHOD NAME:	Init_Components
*
* DESCRIPTION:	This method initialises the components in the system.
*		The list of component symbols is retrieved from a file
*		and for each component, the other details are read from
*		the file.
*
* RETURNS:	Bool	TRUE	- Components were initialised successfully
*			FALSE	- An error occurred initialising components
*
*************************************************************************/

Bool
C_System::Init_Components(void)
{
  char	lpszComplexFile[MAX_FILE_NAME_LENGTH];
  char	lpszSymbols[MAX_COMPONENT_SYMBOL_LENGTH * MAX_COMPONENTS];
  char	lpszName[MAX_COMPONENT_NAME_LENGTH];
  char	lpszMsg[MAX_MESSAGE_LENGTH];
  char	lpszSectionName[MAX_INI_SECTION_LENGTH];
  char*	lpszPos;
  int	nCurrComponent;
  int	i;

  // Get name of file containing component information
	
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE,
		     lpszComplexFile))
    return FALSE;

  // Get symbols of components from file
  if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_SYMBOLS, lpszSymbols,
		     lpszComplexFile))
    return FALSE;

  // Strip out symbols from string and create new components

  nCurrComponent = 0;
  // Find first symbol in symbol list
  lpszPos = strtok(lpszSymbols, INI_COMPLEX_SEPARATOR);
  // While the end of the symbol list is not reached
  // and the maximum number of components is not exceeded...
  while ((lpszPos != NULL) && (nCurrComponent < MAX_COMPONENTS))
    {
      i = 0;
      // Loop through all the components already defined
      // ensuring there are no symbol ambiguities
      // (A symbol ambiguity occurs if the new symbol is contained
      //  within a pre-existing symbol or vice versa)
      while ((i < nCurrComponent)
	     && (strstr(lpszPos, Component[i].Get_Symbol()) == NULL) 
	     && (strstr(Component[i].Get_Symbol(), lpszPos) == NULL))
	i++;
      // If a symbolic ambiguity occurred...
      if (i < nCurrComponent)
	{
	  // Create parameter list string for error message
	  sprintf(lpszMsg, "%s%s%s", lpszPos,
		  PARAMETER_LIST_SEPARATOR, Component[i].Get_Symbol());
	  // Output error message
	  m_pApp->Message(MSG_TYPE_ERROR, 50, lpszMsg);
	  return FALSE;
	}
      // If a component symbol length exceeds maximum...
      if (strlen(lpszPos) > MAX_COMPONENT_SYMBOL_LENGTH)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 83, lpszPos);
	  return FALSE;
	}
      // Set the symbol for a new component
      Component[nCurrComponent].Set_Symbol(lpszPos);
      // Update count for number of components
      nCurrComponent ++;
      // Find next symbol
      lpszPos = strtok(NULL, INI_COMPLEX_SEPARATOR);
    }
  // If the end of the list wasn't reached, output error
  if (lpszPos != NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 19);
      return FALSE;
    }

  // Search INI file for names of components

  // Loop through all of the components defined above...
  for (i = 0; i < nCurrComponent; i ++)
    {
      // Create name of section from name of component
      strcpy(lpszSectionName, INI_COMPONENT_SECTION);
      strcat(lpszSectionName, Component[i].Get_Symbol());
      // Get component name from INI file
      if (!m_pApp->Get_INI(lpszSectionName, INI_COMPONENT_NAME, lpszName,
			 lpszComplexFile))
	return FALSE;
      // Set the component name
      Component[i].Set_Name(lpszName);
    }

  // Store the number of components in the system as a member variable
  m_nNumComponents = nCurrComponent;
  return TRUE;  // Return without error
}


/*************************************************************************
*
* METHOD NAME:	Init_Complex_Types
*
* DESCRIPTION:	This method initialises the complex types in the system.
*		These are stored as sets in a file. First, the parameter
*		indicating the number of sets is retrieved. Second,
*		a set of complex types is retrieved. Third, the set is
*		parsed and each complex type is extracted and a complex
*		type object initialised. (See complex type file for precise 
*		details).
*
* RETURNS:	Bool	TRUE	- Complex types were initialised successfully
*			FALSE	- An error occurred initialising complex types
*
*************************************************************************/

Bool
C_System::Init_Complex_Types(void)
{
  char		lpszComplexFile[MAX_FILE_NAME_LENGTH];
  char		lpszSetName[MAX_INI_PARAMETER_LENGTH];
  char		lpszSetComplexTypes[MAX_INI_VALUE_LENGTH];
  char		lpszComplexType[MAX_COMPONENTS_IN_COMPLEX
			       * MAX_COMPONENT_SYMBOL_LENGTH];
  char		lpszTemp[MAX_COMPONENTS_IN_COMPLEX
			* MAX_COMPONENT_SYMBOL_LENGTH];
  char*		lpszPos;
  long		nNumSets;
  int		nCurrType;
  int		nComponentNum;
  int		i;
  int		j;
  C_Component*	P_Component_List[MAX_COMPONENTS_IN_COMPLEX];

  // Get name of file containing complex type information
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE,
		     lpszComplexFile))
    return FALSE;

  // Get number of sets of complex types from file
  if (!m_pApp->Get_INI(INI_COMPLEX_TYPE_SECTION, INI_NUM_SETS, &nNumSets,
		     lpszComplexFile))
    return FALSE;

  // For each set of complex types in the INI file, strip out the complex types
  nCurrType = 0;
  for (i = 0; i < nNumSets; i++)
    {
      // Construct name of set (line name in INI file)
      sprintf(lpszSetName, "%s%d", INI_SET_OF_COMPLEX_TYPES, i + 1);
      // Read set of complex types
      if (!m_pApp->Get_INI(INI_COMPLEX_TYPE_SECTION, lpszSetName,
			 lpszSetComplexTypes, lpszComplexFile))
	return FALSE;

      // Strip out complex types from set

      // Find first type
      lpszPos = strtok(lpszSetComplexTypes, INI_COMPLEX_SEPARATOR);
      // While the end of the set is not reached...
      while ((lpszPos != NULL) && (nCurrType < MAX_COMPLEX_TYPES))
	{
	  nComponentNum = 0;
	  // Copy the composition string of the new complex type
	  strcpy(lpszComplexType, lpszPos);

	  // While there are still components in the complex type...
	  while ((strlen(lpszComplexType) > 0)
		 && (nComponentNum < MAX_COMPONENTS_IN_COMPLEX))
	    {
	      j = 0;
	      // Search through all the components for one which has
	      // the same symbol as the start of the complex type
	      while ((j < m_nNumComponents)
		     && (strncmp(lpszComplexType,
				 Component[j].Get_Symbol(),
				 strlen(Component[j].Get_Symbol())) != 0))
		j ++;

	      // If the component was not found, output error
	      if (j >= m_nNumComponents)
		{
		  m_pApp->Message(MSG_TYPE_ERROR, 20, lpszPos);
		  return FALSE;
		}						

	      // Add component to component list for this complex type
	      P_Component_List[nComponentNum] = &(Component[j]);
	      // Increment number of components in this complex type
	      nComponentNum ++;
	      // Remove the component we have just added to the component list
	      // from the complex type composition string
	      strcpy(lpszTemp,
		     lpszComplexType + strlen(Component[j].Get_Symbol()));
	      strcpy(lpszComplexType, lpszTemp);
	    }

	  // If complex contains too many complex types, output error
	  if (strlen(lpszComplexType) > 0)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 21, lpszPos);
	      return FALSE;
	    }
			
	  // Null terminate component list
	  P_Component_List[nComponentNum] = NULL;

	  // Attempt to create new complex type
	  // using component list just retrieved
	  P_Complex_Type[nCurrType] = new C_Complex_Type(m_pApp,
							 P_Component_List,
							 nCurrType, this);
	  // Ensure the complex type was created successfully
	  if (!P_Complex_Type[nCurrType])
	    {							
	      m_pApp->Message(MSG_TYPE_ERROR, 8, "new Complex Type");
	      return FALSE;
	    }
			
	  nCurrType ++;
	  // Find next complex type in set of complex types
	  lpszPos = strtok(NULL, INI_COMPLEX_SEPARATOR);
	}
		
      // If the end of the set wasn't reached, output error
      if (lpszPos != NULL)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 22);
	  return FALSE;
	}
    }

  // Store the number of complex types created in a member variable
  m_nNumComplexTypes = nCurrType;
  return TRUE;                   // Return without error
}


/*************************************************************************
*
* METHOD NAME:	Init_Complex_Levels
*
* DESCRIPTION:	Initialises the complex objects based on the specified 
*		initial levels of each complex type. Then the
*		initial concentration for each complex type is 
*		retrieved from a file and the appropriate number of 
*		complex objects are created and then set to
*		the required complex type. All the complex types are
*		probed to ensure that any rapid equilibria establish 
*		themselves.
*
* RETURNS:	Bool	TRUE	- Complexes were initialised successfully
*			FALSE	- An error occurred initialising complexes
*
*************************************************************************/

Bool
C_System::Init_Complex_Levels(void)
{
  C_Complex_Type* P_New_Type;
  char		  lpszComplexFile[MAX_FILE_NAME_LENGTH];
  int		  nCurrType;
  //  long		  nLevel;
  long		  nComplexesCreated;
  long		  nNumFreeComplexes;
  long		  i;

  ifstream        Dump_Stream;
  long		  nFromDumpFile;
  char		  lpszDumpFile[MAX_FILE_NAME_LENGTH];
  char		  lpszDumpFilePath[MAX_FILE_NAME_LENGTH];
  char	          lpszPrevTime[MAX_INI_LINE_LENGTH];
  char	          lpszSnapshotTime[MAX_INI_LINE_LENGTH];
  char	          lpszCTCode[MAX_STRING_LENGTH];
  char	          lpszCTConc[MAX_STRING_LENGTH];
  ostringstream   ParaListStream;
  char	          lpszParaList[MAX_MESSAGE_LENGTH];
  char	          lpszTmp[MAX_STRING_LENGTH];
  char*           lpszColTitles;
  char*           lpszSnapshotLine;
  char*           lpszPrevLine;
  char*	          lpszDummy;
  long            P_CT_Level[MAX_COMPLEX_TYPES];
  long            nColWidth;
  long            nColCount;
  long            nCTConc;
  long            nCT_ID;
  long            nLineLength;
  double          flTime;
  double          flSnapshotTime;
  double          flPrevTime;
  Bool            bEOF;

  // Get name of file containing complex level information
	
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE,
		     lpszComplexFile))
    return FALSE;

  // Attempt to get flag indicating whether or not initial levels will
  // be read from a dump file
  if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_INITIAL_LEVELS_FROM_DUMP_FILE,
		     &nFromDumpFile, lpszComplexFile, TRUE))
    {
      // If flag doesn't exist, set it to zero
      nFromDumpFile = 0;
    }

  // For each complex type, set up the initial complex levels

  // If the flag is set, use the dump file
  if (nFromDumpFile)
    {
      // Allocate memory to store each line from dump file
      // (we do this from the heap to avoid allocating too much off the stack)
      lpszColTitles = new char [MAX_INPUT_FROM_DUMP_LINE_LENGTH];

      // Get the name of the dump file to read from
      if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_INITIAL_LEVELS_FILE,
			 lpszDumpFile, lpszComplexFile))
	return FALSE;
      // Get the time at which the state of the system should be read from
      if (!m_pApp->Get_INI(INI_GENERAL_SECTION,
			 INI_INITIAL_LEVELS_FROM_TIME,
			 &flTime, lpszComplexFile))
	return FALSE;
      // Output message to indicate that dump file is being used
      sprintf(lpszParaList, "%s%s%s%s%s", lpszDumpFile,
	      PARAMETER_LIST_SEPARATOR, INI_INITIAL_LEVELS_SECTION,
	      PARAMETER_LIST_SEPARATOR, lpszComplexFile);
      m_pApp->Message(MSG_TYPE_STATUS, 191, lpszParaList);

      strcpy (lpszDumpFilePath, m_pApp->m_lpszWorkingDir);
      strcat (lpszDumpFilePath, FILE_PATH_SEPARATOR);
      strcat (lpszDumpFilePath, lpszDumpFile);
      // Open dump file
      Dump_Stream.open(lpszDumpFilePath, ios::in);
#if ( defined(_ALPHA) || defined(_AIX) )
      if (!Dump_Stream.rdbuf()->is_open())
#else
	if (!Dump_Stream.is_open())
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 29, lpszDumpFilePath);
	  return FALSE;
	}
      // Get first line
      Dump_Stream.getline(lpszColTitles,
			  sizeof(char) * MAX_INPUT_FROM_DUMP_LINE_LENGTH);
      if (Dump_Stream.fail())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 6, lpszDumpFilePath);
	  return FALSE;
	}

      //Calculate column widths

      nColWidth = 0;
      // Find title for column (end of blank space)
      // (Column consists of eg "   Time|     A|     B|"
      //  where '|' is column divider)
      while (lpszColTitles[nColWidth] == VALUES_COLUMN_SEPARATOR[0]
	     || lpszColTitles[nColWidth] == ' ')
	nColWidth ++;
      // Find end of column title
      while (lpszColTitles[nColWidth] != VALUES_COLUMN_SEPARATOR[0])
	nColWidth ++;
      // Calculate the max column width by the width of the column titles
      // plus an extra column in case of overrun
      nLineLength = strlen(lpszColTitles) + nColWidth;

      // Allocate memory for strings

      lpszPrevLine = NULL;
      lpszSnapshotLine = NULL;
      // Try to allocate memory for line strings
      lpszPrevLine = new char [nLineLength];
      lpszSnapshotLine = new char [nLineLength];
      // If either of the strings were not allocated correctly
      if ((lpszPrevLine == NULL) || (lpszSnapshotLine == NULL))
	{
	  // Output error
	  m_pApp->Message(MSG_TYPE_ERROR, 8,
			"reading concentrations from dump file");
	  return FALSE;
	}

      // Read in first line

      // Get the first data line
      Dump_Stream.getline(lpszPrevLine, sizeof(char) * nLineLength);
      if (Dump_Stream.fail())
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 6, lpszDumpFilePath);
	  return FALSE;
	}
      // Copy the time of the first line to a string
      strncpy(lpszPrevTime, lpszPrevLine, nColWidth);
      // Get the time of the first line
      flPrevTime = atof(lpszPrevTime);

      // Find the line closest to the chosen time

      // Try to get 2nd line
      Dump_Stream.getline(lpszSnapshotLine, sizeof(char) * nLineLength);
      // If there's only 1 line or 1st line > time we want
      if (flPrevTime > flTime || Dump_Stream.fail())
	{
	  // Set the line we are going to use - ie the previous
	  // line, by swapping pointers
	  lpszDummy = lpszPrevLine;
	  lpszPrevLine = lpszSnapshotLine;
	  lpszSnapshotLine = lpszDummy;
	}
      // Otherwise
      else
	{
	  // Copy the time of the current line to a string
	  strncpy(lpszSnapshotTime, lpszSnapshotLine, nColWidth);
	  // Get the time of the current line
	  flSnapshotTime = atof(lpszSnapshotTime);
	  bEOF = FALSE;
	  // Keep reading in lines until we've gone too far or
	  // end of file is reached
	  while ((flSnapshotTime < flTime) && (!bEOF))
	    {
	      // Save previous time for next iteration
	      flPrevTime = flSnapshotTime;
	      // Save line for next iteration by swapping pointers
	      lpszDummy = lpszPrevLine;
	      lpszPrevLine = lpszSnapshotLine;
	      lpszSnapshotLine = lpszDummy;
	      // Read line from file
	      Dump_Stream.getline(lpszSnapshotLine,sizeof(char) * nLineLength);
	      if (Dump_Stream.fail())
		{
		  m_pApp->Message(MSG_TYPE_ERROR, 6, lpszDumpFilePath);
		  return FALSE;
		}

	      // If we're not at the end of the file
	      if (!(bEOF = Dump_Stream.eof()))
		{
		  // Copy the time of the current line to a string
		  strncpy(lpszSnapshotTime, lpszSnapshotLine, nColWidth);
		  // Get the time of the current line
		  flSnapshotTime = atof(lpszSnapshotTime);
		}
	    }
	  // If time we want is closer to previous line than current one or
	  // we're at EOF
	  if (flTime - flPrevTime < flSnapshotTime - flTime || bEOF)
	    {
	      // Set the line we are going to use - ie the
	      // previous line, by swapping pointers
	      lpszDummy = lpszPrevLine;
	      lpszPrevLine = lpszSnapshotLine;
	      lpszSnapshotLine = lpszDummy;
	    }
	}
      // Close the dump file
      Dump_Stream.close();
      // Copy the time of the current line to a string
      strncpy(lpszSnapshotTime, lpszSnapshotLine, nColWidth);
      // Get the time of the current line
      flSnapshotTime = atof(lpszSnapshotTime);
      ParaListStream << setprecision(PREC_SNAPSHOT_TIME) << std::fixed;
      ParaListStream << flSnapshotTime << ends;
      m_pApp->Message(MSG_TYPE_STATUS, 189, ParaListStream.str().c_str());
      
      // Initialise array for storing complex type levels
      for (nCurrType = 0; nCurrType < m_nNumComplexTypes; nCurrType ++)
	P_CT_Level[nCurrType] = -1;

      // Loop through all the concentrations in the line extracted 
      for (nColCount = nColWidth + strlen(VALUES_COLUMN_SEPARATOR);
	   nColCount < (long) strlen(lpszSnapshotLine);
	   nColCount += nColWidth + strlen(VALUES_COLUMN_SEPARATOR))
	{
	  // Get the column heading (CT code)
	  strncpy(lpszCTCode, &(lpszColTitles[nColCount]), nColWidth);
	  lpszCTCode[nColWidth] = '\0';
	  // Trim spaces from ends
	  for (i = 0; lpszCTCode[i] == ' '; i++);
	  strcpy(lpszTmp, &(lpszCTCode[i]));
	  strcpy(lpszCTCode,lpszTmp);
	  for (i = 0; lpszCTCode[i] != ' ' && lpszCTCode[i] != '\0'; i++);
	  lpszCTCode[i] = '\0';
	  // Get the concentration
	  strncpy(lpszCTConc, &(lpszSnapshotLine[nColCount]), nColWidth);
	  // Convert the concentration to a long integer
	  nCTConc = atol(lpszCTConc);
	  // If this CT does not represent the state of a multistate CT
	  if (strstr(lpszCTCode, FLAG_CT_SEPARATOR) == NULL)
	    {
	      // Get the System ID of this complex type
	      nCT_ID = -1;
	      for (nCurrType = 0; nCurrType < m_nNumComplexTypes; nCurrType ++)
		{
		  // Get the required level of this complex type from the INI file
		  if (strcmp(P_Complex_Type[nCurrType]->Display_String(),
			     lpszCTCode) == 0)
		    {
		      nCT_ID = nCurrType;
		    }
		}
	      // If it doesn't exist...
	      if (nCT_ID == -1)
		{
		  // Output Error
		  m_pApp->Message(MSG_TYPE_ERROR, 186, 2, lpszCTCode,
				lpszDumpFilePath);
		  return FALSE;
		}

	      // Set level of this complex type
	      P_CT_Level[nCT_ID] = nCTConc;
	    }
	}
      // Clear up memory
      if (lpszColTitles != NULL)
	delete[] lpszColTitles;
      if (lpszPrevLine != NULL)
	delete[] lpszPrevLine;
      if (lpszSnapshotLine != NULL)
	delete[] lpszSnapshotLine;

      // Ensure that all complex type levels were found in dump file
      for (nCurrType = 0; nCurrType < m_nNumComplexTypes; nCurrType ++)
	{
	  // If the level of this complex type is a false value
	  if (P_CT_Level[nCurrType] == -1)
	    {
	      // Output error
	      ParaListStream.str("");
	      ParaListStream << lpszCTCode;
	      ParaListStream << PARAMETER_LIST_SEPARATOR;
	      ParaListStream << lpszDumpFile;
	      ParaListStream << PARAMETER_LIST_SEPARATOR;
	      ParaListStream << setprecision(PREC_SNAPSHOT_TIME) << std::fixed;
	      ParaListStream << flSnapshotTime << std::ends;
	      m_pApp->Message(MSG_TYPE_ERROR, 187, ParaListStream.str().c_str());
	      return FALSE;
	    }
	}
    }
  //  Otherwise, read in complex type levels directly from INI file
  else
    {
      for (nCurrType = 0; nCurrType < m_nNumComplexTypes; nCurrType ++)
	{
	  // Get the required level of this complex type from the INI file
	  if (!m_pApp->Get_INI(INI_INITIAL_LEVELS_SECTION,
			     P_Complex_Type[nCurrType]->Display_String(),
			     &(P_CT_Level[nCurrType]),
			     lpszComplexFile))
	    return FALSE;
	}
    }
 
  // Set the counter to signify we have not created any complexes yet
  nComplexesCreated = 0;
  // Loop for every complex type defined in the system...
  for (nCurrType = 0; nCurrType < m_nNumComplexTypes; nCurrType ++)
    {
      // Ensure that there are sufficient complex objects
      // to cope with the number of complexes of this type
      if (nComplexesCreated + P_CT_Level[nCurrType] > m_nMaxNumComplexes)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 23);
	  return FALSE;
	}
      // Starting from the first free complex,
      // loop for every new complex that must be created...
      for (i = nComplexesCreated;
	   i < nComplexesCreated + P_CT_Level[nCurrType];
	   i ++)
	{
	  // Create a new complex
	  P_Complex[i] = P_Complex_Type[nCurrType]->Create_New_Complex(i);
	  // Ensure the complex was created successfully
	  if (P_Complex[i] == NULL)
	    return FALSE;
	}
      // Update total number of complexes created so far
      nComplexesCreated += P_CT_Level[nCurrType];
    }

  // Push remaining empty complexes onto free complex stack
  // (this is calculated as the smaller of the number of empty complexes 
  //  in the system and the free space on stack)

  nNumFreeComplexes = MIN (m_nMaxNumComplexes - nComplexesCreated,
			   P_Free_Complex->Free_Space());
  for (i = nComplexesCreated; i < nNumFreeComplexes + nComplexesCreated; i ++)
    {
      // Create empty 'placeholder' complex object
      P_Complex[i] = new C_Complex(m_pApp, this, i);
      // Ensure the complex was created successfully
      if (P_Complex[i] == NULL)
	return FALSE;
      // Push complex onto free complex stack
      P_Free_Complex -> Push(P_Complex[i]);
    }

  // Loop through all the complexes that exist and update their type
  // based on the reaction index they return
  // (to establish any rapid equilbrium - see C_Complex_Type_Fuzzy)

  for (i = 0; i < nComplexesCreated; i ++)
    {
      P_New_Type
	= P_Complex_Type[P_Complex[i]->Get_Type()->Get_Reaction_Index()];
      P_Complex[i]->Set_Type(P_New_Type);
    }
  return TRUE;  // Return indicating no error occurred
}


/*************************************************************************
*
* METHOD NAME:	Init_Reactions
*
* DESCRIPTION:	Initialises the reaction matrix. First, an empty reaction
*		matrix is created. Second, the number of reactions is
*		retrieved from a file. Third, for each of these reactions
*		the following information is retrieved:
*			List of substrates
*			List of products
*			Forwards rate constant
*			Reverse rate constant
*		Fourth, the substrates and products (in the form of strings)
*		are identified (in the form of pointers to complex types).
*		Fifth, the forwards and / or reverse reactions are created
*		by calling C_Reaction_Matrix::New_Reaction.
*
*		The number of pseudo-complexes, n0, is calculated as:
*
*	2 * Avogadro * V * (Max Uni Rate Constant) / (Max Bi Rate Constant)
*
*	The probability of the reaction is then calculated as:
*
*	Unimolecular reaction	= k * N * (N + n0) * dt / n0
*	Bimolecular reaction	= k * N * (N + n0) * dt / 2 * Avogadro * V
* 
* PARAMETERS:	long*	     p_nNumDynReactions - Number of dyn reactions
*		C_Reaction** P_P_Dyn_Reactions  - Pointer to array of dyn
*						  reactions
*		char**	     lpszDynCode	- Pointer to array of dyn codes
*		double*	     p_flOrigRateK	- Pointer to array of original
*						  rate constants for reactions
*
* RETURNS:	long*	     p_nNumDynReactions - Number of dyn reactions
*		C_Reaction** P_P_Dyn_Reactions  - Pointer to array of dyn
*						  reactions
*		char**	     lpszDynCode        - Pointer to array of dyn codes
*		double*	     p_flOrigRateK	- Pointer to array of original
*						  rate constants for reactions
*		Bool	   TRUE	     - Reactions were initialised successfully
*			   FALSE     - An error occurred initialising reactions
*
*************************************************************************/

Bool
C_System::Init_Reactions(long* p_nNumDynReactions,
			 C_Reaction** P_P_Dyn_Reactions,
			 char lpszDynCode[][MAX_INI_VALUE_LENGTH],
			 double* p_flOrigRateK)
{
  char	                lpszReactionFile[MAX_FILE_NAME_LENGTH];
  char	                lpszReactionSection[MAX_INI_SECTION_LENGTH];
  char	                lpszSubstrateList[MAX_COMPONENTS_IN_COMPLEX
					 * MAX_COMPONENT_SYMBOL_LENGTH
					 * MAX_PRODUCTS_IN_REACTION];
  char	                lpszProductList[MAX_COMPONENTS_IN_COMPLEX
				       * MAX_COMPONENT_SYMBOL_LENGTH
				       * MAX_PRODUCTS_IN_REACTION];
  char	                lpszKForward[MAX_INI_VALUE_LENGTH];
  char	                lpszKReverse[MAX_INI_VALUE_LENGTH];
  char	                lpszKForwardDynCode[MAX_INI_VALUE_LENGTH];
  char	                lpszKReverseDynCode[MAX_INI_VALUE_LENGTH];
  char*	                lpszPosDynCode;
  char*	                lpszPos;
  long	                nNumReactions;
  long	                nCurrReaction;
  int	                nNumSubstrates;
  int	                nNumProducts;
  double                flNumerator;
  double                flConvUni;
  double                flConvBi;
  double                flKForward;
  double                flKReverse;
  double                flKMaxUni;
  double                flKMaxBi;
  double                flScaleFactor;
  C_Complex_Type*	P_P_Substrates[MAX_PRODUCTS_IN_REACTION];
  C_Complex_Type*       P_P_Products[MAX_PRODUCTS_IN_REACTION];
  C_Reaction*		P_Reaction1;
  C_Reaction*		P_Reaction2;

  // Attempt to create reaction matrix
  // (must be performed after complex types are initialised)

  P_R_M = new C_Reaction_Matrix(m_pApp, P_Complex_Type, m_nNumComplexTypes);
  if (!P_R_M)  // Ensure matrix was created successfully
    {
      m_pApp->Message(MSG_TYPE_ERROR, 24);
      return FALSE;
    }

  // Get name of file containing reaction information
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_REACTION_INI_FILE,
		     lpszReactionFile))
    return FALSE;

  // Get number of reactions in file
  if (!m_pApp->Get_INI(INI_REACT_GENERAL_SECTION, INI_NUM_OF_REACTIONS,
		     &nNumReactions, lpszReactionFile))
    return FALSE;

  // For each reaction in file, get the largest unimolecular
  // and bimolecular rate constants

  flKMaxUni = 0;  // Set temporary maximum for unimolecular rate constant
  flKMaxBi  = 0;  // Set temporary maximum for bimolecular rate constant
  *p_nNumDynReactions = 0;

  for (nCurrReaction = 0; nCurrReaction < nNumReactions; nCurrReaction ++)
    {
      lpszKForwardDynCode[0] = NULL_CHAR;
      lpszKReverseDynCode[0] = NULL_CHAR;
      sprintf(lpszReactionSection, "%s%ld", INI_REACTION_SECTION,
	      nCurrReaction + 1);

      // Retrieve list of substrates
      if (!m_pApp->Get_INI(lpszReactionSection, INI_SUBSTRATES,
			 lpszSubstrateList, lpszReactionFile))
	return FALSE;
      // Retrieve list of products
      if (!m_pApp->Get_INI(lpszReactionSection, INI_PRODUCTS, lpszProductList,
			 lpszReactionFile))
	return FALSE;
      // Retrieve forward reaction rate constant
      if (!m_pApp->Get_INI(lpszReactionSection, INI_K_FORWARD, lpszKForward,
			 lpszReactionFile))
	return FALSE;
      // Retrieve reverse reaction rate constant
      if (!m_pApp->Get_INI(lpszReactionSection, INI_K_REVERSE, lpszKReverse,
			 lpszReactionFile))
	return FALSE;

      // Look for a dynamic object code for the forward rate constant
      lpszPosDynCode = strstr(lpszKForward, INI_RATE_DYN_CODE_SEPARATOR);
      // If a dynamic object code is specified with the forward rate constant
      if (lpszPosDynCode != NULL)
	// Null terminate the rate constant				
	lpszPosDynCode[0] = NULL_CHAR;
      // Convert the rate constant to a number
      flKForward = atof(lpszKForward);  

      // Look for a dynamic object code for the reverse rate constant
      lpszPosDynCode = strstr(lpszKReverse, INI_RATE_DYN_CODE_SEPARATOR);
      // If a dynamic object code is specified with the forward rate constant
      if (lpszPosDynCode != NULL)
	// Null terminate the rate constant
	lpszPosDynCode[0] = NULL_CHAR;
      // Convert the rate constant to a number
      flKReverse = atof(lpszKReverse);

      // If forward reaction is unimolecular...
      if (strstr(lpszSubstrateList, INI_REACTION_SEPARATOR) == NULL)
	//   Update the maximum uni rate constant
	flKMaxUni = MAX(flKMaxUni, flKForward);
      else  // Otherwise...
	//   Update the maximum bi rate constant
	flKMaxBi = MAX(flKMaxBi, flKForward);
      // If reverse reaction is unimolecular...
      if (strstr(lpszProductList, INI_REACTION_SEPARATOR) == NULL)
	//   Update the maximum uni rate constant
	flKMaxUni = MAX(flKMaxUni, flKReverse);
      else  // Otherwise...
	//   Update the maximum bi rate constant
	flKMaxBi = MAX(flKMaxBi, flKReverse);
    }

  // If a maximum unimolecular or bimolecular rate was not found...
  if (flKMaxUni == 0)
    m_nNumUniPseudoComplexes = 1;  // Assume only one pseudo-complex
  // Otherwise set the number of pseudo-complexes according to the equation
  else if (flKMaxBi == 0)
    m_nNumUniPseudoComplexes = MAX_LONG - m_nMaxNumComplexes;
  else
    m_nNumUniPseudoComplexes
      = (long) (2.0 * AVOGADRO_N * m_flReactionVolume * flKMaxUni / flKMaxBi);

  // If the number of pseudo-complexes is negative (because of overflow)
  if (m_nNumUniPseudoComplexes < 0)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 200);
      return FALSE;
    }
  // If there are no pseudo-complexes (because of rounding errors)
  if (m_nNumUniPseudoComplexes < 1)
    // Set the number of pseudo-complexes to one
    m_nNumUniPseudoComplexes = 1;
  // Output number of pseudo-complexes for unimolecular reactions being used
  if (m_nNumUniPseudoComplexes == 1)
    m_pApp->Message(MSG_TYPE_STATUS, 73);
  else
    m_pApp->Message(MSG_TYPE_STATUS, 74, 1, m_nNumUniPseudoComplexes);


  // Calculate constants for converting rate constants into probabilities

  flNumerator
    = ((double) m_nMaxNumComplexes
       * (double) (m_nMaxNumComplexes + m_nNumUniPseudoComplexes)
       * (double) m_flTimeInc);
  flConvBi  = flNumerator / (2 * m_flReactionVolume * AVOGADRO_N);
  flConvUni = flNumerator / (double) m_nNumUniPseudoComplexes;

  // For each reaction in file, create new entries in reaction matrix

  for (nCurrReaction = 0; nCurrReaction < nNumReactions; nCurrReaction ++)
    {

      lpszKForwardDynCode[0] = NULL_CHAR;
      lpszKReverseDynCode[0] = NULL_CHAR;
      sprintf(lpszReactionSection, "%s%ld", INI_REACTION_SECTION,
	      nCurrReaction + 1);
      // Retrieve list of substrates
      if (!m_pApp->Get_INI(lpszReactionSection, INI_SUBSTRATES,
			 lpszSubstrateList, lpszReactionFile))
	return FALSE;
      // Retrieve list of products
      if (!m_pApp->Get_INI(lpszReactionSection, INI_PRODUCTS, lpszProductList,
			 lpszReactionFile))
	return FALSE;
      // Retrieve forward reaction rate constant
      if (!m_pApp->Get_INI(lpszReactionSection, INI_K_FORWARD, lpszKForward,
			 lpszReactionFile))
	return FALSE;
      // Retrieve reverse reaction rate constant
      if (!m_pApp->Get_INI(lpszReactionSection, INI_K_REVERSE, lpszKReverse,
			 lpszReactionFile))
	return FALSE;

      // Look for a dynamic object code for the forward rate constant
      lpszPosDynCode = strstr(lpszKForward, INI_RATE_DYN_CODE_SEPARATOR);
      // If a dynamic object code is specified with the forward rate constant
      if (lpszPosDynCode != NULL)
	{
	  // Extract the dynamic object code
	  strcpy(lpszKForwardDynCode,
		 lpszPosDynCode + strlen(INI_RATE_DYN_CODE_SEPARATOR));
	  lpszPosDynCode[0] = NULL_CHAR;  // Null terminate the rate constant
	}
      // Convert the rate constant to a number
      flKForward = atof(lpszKForward);

      // Look for a dynamic object code for the reverse rate constant
      lpszPosDynCode = strstr(lpszKReverse, INI_RATE_DYN_CODE_SEPARATOR);
      // If a dynamic object code is specified with the forward rate constant
      if (lpszPosDynCode != NULL)
	{
	  // Extract the dynamic object code
	  strcpy(lpszKReverseDynCode,
		 lpszPosDynCode + strlen(INI_RATE_DYN_CODE_SEPARATOR));
	  lpszPosDynCode[0] = NULL_CHAR;  // Null terminate the rate constant
	}
      // Convert the rate constant to a number
      flKReverse = atof(lpszKReverse);

      // Ensure the rate constants are valid
      if ((flKForward < 0) || (flKReverse < 0))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 25, 1, nCurrReaction);
	  return FALSE;
	}

      // Identify the substrates specified

      nNumSubstrates = 0;
      // Find the first substrate in the list of substrates string
      lpszPos = strtok(lpszSubstrateList, INI_REACTION_SEPARATOR);
      // While the end of the substrate list has not been reached
      // and the max no of products has not been exceeded
      while ((lpszPos != NULL) && (nNumSubstrates < MAX_PRODUCTS_IN_REACTION))
	{
	  P_P_Substrates[nNumSubstrates] = Identify_Complex_Type(lpszPos);
	  // Identify complex type from list
	  // If complex type is not recognised, output error
	  if (P_P_Substrates[nNumSubstrates] == NULL)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 26, 1, nCurrReaction + 1);
	      return FALSE;	
	    }
	  // Increment number of substrates identified so far
	  nNumSubstrates ++;
	  // Find the next substrate in the list of substrates
	  lpszPos = strtok(NULL, INI_REACTION_SEPARATOR);
	}
      // If the end of the list wasn't reached, output error
      if (lpszPos != NULL)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 14);
	  return FALSE;
	}

      // Identify the products specified

      // Find the first product in the list of products string
      nNumProducts = 0;
      lpszPos = strtok(lpszProductList, INI_REACTION_SEPARATOR);
      // While the end of the product list has not been reached
      // and the max no of products has not been exceeded
      while ((lpszPos != NULL) && (nNumProducts < MAX_PRODUCTS_IN_REACTION))
	{
	  // Identify complex type from list
	  P_P_Products[nNumProducts] = Identify_Complex_Type(lpszPos);
	  // If complex type is not recognised, output error
	  if (P_P_Products[nNumProducts] == NULL)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 26, 1, nCurrReaction + 1);
	      return FALSE;
	    }
	  nNumProducts ++;  // Increment number of products identified so far
	  // Find the next product in the list of products
	  lpszPos = strtok(NULL, INI_REACTION_SEPARATOR);
	}
      // If the end of the list wasn't reached, output error
      if (lpszPos != NULL)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 15);
	  return FALSE;
	}

      // Create forwards reaction

      if (flKForward > 0)  // If the forward rate constant is non-zero...
	{
	  // Calculate scale factor to convert rate to probability
	  flScaleFactor = nNumSubstrates > 1 ? flConvBi : flConvUni;
	  // Call method to create reaction in reaction matrix;
	  // ensure no errors occurred
	  if (!P_R_M->New_Reaction(nNumSubstrates, nNumProducts,
				   P_P_Substrates, P_P_Products,
				   flKForward * flScaleFactor, 
				   nCurrReaction + 1, &P_Reaction1,
				   &P_Reaction2))
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 26, 1, nCurrReaction + 1);
	      return FALSE;
	    }
	  // If the forward rate constant can change over time
	  // (ie reaction is a dynamic object), record details
	  if (strlen(lpszKForwardDynCode) > 0)
	    {
	      P_P_Dyn_Reactions[*p_nNumDynReactions] = P_Reaction1;
	      strcpy(lpszDynCode[*p_nNumDynReactions], lpszKForwardDynCode);
	      p_flOrigRateK[*p_nNumDynReactions] = flKForward;
	      (*p_nNumDynReactions) ++;
	    }
	  // If rate constant can change and this is bimolecular,
	  // record details
	  if ((strlen(lpszKForwardDynCode) > 0) && (nNumSubstrates > 1))
	    {
	      P_P_Dyn_Reactions[*p_nNumDynReactions] = P_Reaction2;
	      strcpy(lpszDynCode[*p_nNumDynReactions], lpszKForwardDynCode);
	      p_flOrigRateK[*p_nNumDynReactions] = flKForward;
	      (*p_nNumDynReactions) ++;
	    }
	}

      // Create reverse reaction

      // If the reverse rate constant is non-zero...
      if (flKReverse > 0)
	{
	  // Calculate scale factor to convert rate to probability
	  flScaleFactor = nNumProducts > 1 ? flConvBi : flConvUni;
	  // Call method to create reaction in reaction matrix;
	  // ensure no errors occurred
	  if (!P_R_M->New_Reaction(nNumProducts, nNumSubstrates, P_P_Products,
				   P_P_Substrates, flKReverse * flScaleFactor, 
				   (nCurrReaction + 1
				    + REVERSE_REACTION_ID_OFFSET),
				   &P_Reaction1, &P_Reaction2))
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 26, 1, nCurrReaction + 1);
	      return FALSE;
	    }
	  // If the reverse rate constant can change over time
	  // (ie reaction is a dynamic object), record details
	  if (strlen(lpszKReverseDynCode) > 0)
	    {
	      P_P_Dyn_Reactions[*p_nNumDynReactions] = P_Reaction1;
	      strcpy(lpszDynCode[*p_nNumDynReactions], lpszKReverseDynCode);
	      p_flOrigRateK[*p_nNumDynReactions] = flKReverse;
	      (*p_nNumDynReactions) ++;
	    }
	  // If rate constant can change and this is bimolecular,
	  // record details
	  if ((strlen(lpszKReverseDynCode) > 0) && (nNumProducts > 1))
	    {
	      P_P_Dyn_Reactions[*p_nNumDynReactions] = P_Reaction2;
	      strcpy(lpszDynCode[*p_nNumDynReactions], lpszKReverseDynCode);
	      p_flOrigRateK[*p_nNumDynReactions] = flKReverse;
	      (*p_nNumDynReactions) ++;
	    }
	}
    }
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Init_Reactions_Optimise
*
* DESCRIPTION:	Optimises the reaction matrix. Then the reactions saved to 
*		a file and dynamic values created.
* 
* PARAMETERS:	long	     nNumDynReactions  - Number of dyn reactions
*		C_Reaction** P_P_Dyn_Reactions - Pointer to array of dynamic
*					         reactions
*		char**	     lpszDynCode       - Pointer to array of dyn codes
*		double*	     p_flOrigRateK     - Pointer to array of original
*						 rate constants for reactions
*
* RETURNS:	Bool	TRUE	- Optimisation was successful
*			FALSE	- An error occurred optimisation
*
*************************************************************************/

Bool
C_System::Init_Reactions_Optimise(long nNumDynReactions,
				  C_Reaction** P_P_Dyn_Reactions,
				  char lpszDynCode[][MAX_INI_VALUE_LENGTH],
				  double* p_flOrigRateK)
{
  char lpszReactionFile[MAX_FILE_NAME_LENGTH];
  long nCurrDynReaction;
  long nOptimiseTimeInc;
  long nErrorID;
  
  // Get flag from INI file to see if we should optimise reaction matrix

  // Get the reaction INI file name
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_REACTION_INI_FILE,
		     lpszReactionFile))
    return FALSE;
  // Retrieve reaction matrix optimisation flag from INI file
  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_OPTIMISE_TIME_INC,
		     &nOptimiseTimeInc))
    return FALSE;

  // Optimise reaction matrix

  // If the reaction matrix optimisation flag is set...
  if (nOptimiseTimeInc)
    {
      // Output status message
      m_pApp->Message(MSG_TYPE_STATUS, 178);
      // Optimise reaction matrix
      if (!P_R_M->Optimise(&m_flTimeInc, P_Complex_Type))
	return FALSE;
    }
  if (!P_R_M->Dump())  // Output reaction matrix to file
    return FALSE;

  if (!P_R_M->Validate())  // Validate reaction matrix
    return FALSE;

  // Initialise the events if this is a dynamic object
  // (ie if rates change over time)

  // Loop through each dynamic reaction
  for (nCurrDynReaction = 0; nCurrDynReaction < nNumDynReactions;
       nCurrDynReaction ++)
    
    // Attempt to create events (the dynamic values must be scaled by ratio
    //   of original rate constant to optimised probability)
    if (!(P_Event_Manager
	  -> Init_Dynamic_Object(P_P_Dyn_Reactions[nCurrDynReaction],
				 lpszDynCode[nCurrDynReaction],
				 (P_P_Dyn_Reactions[nCurrDynReaction]
				  -> Get_Probability())
				 / p_flOrigRateK[nCurrDynReaction])))
      {
	// Set error ID of reaction and output error message
	nErrorID = P_P_Dyn_Reactions[nCurrDynReaction]->Get_ID();
	m_pApp->Message(MSG_TYPE_ERROR, 113, 1,
		      (((nErrorID > 0)
			&& (nErrorID < REVERSE_REACTION_ID_OFFSET))
		       ? nErrorID : nErrorID - REVERSE_REACTION_ID_OFFSET));
	return FALSE;
      }

  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Init_Complex_Types_Special
*
* DESCRIPTION:	The names of each special complex type is extracted from
*		the INI file. For each special complex type, the members
*		of the type are read, and the appropriate special complex
*		type is constructed. Further initialisation is occurred
*		after other objects (eg reaction matrix) is constructed
*		(see Init_Complex_Types_Post).
*
* RETURNS:	Bool	TRUE	- Special complex types were initialised
*                                 successfully
*		        FALSE	- An error occurred initialising special
*                                 complex types
*
*************************************************************************/

Bool
C_System::Init_Complex_Types_Special(void)
{
  C_Complex_Type*	P_Old_Complex_Type;
  C_Complex_Type*	P_New_Complex_Type;
  char			lpszComplexFile[MAX_FILE_NAME_LENGTH];
  char			lpszSpecialTypes[MAX_INI_VALUE_LENGTH];
  char			lpszSpecialTypeName[MAX_INI_SECTION_LENGTH];
  char			lpszSpecialTypeMembers[MAX_INI_VALUE_LENGTH];
  char*			lpszPos;
  char*			lpszPosCType;

  // Get name of file containing complex type information
  // (ie the complex INI file name)
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE,
		     lpszComplexFile))
    return FALSE;

  // Get the names of the special complex types
  if (!m_pApp->Get_INI(INI_SPECIAL_TYPES_SECTION, INI_SPECIAL_TYPES,
		     lpszSpecialTypes, lpszComplexFile))
    return FALSE;

  // Loop through all the special complex types;
  // for each, create the appropriate special complex types

  // Find first special type
  lpszPos = strtok(lpszSpecialTypes, INI_COMPLEX_SEPARATOR);
  // While the end of the list is not reached...
  while (lpszPos != NULL)
    {
      // Copy the string of the new special complex type
      strcpy(lpszSpecialTypeName, lpszPos);
      // Retrieve the list of complex types of this special type
      if (!m_pApp->Get_INI(INI_SPECIAL_TYPES_SECTION, lpszSpecialTypeName,
			 lpszSpecialTypeMembers, lpszComplexFile))
	return FALSE;  

      // For each complex type of this special type, create the new objects

      // Find first complex type
      lpszPosCType = strtok(lpszSpecialTypeMembers, INI_COMPLEX_SEPARATOR);
      // While the end of the list is not reached...
      while (lpszPosCType != NULL)
	{
	  P_New_Complex_Type = NULL;
	  // Identify the complex type
	  P_Old_Complex_Type = Identify_Complex_Type(lpszPosCType);
	  // Ensure the complex type could be identified
	  if (P_Old_Complex_Type == NULL)
	    return FALSE;

	  // If the compiler variable has been defined
#ifdef _SPECIAL_COMPLEX_TYPE_1_NAME
	  // Create the special complex type object specified
	  // if special name matches
	  if (strcmp(lpszSpecialTypeName, _SPECIAL_COMPLEX_TYPE_1_NAME) == 0)
	    P_New_Complex_Type
	      = new _SPECIAL_COMPLEX_TYPE_1_CLASS(m_pApp, P_Old_Complex_Type,
						  this);
#endif

	  // If the compiler variable has been defined
#ifdef _SPECIAL_COMPLEX_TYPE_2_NAME
	  // Create the special complex type object specified
	  // if special name matches
	  if (strcmp(lpszSpecialTypeName, _SPECIAL_COMPLEX_TYPE_2_NAME) == 0)
	    P_New_Complex_Type
	      = new _SPECIAL_COMPLEX_TYPE_2_CLASS(m_pApp, P_Old_Complex_Type,
						  this);
#endif

	  // If the compiler variable has been defined
#ifdef _SPECIAL_COMPLEX_TYPE_3_NAME
	  // Create the special complex type object specified
	  // if special name matches
	  if (strcmp(lpszSpecialTypeName, _SPECIAL_COMPLEX_TYPE_3_NAME) == 0)
	    P_New_Complex_Type
	      = new _SPECIAL_COMPLEX_TYPE_3_CLASS(m_pApp, P_Old_Complex_Type,
this);
#endif

	  // Ensure object was created successfully
	  if (P_New_Complex_Type == NULL)
	    {
	      // Output error if the special type is not recognised
	      m_pApp->Message(MSG_TYPE_ERROR, 80, lpszSpecialTypeName);
	      return FALSE;
	    }
	  // Copy new complex type into system (replacing old complex type)
	  P_Complex_Type[P_Old_Complex_Type->Get_Index()] = P_New_Complex_Type;
	  delete P_Old_Complex_Type;  // Delete old complex type
	  // Find next complex type in list
	  lpszPosCType = strtok(NULL, INI_COMPLEX_SEPARATOR);
	}
      // Find next special type in set
      lpszPos = strtok(NULL, INI_COMPLEX_SEPARATOR);
    }

  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Init_Complex_Types_Neighbour_Sensitive
*
* DESCRIPTION:	The names of each neighbour-sensitive (NS) complex type is
*		extracted from the INI file. For each NS complex type, the
*		members of the type are read, and the appropriate NS complex
*		type is constructed. Further initialisation occurs
*		after other objects (eg complex array) are constructed
*		(see Init_Complex_Types_Post).
*
* RETURNS:	Bool	TRUE	- Special complex types were initialised
*                                 successfully
*			FALSE	- An error occurred initialising special
*                                 complex types
*
*************************************************************************/

Bool
C_System::Init_Complex_Types_Neighbour_Sensitive(void)
{
  C_Complex_Type*	P_Old_Complex_Type;
  C_Complex_Type*	P_New_Complex_Type;
  char			lpszArrayFile[MAX_FILE_NAME_LENGTH];
  char			lpszComplexFile[MAX_FILE_NAME_LENGTH];
  char			lpszNSFile[MAX_FILE_NAME_LENGTH];
  char			lpszNSTypeMembers[MAX_INI_VALUE_LENGTH];
  char			lpszParameter[MAX_INI_PARAMETER_LENGTH];
  char*			lpszPos;
  long                  nNumNeighbours;
	
  // Get name of file containing complex information
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE,
		     lpszComplexFile))
    return FALSE;
  // Get name of file containing array information (for backwards compat.)
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE, lpszArrayFile))
    return FALSE;


  // Get names of neighbour sensitive complex types
  if (!m_pApp->Get_INI(INI_NS_TYPES_SECTION, INI_NS_COMPLEXES,
		     lpszNSTypeMembers, lpszComplexFile, TRUE))
    {
      // For backwards compatibility
      if (!m_pApp->Get_INI(INI_NS_TYPES_SECTION, INI_NS_COMPLEXES,
			 lpszNSTypeMembers, lpszArrayFile))
	return FALSE;
    }

  // For each neighbour sensitive complex type, create the new objects

  // Find first complex type
  lpszPos = strtok(lpszNSTypeMembers, INI_COMPLEX_SEPARATOR);
  // While the end of the list is not reached...
  while (lpszPos != NULL)
    {
      // Get name of file containing NS complex type information
      strcpy(lpszParameter, lpszPos);
      strcat(lpszParameter, INI_NS_TYPES_FILE);
      if (!m_pApp->Get_INI(INI_NS_TYPES_SECTION, lpszParameter, lpszNSFile,
			 lpszComplexFile, TRUE))
	{
	  // For backwards compatibility
	  if (!m_pApp->Get_INI(INI_NS_TYPES_SECTION, lpszParameter, lpszNSFile,
			     lpszArrayFile))
	    return FALSE;
	}
      // Get the number of neighbours for this NS complex type
      if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_NS_NUM_NEIGHBOURS,
			 &nNumNeighbours, lpszNSFile, TRUE))
	{
	  // For backwards compatibility, assume four neighbours if unspecified
	  nNumNeighbours = 4;
	}
      
      P_New_Complex_Type = NULL;
      // Identify the complex type
      P_Old_Complex_Type = Identify_Complex_Type(lpszPos);
      // Ensure the complex type could be identified
      if (P_Old_Complex_Type == NULL)
	return FALSE;

      P_New_Complex_Type
	= new C_Complex_Type_Neighbour_Sensitive(m_pApp, P_Old_Complex_Type,
						 this,
						 nNumNeighbours);
      // Ensure object was created successfully
      if (P_New_Complex_Type == NULL)
	{
	  // Output error if memory allocation failed
	  m_pApp->Message(MSG_TYPE_ERROR, 80, INI_NS_COMPLEXES);
	  return FALSE;
	}
      // Copy new complex type into system (replacing old complex type)
      P_Complex_Type[P_Old_Complex_Type->Get_Index()] = P_New_Complex_Type;
      // Delete old complex type
      delete P_Old_Complex_Type;
      // Find next complex type in list
      lpszPos = strtok(NULL, INI_COMPLEX_SEPARATOR);
    }
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Init_Complex_Arrays
*
* DESCRIPTION:	The names of each complex array is extracted from
*		the INI file. For each complex array, the members
*		of the array are read, and the appropriate complex array
*		type is constructed. 
*
* RETURNS:	Bool	TRUE	- Complex arrays were initialised successfully
*			FALSE	- An error occurred initialising complex arrays
*
*************************************************************************/

Bool
C_System::Init_Complex_Arrays(void)
{
  char			lpszArrayFile[MAX_FILE_NAME_LENGTH];
  char			lpszComplexFile[MAX_FILE_NAME_LENGTH];
  char			lpszComplexTypeName[MAX_INI_VALUE_LENGTH];
  char			lpszArrays[MAX_INI_VALUE_LENGTH_LONG];
  char			lpszArrayName[MAX_NUM_COMPLEX_ARRAYS][MAX_INI_SECTION_LENGTH];
  char			lpszInitialState[MAX_INI_VALUE_LENGTH];
  long			nIsNeighbourSensitive;
  int			nInitialState;
  long			nXDimension;
  long			nYDimension;
  char*			lpszPos;
  char			lpszParaList[MAX_MESSAGE_LENGTH];
  int			nCountComplex;
  int			nArrayCount;
  int			i;
  int			j;
  C_Integer_Pair		Dimensions;
  C_Integer_Pair		Address;
  C_Integer_Pair		RandAddress;
  C_Complex_Type_Multistate*	P_MSCT;
  long*				P_nInsStateLevel;
  long**                        P_P_nStateLevel;
  long**                        P_P_nStateTotalLevel;
  C_Complex_Multistate*		P_Complex_Temp;
  long				nNumCoords;
  long				nState;
  long				nNumComplexesToAdd;
  double			flRandom;
  long				nRandom;
  double			flThreshold;
  long				nComplexLevel;
  char				lpszBitString[MAX_MS_NUM_FLAGS + 1];
  long                          nBoundaryCondition;
  double                        flArrayEquilInterval;
  char                          lpszGeometry[MAX_INI_VALUE_LENGTH];
  char                          lpszInitialStateTime[MAX_INI_VALUE_LENGTH];
  char                          lpszDumpFile[MAX_INI_VALUE_LENGTH];
  char                          lpszDumpFilePath[MAX_FILE_NAME_LENGTH];
  double                        flInitialStateTime;
  long**                        P_P_nState;
  long                          nCTIndex;
  long                          nRemainingLevel;
  
  // Get name of file containing complex array information
  // (ie the array INI file name)
  
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE,
		     lpszArrayFile))
    return FALSE;

  // Get the names of the complex arrays
  if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_COMPLEX_ARRAYS, lpszArrays,
		     lpszArrayFile))
    return FALSE;

  // Allocate memory to store complex type levels
  P_P_nStateLevel = new long * [m_nNumComplexTypes];
  if (P_P_nStateLevel == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8, "initialising complex arrays");
      return FALSE;
    }
  P_P_nStateTotalLevel = new long * [m_nNumComplexTypes];
  if (P_P_nStateTotalLevel == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8, "initialising complex arrays");
      return FALSE;
    }
  // Initialise array
  for (i = 0; i < m_nNumComplexTypes; i ++)
    {
      P_P_nStateLevel[i] = NULL;
      P_P_nStateTotalLevel[i] = NULL;
    }

  // Loop through all the complex arrays;
  // for each, create the appropriate complex array object

  // Find first array
  lpszPos = strtok(lpszArrays, INI_ARRAY_SEPARATOR);
  nArrayCount = 0;
  // While the end of the list is not reached...
  while (lpszPos != NULL)
    {
      // Copy the string of the new complex array
      strcpy(lpszArrayName[nArrayCount], lpszPos);
      nArrayCount++;
      lpszPos = strtok(NULL, INI_ARRAY_SEPARATOR);
    }
  // Record the number of complex arrays in the member variable 
  m_nNumArrays = nArrayCount;

  // Loop through all of the arrays	
  for (nArrayCount = 0; nArrayCount < m_nNumArrays; nArrayCount++)
    {
		
      // Retrieve the complex type this array contains
      if (!m_pApp->Get_INI(lpszArrayName[nArrayCount], INI_ARRAY_COMPLEX,
			 lpszComplexTypeName, lpszArrayFile))
	return FALSE;
		
      // Check INI file to see whether the complexes in this array
      // are neighbour-sensitive
      if (!m_pApp->Get_INI(lpszArrayName[nArrayCount], INI_NEIGHBOUR_SENSITIVE,
			 &nIsNeighbourSensitive, lpszArrayFile))
	return FALSE;
      // If the value is invalid,
      if (nIsNeighbourSensitive != 0 && nIsNeighbourSensitive != 1)
	{
	  // Output error
	  m_pApp->Message(MSG_TYPE_ERROR, 138, 2, INI_NEIGHBOUR_SENSITIVE,
			lpszArrayName[nArrayCount]);
	  return FALSE;
	}
      // Get the complex type that this array contains
      P_MSCT = ((C_Complex_Type_Multistate*)
		Identify_Complex_Type(lpszComplexTypeName));
      // If the complex type name is invalid
      if(P_MSCT == NULL)
	{
	  // Output error
	  m_pApp->Message(MSG_TYPE_ERROR, 27, lpszComplexTypeName);
	  return FALSE;
	}

      // Get array equilibration frequency for complex arrays from file
      if (!m_pApp->Get_INI(lpszArrayName[nArrayCount], INI_ARRAY_EQUIL_INTERVAL,
			 &flArrayEquilInterval, lpszArrayFile, TRUE))
	{
	  // For backwards compatibility
	  if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_ARRAY_EQUIL_INTERVAL,
			     &flArrayEquilInterval, lpszArrayFile, TRUE))
	    // For further backwards compatibility
	    if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_ARRAY_EQUIL_FREQ,
			       &flArrayEquilInterval, lpszArrayFile, TRUE))
	      return FALSE;
	}
      // Convert equilibration frequency to iterations
      m_pnArrayEquilInterval[nArrayCount]
	= MAX((unsigned long long) (flArrayEquilInterval / m_flTimeInc), 1);

      // Determine how to initialise this complex array
      if (!m_pApp->Get_INI(lpszArrayName[nArrayCount],
			 INI_ARRAY_INITIAL_STATE,
			 lpszInitialState, lpszArrayFile, TRUE))
	{
	  // Default initialisation is randomised insertion in proportion
	  // of concentration
	  strcpy(lpszInitialState, ARRAY_FROM_CONC_STRING);
	}
      // Convert initial state string to integer value
      if (strcmp(lpszInitialState, ARRAY_FROM_CONC_STRING) == 0)
	nInitialState = ARRAY_FROM_CONC_NUM;
      else if (strcmp(lpszInitialState, ARRAY_FROM_DUMP_STRING) == 0)
	nInitialState = ARRAY_FROM_DUMP_NUM;
      else
	nInitialState = 0;

      // Retrieve X dimension of this array
      if (!m_pApp->Get_INI(lpszArrayName[nArrayCount], INI_ARRAY_X_DIMENSION,
			 &nXDimension, lpszArrayFile))
	return FALSE;
      // Retrieve Y dimension of this array		
      if (!m_pApp->Get_INI(lpszArrayName[nArrayCount], INI_ARRAY_Y_DIMENSION,
			 &nYDimension, lpszArrayFile))
	return FALSE;
		
      // Retrieve boundary condition of this array		
      if (!m_pApp->Get_INI(lpszArrayName[nArrayCount],
			 INI_ARRAY_BOUNDARY_CONDITION,
			 &nBoundaryCondition, lpszArrayFile))
	return FALSE;
		
      // Retrieve geometry of this array		
      if (!m_pApp->Get_INI(lpszArrayName[nArrayCount],
			 INI_ARRAY_GEOMETRY,
			 lpszGeometry, lpszArrayFile, TRUE))
	{
	  // Default is square (for backwards compatibility)
	  strcpy(lpszGeometry, INI_ARRAY_GEOMETRY_SQUARE);
	}

      // Set the dimensions in variable to pass to constructor
      Dimensions.Set(nXDimension, nYDimension);
      
      // Attempt to create new array object of the specified geometry
      if (strcmp(lpszGeometry, INI_ARRAY_GEOMETRY_SQUARE) == 0)
	{
	  
	  P_Complex_Array[nArrayCount]
	    = new C_Complex_Array_Square(m_pApp, lpszArrayName[nArrayCount],
					 this,
					 nArrayCount, P_MSCT,
					 &Dimensions,
					 (Bool) nIsNeighbourSensitive,
					 nBoundaryCondition);
	}
      else if (strcmp(lpszGeometry, INI_ARRAY_GEOMETRY_LINE) == 0)
	{
	  
	  P_Complex_Array[nArrayCount]
	    = new C_Complex_Array_Line(m_pApp, lpszArrayName[nArrayCount],
				       this,
				       nArrayCount, P_MSCT,
				       &Dimensions,
				       (Bool) nIsNeighbourSensitive,
				       nBoundaryCondition);
	}
      else if (strcmp(lpszGeometry, INI_ARRAY_GEOMETRY_TRIANGLE) == 0
		 ||strcmp(lpszGeometry, INI_ARRAY_GEOMETRY_TRIGONAL) == 0)
	{
	  P_Complex_Array[nArrayCount]
	    = new C_Complex_Array_Trigonal(m_pApp, lpszArrayName[nArrayCount],
					   this,
					   nArrayCount, P_MSCT,
					   &Dimensions,
					   (Bool) nIsNeighbourSensitive,
					   nBoundaryCondition);
	}
      else if (strcmp(lpszGeometry, INI_ARRAY_GEOMETRY_HEXAGON) == 0
	       || strcmp(lpszGeometry, INI_ARRAY_GEOMETRY_HEXAGONAL) == 0)
	{
	  P_Complex_Array[nArrayCount]
	    = new C_Complex_Array_Hexagonal(m_pApp, lpszArrayName[nArrayCount],
					    this,
					    nArrayCount, P_MSCT,
					    &Dimensions,
					    (Bool) nIsNeighbourSensitive,
					    nBoundaryCondition);
	}
      // If construction of array object did not succeed...
      if(P_Complex_Array[nArrayCount] == NULL)
	{
	  // Output error
	  m_pApp->Message(MSG_TYPE_ERROR, 169, lpszArrayName[nArrayCount]);
	  return FALSE;
	}
      //Format string and output status
      sprintf(lpszParaList, "%ld%s%ld%s%s%s%s%s%s%s%ld", Dimensions.nX,
	      PARAMETER_LIST_SEPARATOR, Dimensions.nY,
	      PARAMETER_LIST_SEPARATOR, lpszGeometry,
	      PARAMETER_LIST_SEPARATOR, lpszArrayName[nArrayCount],
	      PARAMETER_LIST_SEPARATOR, P_MSCT->Display_String(),
	      PARAMETER_LIST_SEPARATOR, nBoundaryCondition);
      m_pApp->Message(MSG_TYPE_STATUS, 156, lpszParaList);
		

      // Compute the number of coordinates in the array
      nNumCoords = Dimensions.nX * Dimensions.nY;

      // Get the system ID of the complex type of this array
      nCTIndex = P_MSCT->Get_Index();

      // Retrieve geometry of this array		
      if (!m_pApp->Get_INI(lpszArrayName[nArrayCount],
			 INI_ARRAY_GEOMETRY,
			 lpszGeometry, lpszArrayFile, TRUE))
	{
	  // Default is square (for backwards compatibility)
	  strcpy(lpszGeometry, INI_ARRAY_GEOMETRY_SQUARE);
	}


      // Allocate memory for storing of state level information for complexes
      // to be inserted
      P_nInsStateLevel
	= new long [PowerOfTwo(P_MSCT->Get_Num_Flags())];

      // If memory for this complex type has not already been allocated
      if (P_P_nStateLevel[nCTIndex] == NULL)
	{
	  // Allocate memory for temporary storage
	  P_P_nStateLevel[nCTIndex]
	    = new long [PowerOfTwo(P_MSCT->Get_Num_Flags())];
	  if (P_P_nStateLevel[nCTIndex] == NULL)
	    {
	      // Output error if allocation fails
	      m_pApp->Message(MSG_TYPE_ERROR, 8, "initialising complex arrays");
	      return FALSE;
	    }
	  P_P_nStateTotalLevel[nCTIndex]
	    = new long [PowerOfTwo(P_MSCT->Get_Num_Flags())];
	  if (P_P_nStateTotalLevel[nCTIndex] == NULL)
	    {
	      // Output error if allocation fails
	      m_pApp->Message(MSG_TYPE_ERROR, 8, "initialising complex arrays");
	      return FALSE;
	    }

	  // Get level of each state of this complex type
	  for (nState = 0; nState < PowerOfTwo(P_MSCT->Get_Num_Flags());
	       nState++)
	    {
	      P_P_nStateLevel[nCTIndex][nState]
		= P_MSCT -> Get_State_Level(nState);
	      P_P_nStateTotalLevel[nCTIndex][nState]
		= P_P_nStateLevel[nCTIndex][nState];
	    }
	}

      // Initialise array according to the initialisation method specified
      // in the INI file

      switch(nInitialState)
	{
	  // This subroutine of this switch construct initialises the complex
	  // array based on the concentration profile of the reaction system,
	  // in such a way that the resulting concentrations of different
	  // states within the complex array are proportional to their
	  // concentration in the entire reaction system.  After the complex 
	  // array is filled with the appropriate number of complexes, their
	  // positions are randomised.
	case ARRAY_FROM_CONC_NUM:

	  // Get complex levels for this complex and each state of this complex

	  // Get Complex INI filename
	  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE,
			     lpszComplexFile))
	    return FALSE;
	  // Get complex level for this complex
	  if (!m_pApp->Get_INI(INI_INITIAL_LEVELS_SECTION,
			     P_MSCT->Display_String(),
			     &nComplexLevel, lpszComplexFile))
	    return FALSE;
/*
          // THIS BLOCK OF CODE IS NOT NECESSARY BECAUSE THE STATE LEVELS
          // OF MS-COMPLEXES ARE INITIALIZED BEFORE THIS FUNCTION IS CALLED

	  // Get INI filename for this multistate complex type
	  strcpy(lpszParameter, P_MSCT->Display_String());
	  strcat(lpszParameter, INI_SPECIAL_TYPES_FILE);
	  if (!m_pApp->Get_INI(INI_SPECIAL_TYPES_SECTION, lpszParameter,
			     lpszMultistateFile, lpszComplexFile))
	    return FALSE;
		
	  // Loop through all of the states of this complex type
	  for (nState = 0; nState < PowerOfTwo(P_MSCT->Get_Num_Flags());
	       nState++)
	    {
	      // Convert state value to bit string
	      LongToBitString(nState, lpszBitString, P_MSCT->Get_Num_Flags());
	      // Get complex level for this state from INI file
	      if (!m_pApp->Get_INI(INI_INITIAL_STATES_SECTION, lpszBitString,
				 &P_nStateLevel[nState], lpszMultistateFile,
				 TRUE))
		// Set to default concentration if not specified in INI file
		P_nStateLevel[nState] = MS_DEFAULT_CONC;
	    }
*/
	  // Get total remaining level of this complex type
	  nRemainingLevel = 0;
	  for (nState = 0; nState < PowerOfTwo(P_MSCT->Get_Num_Flags());
	       nState++)
	    nRemainingLevel += P_P_nStateLevel[nCTIndex][nState];
	  
	  // Ensure that remaining level of this complex type is
	  // sufficient to fill the array
	  if (nRemainingLevel < nNumCoords)
	    {
	      sprintf(lpszParaList, "%s%s%s%s%ld%s%ld",
		      P_MSCT->Display_String(),
		      PARAMETER_LIST_SEPARATOR, lpszArrayName[nArrayCount],
		      PARAMETER_LIST_SEPARATOR, 
		      nRemainingLevel,
		      PARAMETER_LIST_SEPARATOR, 
		      nNumCoords);
	      m_pApp->Message(MSG_TYPE_ERROR, 194, lpszParaList);
	      return FALSE;
	    }

	  // Calculate how many complexes of each state we need in the array
	  // (from initial concentration)

	  nNumComplexesToAdd = 0;
	  // Loop through all of the states of this complex type
	  for (nState = 0; nState < PowerOfTwo(P_MSCT->Get_Num_Flags());
	       nState++)
	    {
	      // Get level of current state within the array 
	      // (proportional to the fraction of this state in total level)
	      if (P_P_nStateTotalLevel[nCTIndex][nState] != 0)
		{
		  P_nInsStateLevel[nState]
		    = (long) (nNumCoords
			      * ((double)P_P_nStateTotalLevel[nCTIndex][nState]
				 / nComplexLevel));
		}
	      else
		{
		  P_nInsStateLevel[nState] = 0;
		}
	      nNumComplexesToAdd += P_nInsStateLevel[nState];
	      P_P_nStateLevel[nCTIndex][nState] -= P_nInsStateLevel[nState];
	    }
				
	  if (nNumComplexesToAdd > nNumCoords)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 131,
			    P_Complex_Array[nArrayCount] -> Get_Name());
	      return FALSE;
	    }

	  // If total of all states is smaller than number of cooordinates
	  // (due to rounding off), keep adding 1 to one of the states
	  // (chosen by probability proportional to its fraction within the
	  // total initial concentration) until the total matches the number
	  // of coordinates in the array

	  long nNZCount;
	  long nNonZero;
	  long nNZLevelSum;
	  long* P_nNZState;
	  P_nNZState = new long [PowerOfTwo(P_MSCT->Get_Num_Flags())];
	  // First, find all the non-zero states
	  nNZCount = 0;
	  for (nState = 0; nState < PowerOfTwo(P_MSCT->Get_Num_Flags());
	       nState++)
	    if (P_P_nStateLevel[nCTIndex][nState] != 0)
	      {
		P_nNZState[nNZCount] = nState;
		nNZCount++;
	      }
	  nNonZero = nNZCount;
	  
	  while(nNumComplexesToAdd < nNumCoords)
	    {
	      flRandom = P_Ran_Gen->New_Num();
	      flThreshold = 0;
	      
	      // First, calculate the sum of all non-zero states
	      nNZLevelSum = 0;
	      // Loop through all the non-zero states
	      for (nNZCount = 0; nNZCount < nNonZero; nNZCount++)
		{
		  // If this state is still non-zero (see below)
		  if (P_nNZState[nNZCount] != -1)
		    {
		      // Add to the sum of non-zero levels
		      nNZLevelSum +=
			P_P_nStateTotalLevel[nCTIndex][P_nNZState[nNZCount]];
		    }
		}
	      // Loop through all the non-zero states
	      for (nNZCount = 0; nNZCount < nNonZero; nNZCount++)
		{
		  // If this state is still non-zero (see below)
		  if (P_nNZState[nNZCount] != -1)
		    {
		      // Increase probability threshold in proportion to ratio
		      // (total level of this state) : (sum of non-zero states)

		      flThreshold
			+= ((double)
			    P_P_nStateTotalLevel[nCTIndex][P_nNZState[nNZCount]]			    / nNZLevelSum);
		      if (flRandom < flThreshold)
			{
			  P_nInsStateLevel[P_nNZState[nNZCount]]++;
			  P_P_nStateLevel[nCTIndex][P_nNZState[nNZCount]]--;
			  // If this state has reached zero, 
			  if (P_P_nStateLevel[nCTIndex][P_nNZState[nNZCount]]
			      == 0)
			    {
			      // Mark this entry of NZState to false value
			      P_nNZState[nNZCount] = -1;
			    }
			  nNumComplexesToAdd++;
			  break;
			}
		    }
		}
/*
	      for (nState = 0; nState < PowerOfTwo(P_MSCT->Get_Num_Flags());
		   nState++)
		{
		  if (P_P_nStateTotalLevel[nCTIndex][nState] != 0)
		    {
		      flThreshold
			+= ((double) P_P_nStateTotalLevel[nCTIndex][nState]
			    / nComplexLevel);
		    }
		  if (flRandom < flThreshold)
		    {
		      P_nInsStateLevel[nState]++;
		      P_P_nStateLevel[nCTIndex][nState]--;
		      nRemainingLevel--;
		      nNumComplexesToAdd++;
		      break;
		    }
		}
*/
	    }

	  // Fill array with complexes in proportions calculated above

	  nState = 0;
	  nCountComplex = 0;
	  Address.Set(0, 0);  // Set address variable to (0,0)
	  while(nNumComplexesToAdd > 0)
	    {
	      // Skip this state if initial concentration is zero
	      while (P_nInsStateLevel[nState] == 0)
		nState++;
	
	      for (nCountComplex = 0; nCountComplex < m_nMaxNumComplexes;
		   nCountComplex ++)
		{
		  // Look for next complex in system which matches
		  // the complex type of the array
		  if(P_Complex[nCountComplex]->Get_Type() == P_MSCT
		     && (((C_Complex_Multistate*)P_Complex[nCountComplex])
			 -> Get_State() == nState)
		     && (((C_Complex_Neighbour_Sensitive*)
			  P_Complex[nCountComplex])
			 -> Get_IsArrayMember() == FALSE))
		    {
		      break;
		    }
		}
	      // If the maximum number of complexes has been reached...
	      if (nCountComplex == m_nMaxNumComplexes)
		{
		  LongToBitString(nState, lpszBitString,
				  P_MSCT->Get_Num_Flags());
		  sprintf(lpszParaList, "%s%s%s%s%s%s%ld",
			  P_MSCT->Display_String(),
			  PARAMETER_LIST_SEPARATOR, lpszBitString,
			  PARAMETER_LIST_SEPARATOR, lpszArrayName[nArrayCount],
			  PARAMETER_LIST_SEPARATOR, 
			  P_MSCT->Get_State_Level(nState));
		  m_pApp->Message(MSG_TYPE_ERROR, 193, lpszParaList);
		  return FALSE;
		}

	      // Insert this complex into complex aray
	      if(!(P_Complex_Array[nArrayCount]
		   -> Insert_Complex(&Address, 
				     (C_Complex_Multistate*)
				     P_Complex[nCountComplex])))
		{
		  // Output error if insertion fails
		  sprintf(lpszParaList, "%ld%s%ld%s%s", Address.nX,
			  PARAMETER_LIST_SEPARATOR, Address.nY,
			  PARAMETER_LIST_SEPARATOR,
			  lpszArrayName[nArrayCount]);
		  m_pApp->Message(MSG_TYPE_ERROR, 126, lpszParaList);
		  return FALSE;
		}
	      // On Success of insertion...
	      // Decrement number of complexes of this state to be added
	      P_nInsStateLevel[nState]--;
	      // Decrement number of complexes  to be added to array
	      nNumComplexesToAdd--;
	      
	      // Increment address variable
	      if (Address.nY != Dimensions.nY - 1)
		Address.nY++;
	      else
		{
		  Address.nY = 0;
		  Address.nX++;
		}
	      if(nCountComplex >= m_nMaxNumComplexes)
		{
		  m_pApp->Message(MSG_TYPE_ERROR, 125,
				lpszArrayName[nArrayCount]);
		  return FALSE;
		}
	    }

	  // Check for errors

	  if (Address.nX != Dimensions.nX)
	    {
	      sprintf(lpszParaList, "%s%s%ld%s%ld",
		      P_Complex_Array[nArrayCount]->Get_Name(),
		      PARAMETER_LIST_SEPARATOR, Address.nX,
		      PARAMETER_LIST_SEPARATOR, Address.nY);
	      m_pApp->Message(MSG_TYPE_ERROR, 133, lpszParaList);
	    }

	  // Randomise order of complexes in array

	  // Loop through all of the addresses in the array
	  for (i = 0; i < Dimensions.nX; i++)
	    for (j = 0; j < Dimensions.nY; j++)
	      {
		// Set variable to current address
		Address.Set((long) i, (long) j);
		nRandom = P_Ran_Gen->New_Num(nNumCoords);
		if(nRandom < 0 || nRandom >= nNumCoords)
		  {
		    m_pApp->Message(MSG_TYPE_ERROR, 131,
				  P_Complex_Array[nArrayCount]->Get_Name());
		    return FALSE;
		  }
		RandAddress.Set(nRandom % Dimensions.nX,
				nRandom / Dimensions.nX);
	  
		// Get a pointer to the complex in the destination address
		P_Complex_Temp
		  = P_Complex_Array[nArrayCount]->Get_Complex(&Address);
		// Move the complex from the origin to destination address
		P_Complex_Array[nArrayCount]
		  -> Insert_Complex(&Address,
				    P_Complex_Array[nArrayCount] 
				    -> Get_Complex(&RandAddress));
		// Move the complex that was in the dest address to the origin
		P_Complex_Array[nArrayCount] 
		  -> Insert_Complex(&RandAddress, P_Complex_Temp);
	      }

#ifdef _FIXED_COORD_TEST
	  /*** THIS SECTION OF CODE IS USED FOR FIXED COORDINATE TESTS ***/
	  // *** DO NOT USE IT, UNLESS YOU KNOW WHAT YOU'RE DOING!! ***
	  long nStateBuff;
	  // Loop through all of the addresses in the array
	  for (i = 0; i < Dimensions.nX; i++)
	    for (j = 0; j < Dimensions.nY; j++)
	      {
		Address.Set((long) i, (long) j);	
		// Get the state of this coordinate
		nStateBuff
		  = (((C_Complex_Multistate*)
		      (P_Complex_Array[nArrayCount]->Get_Complex(&Address)))
		     -> Get_State());
		// Clear all methylation flags by ANDing with complement
		// of mask
		nStateBuff &= 1807;
		if (i == 21 && j == 22)
		  // Set the m4 flag for the coordinate (21, 22)
		  nStateBuff |= 16;
		else if (i == 42 && j == 44)
		  // Set the asp flag for the coordinate (42, 44)
		  nStateBuff |= 1024;
		else
		  // Set m2 flag for all other coordinates 
		  nStateBuff |= 64;
		// Update state of complex to modified state
		((C_Complex_Multistate*)
		 (P_Complex_Array[nArrayCount]->Get_Complex(&Address)))
		  ->Set_State(nStateBuff);
	      }
	  // *** END OF CODE FOR FIXED COORDINATE TEST ***/
#endif /* _FIXED_COORD_TEST */
      

	  // Check for errors
      
	  // Loop through all of the addresses in the array
	  for (i = 0; i < Dimensions.nX; i++)
	    for (j = 0; j < Dimensions.nY; j++)
	      {
		Address.Set((long) i, (long) j);	
		if (P_Complex_Array[nArrayCount]->Get_Complex(&Address) == NULL)
		  {
		    sprintf(lpszParaList, "%ld%s%ld%s%s", Address.nX,
			    PARAMETER_LIST_SEPARATOR, Address.nY,
			    PARAMETER_LIST_SEPARATOR,
			    P_Complex_Array[nArrayCount]->Get_Name());
		    m_pApp->Message(MSG_TYPE_ERROR, 132, lpszParaList);
		    return FALSE;
		  }
#ifdef _CA_DEBUG // For debugging complex arrays
		else
		  {
		    // Set the address of each complex explicitly
		    ((C_Complex_Neighbour_Sensitive*)
		     P_Complex_Array[nArrayCount] -> Get_Complex(&Address))
		      ->m_pAddress -> Set(Address.nX,Address.nY);
		  }
#endif /* _CA_DEBUG */
	      }
	  break;


	  // This subroutine of this switch construct initialises the complex
	  // array based on a specified dump file.

	case ARRAY_FROM_DUMP_NUM:

	  // Allocate memory for storing the array state from dump file
	  P_P_nState
	    = new long* [Dimensions.nX];
	  for (i = 0; i < Dimensions.nX; i++)
	    {
	      P_P_nState[i]
		= new long [Dimensions.nY];
	    }
	  //Convert to Multistate Complex Type pointer
	  // (This is a bit awkward, but we don't have any use for normal
	  //  C_Complex objects in the complex array at the moment!)
	  P_MSCT = (P_Complex_Array[nArrayCount] -> Get_Type());

	  // Get the dump file name for initialising this array, and the
	  // time in the dumpfile to use for the initial state

	  // Get Array INI filename
	  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE,
			     lpszArrayFile))
	    return FALSE;
	  // Get DUMP filename
	  if (!m_pApp->Get_INI(lpszArrayName[nArrayCount],
			     INI_ARRAY_INITIAL_STATE_FILE,
			     lpszDumpFile, lpszArrayFile))
	    return FALSE;
	  // Get the time to use
	  if (!m_pApp->Get_INI(lpszArrayName[nArrayCount],
			     INI_ARRAY_INITIAL_STATE_FROM_TIME,
			     lpszInitialStateTime, lpszArrayFile))
	    return FALSE;
	  flInitialStateTime = atof(lpszInitialStateTime);

	  /*
	  // Get complex levels for this complex and each state of this complex

	  // Get Complex INI filename
	  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE,
			     lpszComplexFile))
	    return FALSE;

	  // Get complex level for this complex
	  if (!m_pApp->Get_INI(INI_INITIAL_LEVELS_SECTION,
			     P_MSCT->Display_String(),
			     &nComplexLevel, lpszComplexFile))
	    return FALSE;

	  // Get INI filename for this multistate complex type
	  strcpy(lpszParameter, P_MSCT->Display_String());
	  strcat(lpszParameter, INI_SPECIAL_TYPES_FILE);
	  if (!m_pApp->Get_INI(INI_SPECIAL_TYPES_SECTION, lpszParameter,
			     lpszMultistateFile, lpszComplexFile))
	    return FALSE;
		
	  // Loop through all of the states of this complex type
	  for (nState = 0; nState < PowerOfTwo(P_MSCT->Get_Num_Flags());
	       nState++)
	    {
	      // Convert state value to bit string
	      LongToBitString(nState, lpszBitString, P_MSCT->Get_Num_Flags());
	      // Get complex level for this state from INI file
	      if (!m_pApp->Get_INI(INI_INITIAL_STATES_SECTION, lpszBitString,
				 &P_nStateLevel[nState], lpszMultistateFile,
				 TRUE))
		// Set to default concentration if not specified in INI file
		P_nStateLevel[nState] = MS_DEFAULT_CONC;
	    }
	  */


	  // Construct full path name for dump input
	  sprintf(lpszDumpFilePath,"%s%s%s",m_pApp->m_lpszWorkingDir,
		  FILE_PATH_SEPARATOR, lpszDumpFile);
	  // Try to get the state of the complex array from dump file
	  if(!(P_Complex_Array[nArrayCount] 
	       -> Get_State_From_Dump(lpszDumpFilePath, flInitialStateTime,
				      P_P_nState)))
	    {
	      // Output error if retrieval failed
	      m_pApp->Message(MSG_TYPE_ERROR, 186, 2, lpszArrayName[nArrayCount],
			    lpszDumpFilePath);
	      return FALSE;
	    }

	  // Loop through coordinates in the array and fill it with complexes
	  // according to the state of the array read from the dump file.

	  nNumComplexesToAdd = nNumCoords;
	  for (i = 0; i < Dimensions.nX; i++)
	    for (j = 0; j < Dimensions.nY; j++)
	      {
		// Set address variable to (i,j)
		Address.Set(i, j);
		// Look for next uninserted complex of this type in the system
		// which matches the state of the current coordinate
		for (nCountComplex = 0; nCountComplex < m_nMaxNumComplexes;
		     nCountComplex++)
		  {
		    if (P_Complex[nCountComplex]->Get_Type() == P_MSCT
			&& (((C_Complex_Multistate*)P_Complex[nCountComplex])
			    -> Get_State() == P_P_nState[i][j])
			&& (((C_Complex_Neighbour_Sensitive*)
			     P_Complex[nCountComplex])
			    -> Get_IsArrayMember() == FALSE))
		      {
			break;
		      }
		  }
		// If the maximum number of complexes has been reached...
		if (nCountComplex == m_nMaxNumComplexes)
		  {
		    // Output error
		    LongToBitString(P_P_nState[i][j], lpszBitString,
				    P_MSCT->Get_Num_Flags());
		    sprintf(lpszParaList, "%s%s%s%s%s%s%s%s%ld",
			    P_MSCT->Display_String(),
			    PARAMETER_LIST_SEPARATOR, lpszBitString,
			    PARAMETER_LIST_SEPARATOR,
			    lpszArrayName[nArrayCount],
			    PARAMETER_LIST_SEPARATOR, lpszDumpFilePath,
			    PARAMETER_LIST_SEPARATOR, 
			    P_MSCT->Get_State_Level(P_P_nState[i][j]));
		    m_pApp->Message(MSG_TYPE_ERROR, 185, lpszParaList);
		    return FALSE;
		  }
	      
		// Insert this complex into complex aray
		if (!(P_Complex_Array[nArrayCount]
		      -> Insert_Complex(&Address,
					(C_Complex_Multistate*)
					P_Complex[nCountComplex])))
		  {
		    // Output error if insertion fails
		    sprintf(lpszParaList, "%d%s%d%s%s", i,
			    PARAMETER_LIST_SEPARATOR, j,
			    PARAMETER_LIST_SEPARATOR,
			    lpszArrayName[nArrayCount]);
		    m_pApp->Message(MSG_TYPE_ERROR, 126, lpszParaList);
		    return FALSE;
		  }
	      }

	  // Check for errors

	  // Loop through all of the addresses in the array and check for NULLs
	  for (i = 0; i < Dimensions.nX; i++)
	    for (j = 0; j < Dimensions.nY; j++)
	      {
		Address.Set((long) i, (long) j);	
		if (P_Complex_Array[nArrayCount]->Get_Complex(&Address)
		    == NULL)
		  {
		    sprintf(lpszParaList, "%ld%s%ld%s%s", Address.nX,
			    PARAMETER_LIST_SEPARATOR, Address.nY,
			    PARAMETER_LIST_SEPARATOR,
			    P_Complex_Array[nArrayCount]->Get_Name());
		    m_pApp->Message(MSG_TYPE_ERROR, 132, lpszParaList);
		  }
#ifdef _CA_DEBUG // For debugging complex arrays
		else
		  {
		    // Set the address of each complex explicitly
		    ((C_Complex_Neighbour_Sensitive*)
		     P_Complex_Array[nArrayCount] -> Get_Complex(&Address))
		      ->m_pAddress -> Set(Address.nX,Address.nY);
		  }
#endif /* _CA_DEBUG */
	      }
	  for(i = Dimensions.nX - 1; i >= 0; i--)
	    if (P_P_nState[i] != NULL)
	      delete[] P_P_nState[i];
	  if (P_P_nState != NULL)
	    delete[] P_P_nState;
	  break;

	  // *** THIS SUBROUTINE IS PRESENTLY UNUSED ***
	  // This is the default subroutine of this switch construct.
	  // It will only check that the complex type of the complex being
	  // inserted matches that of the complex array, and ignores the
	  // internal state of each complex.
	default:
	  P_MSCT = P_Complex_Array[nArrayCount]->Get_Type();
	  nCountComplex = 0;

	  // Loop through all of the addresses in the array
	  for (i = 0; i < Dimensions.nX; i++)
	    for (j = 0; j < Dimensions.nY; j++)
	      {
		// Set variable to current address
		Address.Set((long) i, (long) j);
		// Look for next complex in system which matches
		// the complex type of the array
		while(P_Complex[nCountComplex]->Get_Type()->Get_Index()
		      != P_MSCT->Get_Index())
		  {
		    nCountComplex++;
		    if(nCountComplex >= m_nMaxNumComplexes)
		      {
			m_pApp->Message(MSG_TYPE_ERROR, 125,
				      lpszArrayName[nArrayCount]);
			return FALSE;
		      }
		  }
		if(!(P_Complex_Array[nArrayCount]
		     -> Insert_Complex(&Address, (C_Complex_Multistate*)
				       P_Complex[nCountComplex])))
		  {
		    sprintf(lpszParaList, "%ld%s%ld%s%s", Address.nX,
			    PARAMETER_LIST_SEPARATOR, Address.nY,
			    PARAMETER_LIST_SEPARATOR,
			    lpszArrayName[nArrayCount]);
		    m_pApp->Message(MSG_TYPE_ERROR, 126, lpszParaList);
		    return FALSE;
		  }
		nCountComplex++;
	      }
	}
      delete[] P_nInsStateLevel;
      
      if(!P_Complex_Array[nArrayCount]->Init())
	return FALSE;
    }
  // Free allocated memory
  for (i = m_nNumComplexTypes - 1; i >= 0; i--)
    if (P_P_nStateLevel[i] != NULL)
      delete[] P_P_nStateLevel[i];
  if (P_P_nStateLevel != NULL)
    delete[] P_P_nStateLevel;
  for (i = m_nNumComplexTypes - 1; i >= 0; i--)
    if (P_P_nStateTotalLevel[i] != NULL)
      delete[] P_P_nStateTotalLevel[i];
  if (P_P_nStateTotalLevel != NULL)
    delete[] P_P_nStateTotalLevel;

  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Init_Complex_Types_Post
*
* DESCRIPTION:	Each complex type in the system is initialised.
*		Pre-initialisation (ie construction) has already occurred
*		(see Init_Complex_Types, Init_Complex_Types_Special,
*		Init_Complex_Types_Fuzzy).
*
* RETURNS:	Bool	TRUE	- Complex types were initialised successfully
*			FALSE	- An error occurred initialising complex types
*
*************************************************************************/

Bool
C_System::Init_Complex_Types_Post(void)
{
  int	nCount;
  // Loop through all the complex types
  for (nCount = 0; nCount < m_nNumComplexTypes; nCount ++)
    // Call its initialisation method. If it fails, output error
    if (!P_Complex_Type[nCount]->Init(P_Complex, m_nMaxNumComplexes))
      {
	m_pApp->Message(MSG_TYPE_ERROR, 84,
		      P_Complex_Type[nCount]->Display_String());
	return FALSE;
      }
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Init_Event_Manager
*
* DESCRIPTION:	Initialises the event manager. This is called after the
*		events have been created.
*
* PARAMETERS:	int nTimeUnits - 0 => Time being stored in INIs as iterations
*				 1 => Time is specified in INIs as seconds
*
* RETURNS:	Bool TRUE	- Initialisation was successful.
*		     FALSE	- An error occurred during initialisation.
*
*************************************************************************/

Bool
C_System::Init_Event_Manager(int nTimeUnits)
{
  P_Event_Manager->Init(nTimeUnits, m_flTimeInc);
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Init_Output_Managers
*
* DESCRIPTION:	Initialises the output managers. This must be
*               called after all variables have been created and the
*               time-dependent parameters have been set.
*
* PARAMETERS:	int nTimeUnits - 0 => Time being stored in INIs as iterations
*				 1 => Time is specified in INIs as seconds
*
* RETURNS:	Bool TRUE	- Initialisation was successful.
*		     FALSE	- An error occurred during initialisation.
*
*************************************************************************/

Bool
C_System::Init_Output_Managers(long nTimeUnits)
{
  long i;

  for (i = 0; i < m_nNumOutputManagers; i ++)
    if(!P_Output_Manager[i]->Init(nTimeUnits, m_flTimeInc, m_nNumIterations))
      return FALSE;

  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Init_Spatial_Extensions
*
* DESCRIPTION:	Gets information about the spatial extensions being used from
*               the INI files.
*
* PARAMETERS:	NONE
*
* RETURNS:	Bool TRUE	- Initialisation was successful.
*		     FALSE	- An error occurred during initialisation.
*
*************************************************************************/

Bool
C_System::Init_Spatial_Extensions(void)
{

  // Presently, the only spatial extension implemented is the complex array.
  // Therefore, this attribute must be TRUE if spatial extensions
  // are being used
  m_bUsingArrays = TRUE;

  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Init_Dump_Variables
*
* DESCRIPTION:	Initialises the variables used to store a complete set
*		of complex levels to a dump file. The method Get_Variables 
*		is called for each Complex Type, which is itself responsible 
*		for creating the dump variables.
*
* RETURNS:	Bool	TRUE	- Variables were initialised successfully
*			FALSE	- An error occurred initialising variables
*
*************************************************************************/

Bool
C_System::Init_Dump_Variables(void)
{
  C_Output_Variable*	P_Variable[MAX_NUM_DUMP_VARIABLES];
  long			nCountCT;
  long			nCountVar;
  long			nNumDumpVar;
  long			nCreateDumpFile;

  // Retrieve create dump file flag from INI file
  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_CREATE_DUMP_FILE,
		     &nCreateDumpFile))
    return FALSE;

  // If no dump file is going to be created, return without
  // creating the dump variables
  if (!nCreateDumpFile)
    return TRUE;

  // Loop through all the complex types
  for (nCountCT = 0; nCountCT < m_nNumComplexTypes; nCountCT ++)
    {
      // Retrieve pointers to dump variables from complex type
      if (!P_Complex_Type[nCountCT]->Get_Variables(P_Variable,
						   &nNumDumpVar,
						   MAX_NUM_DUMP_VARIABLES))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 96,
			P_Complex_Type[nCountCT]->Display_String());
	  return FALSE;
	}
      // For each of the returned dump variables
      for (nCountVar = 0; nCountVar < nNumDumpVar; nCountVar ++)
	{
	  // Add to the output manager's list of dump variables
	  if (!P_Output_Manager[OM_COMPLEX]
	      -> Add_DumpVariable(P_Variable[nCountVar]))
	      return FALSE;
	}
    }
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Init_Display_Variables
*
* DESCRIPTION:	Initialises the variables used to output complex levels to
*		be displayed. Either variables are created for all the 
*		complex types, or only particular specified variables are
*		created (depending on flag in INI file). If the former is
*		the case, all the complex types are interrogated set up
*		the variables. If the latter is the case, the variable
*		information is read in from the INI file.
*
* RETURNS:	Bool	TRUE	- Variables were initialised successfully
*			FALSE	- An error occurred initialising variables
*
*************************************************************************/

Bool
C_System::Init_Display_Variables(void)
{
  C_Output_Variable*	P_Variable[MAX_NUM_DISPLAY_VARIABLES];
  C_Complex_Type*	P_CT_List[MAX_TYPES_IN_OUTPUT_VAR];
  char			lpszVarFile[MAX_INI_VALUE_LENGTH];
  char			lpszSection[MAX_INI_SECTION_LENGTH];
  char			lpszDispVarName[MAX_INI_VALUE_LENGTH];
  char			lpszDispVarTypes[MAX_INI_VALUE_LENGTH];
  char*			lpszPos;
  long			nCountCT;
  long			nCountVar;
  long			nNumDisplayVar;
  long			nNumAdditionalVar;
  long			nDispAllLevels;
  long			nNumTypes;
  char			lpszArrayFile[MAX_INI_VALUE_LENGTH];
  long			nAveraging;
  Bool			bAveraging;

  nNumDisplayVar = 0;

  // Get name of file containing variable information
  // (ie the complex INI file name)
	
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_COMPLEX_INI_FILE, lpszVarFile))
    return FALSE;

  // Retrieve flag indicating whether or not the levels
  // of all the complex types should be displayed
  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_DISPLAY_ALL_LEVELS,
		     (long*) &nDispAllLevels))
    return FALSE;

  // If all levels should be displayed, create list of display variables
  // by interrogating every complex type

  if (nDispAllLevels != 0)
    {
      // Loop through all the complex types
      for (nCountCT = 0; nCountCT < m_nNumComplexTypes; nCountCT ++)
	{
	  // Retrieve pointers to display variables from complex type
	  if (!(P_Complex_Type[nCountCT] 
		-> Get_Variables(P_Variable, &nNumDisplayVar,
				 MAX_NUM_DISPLAY_VARIABLES)))
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 34, 1,
			    P_Complex_Type[nCountCT] -> Display_String());
	      return FALSE;
	    }
	  // For each of the returned display variables
	  for (nCountVar = 0; nCountVar < nNumDisplayVar; nCountVar ++)
	    {
	      // Add the variable pointer to the Output Manager's list
	      // of display variables
	      if (!P_Output_Manager[OM_COMPLEX]
		  -> Add_DisplayVariable(P_Variable[nCountVar]))
		{
		  return FALSE;
		}
	    }
	}
      return TRUE;
    }

  // Otherwise, the list of display variables should be created
  // by information in the INI file

  // Get the number of display variables
  if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_NUM_DISPLAY_VAR,
		     &nNumDisplayVar, lpszVarFile))
    return FALSE;

  // Ensure that the number of display variables are within allowed range
  if ((nNumDisplayVar < 0) || (nNumDisplayVar > MAX_NUM_DISPLAY_VARIABLES))
    {
      m_pApp->Message(MSG_TYPE_ERROR, 33);
      return FALSE;
    }
	
  // Check whether output should be averaged
  // (This information can be stored in the Array INI file to avoid the GUI)
  
  // First, look in the system INI file
  if (!m_pApp->Get_INI(INI_OPTIONS_SECTION, INI_AVERAGED_OUTPUT,
			  &nAveraging, NULL, TRUE))
    {
      // If it wasn't defined there and spatial extentions are being used...
      if (m_bUsingSpatialExtensions)
	{
	  // Get name of Array INI file (for backwards compatibility)
	  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE,
			     lpszArrayFile, NULL, TRUE))
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
      else
	{
	  // No where else to lok if spatial extensions aren't being used...
	  nAveraging = 0;
	}
    }
  if (nAveraging != 0)
    {
      m_pApp->Message(MSG_TYPE_STATUS, 143);
      bAveraging = TRUE;
    }
  else
    {
      bAveraging = FALSE;
    }

  // For each display variable, read in the information from the INI file
  for (nCountVar = 0; nCountVar < nNumDisplayVar; nCountVar++)
    {
      // Construct section title
      sprintf(lpszSection, "%s%ld", INI_DISPLAY_VAR_SECTION, nCountVar + 1);
      // Read name of display variable
      if (!m_pApp->Get_INI(lpszSection, INI_DISPLAY_VAR_NAME, lpszDispVarName,
			 lpszVarFile))
	return FALSE;
      // Read types this display variable represents
      if (!m_pApp->Get_INI(lpszSection, INI_DISPLAY_VAR_TYPES, lpszDispVarTypes,
			 lpszVarFile))
	return FALSE;

      // Identify the types this variable represents from the list

      nNumTypes = 0;
      // Find first type
      lpszPos = strtok(lpszDispVarTypes, INI_VAR_TYPES_SEPARATOR);
      // While the end of the list is not reached...
      while ((lpszPos != NULL) && (nNumTypes < MAX_TYPES_IN_OUTPUT_VAR))
	{
	  // Add the complex type to a list
	  P_CT_List[nNumTypes] = Identify_Complex_Type(lpszPos);
	  nNumTypes ++;
	  // Find next complex type in list
	  lpszPos = strtok(NULL, INI_COMPLEX_SEPARATOR);
	}
      // If the end of the list wasn't reached, output error
      if (lpszPos != NULL)
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 35, lpszDispVarName);
	  return FALSE;
	}

		
      // Create the output variable using the list of complex types
		
      // If we are averaging output...
      if (bAveraging)
	{
	  // ...create an averaged output variable
	  P_Variable[nCountVar]
	    = new C_Output_Variable_Complex_Averaged(m_pApp, lpszDispVarName,
						     P_CT_List, nNumTypes);
	  if (!P_Output_Manager[OM_COMPLEX]
	      -> Add_DisplayVariable(P_Variable[nCountVar]))
	    {
	      return FALSE;
	    }
	}
      // Otherwise...
      else
	{
	  // Create a normal output variable
	  P_Variable[nCountVar]
	    = new C_Output_Variable_Complex(m_pApp, lpszDispVarName, P_CT_List,
					    nNumTypes);
	  if (!P_Output_Manager[OM_COMPLEX]
	      -> Add_DisplayVariable(P_Variable[nCountVar]))
	    {
	      return FALSE;
	    }
	}
    }

  // Allow the complex types to create additional display variables
  // as necessary (this is important for the special complex types)

  // Loop through all the complex types
  for (nCountCT = 0; nCountCT < m_nNumComplexTypes; nCountCT ++)
    {
      // Retrieve pointers to display variables from complex type
      if (!(P_Complex_Type[nCountCT]
	    -> Get_Variables_From_File(P_Variable, &nNumAdditionalVar,
				       MAX_NUM_DISPLAY_VARIABLES, bAveraging)))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 109,
			P_Complex_Type[nCountCT] -> Display_String());
	  return FALSE;
	}
      // For each of the returned display variables
      for (nCountVar = 0;
	   nCountVar < nNumAdditionalVar;
	   nCountVar ++)
	{
	  // If too many display variables have been allocated,
	  // return out of memory error
	  /*	  if (nNumDisplayVar >= MAX_NUM_DISPLAY_VARIABLES)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 99);
	      return FALSE;
	      }*/
	  // Add the variable pointer to the Output Manager's list of
	  // display variables
	  if (!P_Output_Manager[OM_COMPLEX]
	      -> Add_DisplayVariable(P_Variable[nCountVar]))
	    {
	    return FALSE;
	    }
	  // Update the number of display variables in the system
	  //nNumDisplayVar ++;
	}
    }

  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Init_Array_Variables
*
* DESCRIPTION:	Initialises the variables used to store snapshots or to
*               dump the complete state of the complex arrays during
*               simulation. 
*
* RETURNS:	Bool	TRUE	- Variables were initialised successfully
*			FALSE	- An error occurred initialising variables
*
*************************************************************************/

Bool
C_System::Init_Array_Variables(void)
{
  long                   nAveraging;
  Bool                   bAveraging;
  char		         lpszParaList[MAX_MESSAGE_LENGTH];
  ostringstream ParaListStream;
  char                   lpszSection[MAX_INI_SECTION_LENGTH];
  double	         flStoreInterval;
  double	         flSampleInterval;
  double	         flAveInterval;
  unsigned long long          nStoreItvl;
  unsigned long long          nSampleItvl;
  unsigned long long          nAveItvl;
  C_Output_Variable*	 P_DumpVariable;
  C_Output_Variable*     P_SnapsVariable[MAX_NUM_ARRAY_SNAPS_VARIABLES];
  char			 lpszFileName[MAX_INI_VALUE_LENGTH];
  char			 lpszArrayVarName[MAX_ARRAY_VAR_NAME_LENGTH];
  long			 nCountArray;
  long			 nCountVar;
  long			 nNumSnapsVar;
  long			 nNumSnapsVarCreated;
  long			 nCreateDumpFile;
  long                   nNumVar;
  // Pointer for type conversion
  C_Complex_Type_Multistate*	p_MSCT;

  // Retrieve file name of complex array INI file
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE, lpszFileName))
    {
      m_pApp->Message(MSG_TYPE_ERROR, 28, 3, INI_ARRAY_INI_FILE,
		    INI_FILE_NAME_SECTION, APP_INI_FILE_NAME);
      return FALSE;
    }
  // Get the number of variables from the file
  if (!m_pApp->Get_INI(INI_ARRAY_GENERAL_SECTION, INI_ARRAY_NUM_SNAPS_VAR,
		     &nNumVar, lpszFileName))
      return FALSE;

  // Set number of snapshot variables to zero
  nNumSnapsVar = 0;
  nNumSnapsVarCreated = 0;

  // For each array in the system...
  for (nCountArray = 0; nCountArray < m_nNumArrays; nCountArray++)
    {
      // Check if dump file needs to be created 
      
      // Retrieve create dump file flag from the complex array INI file
      if (!m_pApp->Get_INI(P_Complex_Array[nCountArray]->Get_Name(),
			 INI_ARRAY_CREATE_DUMP_FILE, &nCreateDumpFile,
			 lpszFileName, TRUE))
	// Dump file will not be created if not specified
	  nCreateDumpFile = 0;

      // If the dump file is going to be created...
      if (nCreateDumpFile != 0)
	{
	  // Format name of variable
	  sprintf(lpszArrayVarName, ARRAY_NAME_FORMAT_STRING,
		  P_Complex_Array[nCountArray]->Get_Name(),
		  ARRAY_OUTPUT_DUMP_SUFFIX);
	  //Create new dump variable
	  P_DumpVariable
	    = new C_Output_Variable_Array(m_pApp, lpszArrayVarName,
					  P_Complex_Array[nCountArray]);
	  // Add the variable to the output manager's list of
	  // snapshot variables
	  if (!P_Output_Manager[OM_ARRAY] -> Add_DumpVariable(P_DumpVariable))
	    {
	      return FALSE;
	    }
	}

      // Create any snapshot variables that are requested in INI file
      
      // Force complexes to be treated as multi-state complex types
      // (unsafe: should be changed in the future)
      p_MSCT = ((C_Complex_Type_Multistate*)
		(P_Complex_Array[nCountArray] -> Get_Type()));

      // Retrieve pointers to snapshot variables from complex array
      if (!(P_Complex_Array[nCountArray]
	    -> Get_Variables_From_File(P_SnapsVariable, &nNumSnapsVar,
				       MAX_NUM_ARRAY_SNAPS_VARIABLES)))
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 158, 1,
			P_Complex_Array[nCountArray] -> Get_Name());
	  return FALSE;
	}
      // For each of the returned display variables
      for (nCountVar = nNumSnapsVarCreated;
	   nCountVar < nNumSnapsVar;
	   nCountVar++)
	{
	  // Collect array output information from INI files... -tss
	    
	  // Construct section title
	  sprintf(lpszSection, "%s%ld", INI_ARRAY_SNAPS_VAR_SECTION,
		  nCountVar+1);
	  // Get value store frequency for array snapshots from file
	  if (!m_pApp->Get_INI(lpszSection, INI_STORE_INTERVAL,
			     &flStoreInterval, lpszFileName, TRUE))
	    {
	      // If not specified, get default value
	      if (!m_pApp->Get_INI(INI_GENERAL_SECTION,
				 INI_DEFAULT_STORE_INTERVAL,
				 &flStoreInterval, lpszFileName, TRUE))
		{
		  // For backwards compatibility
		  if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_STORE_INTERVAL,
				     &flStoreInterval, lpszFileName, TRUE))
		    {
		      // For backwards compatibility
		      if (!m_pApp->Get_INI(INI_GENERAL_SECTION, INI_STORE_FREQ,
					 &flStoreInterval, lpszFileName))
			return FALSE;
		    }
		}
	      // Output message to warn that default value is being used
	      ParaListStream << std::fixed << flStoreInterval;
	      ParaListStream << PARAMETER_LIST_SEPARATOR << nCountVar << std::ends;
	      m_pApp->Message(MSG_TYPE_STATUS, 164, ParaListStream.str().c_str());
	    }
	    
	  // Convert store interval to iterations (minimum 1 iteration)
	  nStoreItvl = MAX((unsigned long long) (flStoreInterval / m_flTimeInc), 1);
	    
	  // If the output averaging parameter is not specified
	  if ((!m_pApp->Get_INI(lpszSection, INI_AVERAGED_OUTPUT,
			      &nAveraging, lpszFileName, TRUE)))
	    {
	      // No output averaging will be performed, so set flag and
	      // interval values for averaging to false values
	      nAveItvl = 0;
	      nSampleItvl = 0;	
	      bAveraging = FALSE;
	    }
	  // Otherwise, the averaging flag is specified in the INI file
	  else
	    {
	      // If the Averaging parameter is non-zero...
	      if (nAveraging != 0)
		{
		  // Get the avergagin and sampling intervals
		  if ((!m_pApp->Get_INI(lpszSection, INI_ARRAY_AVERAGE_INTERVAL,
				      &flAveInterval, lpszFileName)))
		    return FALSE;
		  if(!m_pApp->Get_INI(lpszSection, INI_SAMPLE_INTERVAL,
				    &flSampleInterval, lpszFileName))
		    return FALSE;
		  
		  // If the value of the averaging interval parameter is zero,
		  if (flAveInterval == 0)
		    {
		      // No output averaging will be performed, so set interval
		      // and frequency values for averaging to false values
		      nAveItvl = 0;
		      nSampleItvl = 0;	
		      bAveraging = FALSE;
		      // Output warning message
		      m_pApp->Message(MSG_TYPE_STATUS, 165, 1, nCountVar);
		    }
		  else  // Otherwise, output averaging is desired
		    {
		      bAveraging = TRUE;
		      // Check that averaging interval is a valid value
		      if (flAveInterval < 0 || flAveInterval > flStoreInterval)
			{
			  //Format error message
			  ParaListStream.str("");
			  ParaListStream <<  std::fixed << flAveInterval;
			  ParaListStream << PARAMETER_LIST_SEPARATOR;
			  ParaListStream << nCountVar << std::ends;

			  //Output error
			  m_pApp->Message(MSG_TYPE_ERROR, 141, ParaListStream.str().c_str());
			  return FALSE;
			}
		      // Check that sampling interval is a valid value
		      if (flSampleInterval < 0
			  || flSampleInterval > flAveInterval)
			{
			  //Format error message
			  ParaListStream.str("");
			  ParaListStream <<  std::fixed << flSampleInterval;
			  ParaListStream << PARAMETER_LIST_SEPARATOR;
			  ParaListStream << nCountVar << std::ends;

			  //Output error
			  m_pApp->Message(MSG_TYPE_ERROR, 166, ParaListStream.str().c_str());
			  return FALSE;
			}

		      // Convert averaging interval to iterations
		      nAveItvl = MAX((unsigned long long) (flAveInterval
						      / m_flTimeInc), 1);
		      // Convert sampling interval to iterations
		      nSampleItvl
			= MAX((unsigned long long) (flSampleInterval
					       / m_flTimeInc), 1);
		    }
		}
	      // Otherwise averaging parameter was zero, so...
	      else
		{
		  // No output averaging will be performed, so set interval
		  // and frequency values for averaging to false values
		  nAveItvl = 0;
		  nSampleItvl = 0;	
		  bAveraging = FALSE;
		}
	    }

	  // Add the variable to the output manager's list of
	  // snapshot variables
	  if (!P_Output_Manager[OM_ARRAY]
	      -> Add_SnapsVariable(P_SnapsVariable[nCountVar], nStoreItvl,
				   bAveraging, nSampleItvl, nAveItvl))
	    {
	      return FALSE;
	    }
	}
      nNumSnapsVarCreated += nCountVar - nNumSnapsVarCreated;
    }
  if (nNumSnapsVarCreated != nNumVar)
    {
      sprintf(lpszParaList, "%s%s%ld%s%ld", lpszFileName,
	      PARAMETER_LIST_SEPARATOR, nNumVar,
	      PARAMETER_LIST_SEPARATOR, nNumSnapsVarCreated);
      m_pApp->Message(MSG_TYPE_STATUS, 195, lpszParaList);
      return FALSE;
    }
  return TRUE;
}

