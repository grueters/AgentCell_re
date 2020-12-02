/*************************************************************************
*
* FILENAME:	System2.cpp
*
* CLASS:	C_System (see also System1.cpp)
*
* DESCRIPTION:	(See SYSTEM1.CPP for a complete description of this
*		class). Contained here are all C_System methods
*		which are not involved in initialising the system.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"
/* #include <crtdbg.h> */


/*************************************************************************
*
* METHOD NAME:	Run
*
* DESCRIPTION:	This method runs the simulation which has already been
*		initialised. The simulation is executed for the required
*		number of iterations. In each iteration, two complexes
*		are selected at random to react. If the selected complexes
*		actually exist, the C_Complex::Test_Reaction method is
*		called. Note that if the second selected reactant is
*		greater than the number of complexes in the system,
*		the reaction will be a unimolecular reaction.
*		The complex levels are recorded and displayed at regular 
*		intervals.
*
* RETURNS:	Bool	 TRUE	- System executed successfully
*			 FALSE	- Execution of system failed
*
*************************************************************************/

Bool
C_System::Run(void)
{
  ostringstream	 TimeStream;
  unsigned long long	 i;
  long	                 j;
  unsigned long long nNumIterationsInSet;
  unsigned long long nIterationsDone;
  time_t tStart;
  time_t tEnd;

  // If we are debugging and require validation of random numbers,
  // test the random number generator

#if defined(_DEBUG) && defined(__RAN_NUM_GEN_VALIDATION)
  tStart = clock();  // Record start time of test
  // Test generator and ensure no error occurs during test
  if (!P_Ran_Gen->Test(50 * 350, 350, RG_TEST_NO_OUTPUT))
    return FALSE;
  tEnd = clock();  // Record ending time of test
  TimeStream << setprecision(PREC_SIMULATION_TIME) << std::fixed;
  TimeStream << ((float) (tEnd - tStart)) / CLOCKS_PER_SEC << std::ends;
  //  Display the time taken to test generator
  m_pApp->Message(MSG_TYPE_STATUS, 75, TimeStream.str().c_str());
#endif

  // Perform operations prior to running the simulation

  // Record start time of the simulation
  tStart = clock();
  nIterationsDone = 0;

// If we are debugging NS-Complexes...
#ifdef _NSDEBUG
  ofstream Debug_Stream;
  long     nStart;

  EQUIL_COUNT = 0;
  nStart = clock();  // Start clock running
  // Repeatedly try to open file for appending output, until either the
  // file opens successfully or maximum time limit is exceeded. This is
  // necessary in case another program (eg Excel) is opening file
  // (for eg graphing)
  do
    Debug_Stream.open("NSDEBUG.txt", ios::out);

#if ( defined(_ALPHA) || defined(_AIX) )
  while ((!Debug_Stream.rdbuf()->is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#else
  while ((!Debug_Stream.is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */

  // Ensure file was opened successfully (eventually)
  if (Debug_Stream == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 2, "NSDEBUG");
      return FALSE;
    }
  Debug_Stream << "TIME\tEquilibrations" << endl;
  Debug_Stream << "0.0\t";
  Debug_Stream.close();
#endif /* _NSDEBUG */


  // Tell Output Managers to display the current values of each variable
    for (j = 0; j < m_nNumOutputManagers; j++)
      {
	if(!P_Output_Manager[j]
	   -> Display_And_Store(nIterationsDone * m_flTimeInc))
	  {
	    return FALSE;
	  }
      }

  // Start the simulation

  // Keep looping through all iterations until they have all been completed
  while (nIterationsDone < m_nNumIterations)
    {
      // If we have more than MAX_LONG iterations to go...
      if (m_nNumIterations - nIterationsDone > MAX_UNSIGNED_LONG_LONG)
	{
	  // Do MAX_LONG iterations this time
	  nNumIterationsInSet =  MAX_UNSIGNED_LONG_LONG;
	}
      // Otherwise, only do the number of iterations left to do
      else
	{
	  nNumIterationsInSet =
	    (unsigned long long) (m_nNumIterations - nIterationsDone);
	}
      if (nNumIterationsInSet == 0) break;
      // Repeat the following for the specified number of iterations...
      for (i = 0; i < nNumIterationsInSet; i++)
	{
	  if (nIterationsDone == P_Event_Manager->Get_Next_Event_Time())
	    {
	      // If an event is about to occur, execute the event
	      P_Event_Manager->Exec_Event();
	    }
	  // Perform iteration
	  if (!Run_Iteration())
	    {
	      return FALSE;
	    }
	  // If complex arrays are being used
	  if (m_bUsingArrays)
	    {
	      // Loop through all the complex arrays
	      for (int nArrayCount = 0; nArrayCount < m_nNumArrays;
		   nArrayCount++)
		{
		  // Increment equilibration counter for this array 
		  m_pnArrayEquilItCount[nArrayCount] ++;

		  // If this is an iteration in which this complex array
		  // should be equilibrated
		  if (m_pnArrayEquilItCount[nArrayCount]
		      == m_pnArrayEquilInterval[nArrayCount])
		    {
		      // Equilibrate all complex arrays
		      P_Complex_Array[nArrayCount]->Equilibrate();
		      // Reset equilibration counter for this array 
		      m_pnArrayEquilItCount[nArrayCount] = 0;
		    }
		}
	    }

	  for (j = 0; j < m_nNumOutputManagers; j++)
	    {
	      if (!P_Output_Manager[j]->Handle_Output(nIterationsDone
						      * m_flTimeInc))
		{
		  return FALSE;
		}
	    }
	  // Update the number of iterations done so far
	  nIterationsDone ++;
	}
    }
// 
  // Perform post-simulation operations

  // Record the ending time of the simulation
  tEnd = clock();
  // Tell Output Managers to display the current values of each variable
//  for (j = 0; j < m_nNumOutputManagers; j++)
//    {
//      if (!P_Output_Manager[j]
//	  -> Display_And_Store(nIterationsDone * m_flTimeInc))
//	{
//	  return FALSE;
//	}
//    }

#ifdef _NS_R_DEBUG  // For debugging neighbour-sensitive reactions
  long nReaction;
  // Print NS reaction count
  for (nReaction = 0; nReaction < 21; nReaction ++)
    {
      if(((C_Complex_Type_Neighbour_Sensitive*)P_Complex_Type[1])
	 -> Get_NS_R_Count(nReaction) != -1)
	{
	  cout << nReaction << " = " 
	       << ((C_Complex_Type_Neighbour_Sensitive*)P_Complex_Type[1])
	    -> Get_NS_R_Count(nReaction) << endl;
	}
    }
#endif /* _NS_R_DEBUG */

// Format message string
  TimeStream.str("");
  TimeStream << setprecision(PREC_SIMULATION_TIME) << std::fixed;
  TimeStream << ((float) (tEnd - tStart)) / CLOCKS_PER_SEC << std::ends;
  // Display the time taken to run the simulation
  m_pApp->Message(MSG_TYPE_STATUS, 76, TimeStream.str().c_str());
  // Return indicating no error occurred while running the simulation
  return TRUE;
}


Bool
C_System::Step(double deltaT)
{
  unsigned long long	 i;
  long	                 j;
  unsigned long long nIterationsDone;
  unsigned long long nNumIterationsInStep;
  unsigned long long nNumIterationsInSet;

  // Perform operations prior to stepping the simulation

  nIterationsDone = 0;
  nNumIterationsInStep = (unsigned long long) MAX((deltaT / m_flTimeInc), 1);
    
// If we are debugging NS-Complexes...
#ifdef _NSDEBUG
  ofstream Debug_Stream;
  long     nStart;

  EQUIL_COUNT = 0;
  nStart = clock();  // Start clock running
  // Repeatedly try to open file for appending output, until either the
  // file opens successfully or maximum time limit is exceeded. This is
  // necessary in case another program (eg Excel) is opening file
  // (for eg graphing)
  do
    Debug_Stream.open("NSDEBUG.txt", ios::out);

#if ( defined(_ALPHA) || defined(_AIX) )
  while ((!Debug_Stream.rdbuf()->is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#else
  while ((!Debug_Stream.is_open())
	 && ((clock() - nStart) / CLOCKS_PER_SEC
	     < RETRY_TIME_VALUE_FILE_OPEN));
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */

  // Ensure file was opened successfully (eventually)
  if (Debug_Stream == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 2, "NSDEBUG");
      return FALSE;
    }
  Debug_Stream << "TIME\tEquilibrations" << endl;
  Debug_Stream << "0.0\t";
  Debug_Stream.close();
#endif /* _NSDEBUG */


  // Tell Output Managers to display the current values of each variable
  if (m_nIterationsDone == 0) {
    for (j = 0; j < m_nNumOutputManagers; j++)
      {
	if(!P_Output_Manager[j]
	   -> Display_And_Store(m_nIterationsDone * m_flTimeInc))
	  {
	    return FALSE;
	  }
      }
  }

  // Start the simulation step

  // Keep looping through all iterations until they have all been completed
  while (nIterationsDone < nNumIterationsInStep)
    {
      // If we have more than MAX_LONG iterations to go...
      if (nNumIterationsInStep - nIterationsDone  > MAX_LONG_LONG)
	{
	  // Do MAX_LONG iterations this time
	  nNumIterationsInSet = MAX_LONG_LONG;
	}
      // Otherwise, only do the number of iterations left to do
      else
	{
	  nNumIterationsInSet = (long) (nNumIterationsInStep - nIterationsDone);
	}
      if (nNumIterationsInSet == 0) break;
      // Repeat the following for the specified number of iterations...
      for (i = 0; i < nNumIterationsInSet; i++)
	{
	  if (m_nIterationsDone + nIterationsDone ==
	      P_Event_Manager->Get_Next_Event_Time());
	    {
	      // If an event is about to occur, execute the event
	      P_Event_Manager->Exec_Event();
	    }
	  // Perform iteration
	  if (!Run_Iteration())
	    {
	      return FALSE;
	    }
	  // If complex arrays are being used
	  if (m_bUsingArrays)
	    {
	      // Loop through all the complex arrays
	      for (int nArrayCount = 0; nArrayCount < m_nNumArrays;
		   nArrayCount++)
		{
		  // Increment equilibration counter for this array 
		  m_pnArrayEquilItCount[nArrayCount] ++;

		  // If this is an iteration in which this complex array
		  // should be equilibrated
		  if (m_pnArrayEquilItCount[nArrayCount]
		      == m_pnArrayEquilInterval[nArrayCount])
		    {
		      // Equilibrate all complex arrays
		      P_Complex_Array[nArrayCount]->Equilibrate();
		      // Reset equilibration counter for this array 
		      m_pnArrayEquilItCount[nArrayCount] = 0;
		    }
		}
	    }

	  for (j = 0; j < m_nNumOutputManagers; j++)
	    {
	      if (!P_Output_Manager[j]->Handle_Output((m_nIterationsDone
						       + nIterationsDone)
						      * m_flTimeInc))
		{
		  return FALSE;
		}
	    }
	  // Update the number of iterations done so far
	  nIterationsDone ++;
	}
    }
// 
  m_nIterationsDone += nIterationsDone;
#ifdef _NS_R_DEBUG  // For debugging neighbour-sensitive reactions
  long nReaction;
  // Print NS reaction count
  for (nReaction = 0; nReaction < 21; nReaction ++)
    {
      if(((C_Complex_Type_Neighbour_Sensitive*)P_Complex_Type[1])
	 -> Get_NS_R_Count(nReaction) != -1)
	{
	  cout << nReaction << " = " 
	       << ((C_Complex_Type_Neighbour_Sensitive*)P_Complex_Type[1])
	    -> Get_NS_R_Count(nReaction) << endl;
	}
    }
#endif /* _NS_R_DEBUG */
  // Return indicating no error occurred while running the simulation
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Run_Iteration
*
* DESCRIPTION:	Performs a single iteration of the running simulation.
*		Two complexes are selected at random to react. If the 
*		selected complexes exist, the C_Complex::Test_Reaction
*		method is called. Note that if the second selected reactant
*		is greater than the number of complexes in the system,
*		the reaction will be a unimolecular reaction.
*
* RETURNS:	Bool	 TRUE	- Iteration executed successfully
*			 FALSE	- Execution of iteration failed
*
*************************************************************************/

inline Bool
C_System::Run_Iteration(void)
{
  int		nSpecies1;
  int		nSpecies2;
  // Select the first complex to react
  nSpecies1 = P_Ran_Gen->New_Num(m_nMaxNumComplexes);
  // If this species exists...
  if (P_Complex[nSpecies1]->Get_Exists())
    {
      // Select the second complex to react
      nSpecies2
	= P_Ran_Gen->New_Num(m_nMaxNumComplexes + m_nNumUniPseudoComplexes);
      // If the second selected complex number is greater than
      // the number of complexes in the system...
      if (nSpecies2 >= m_nMaxNumComplexes)
	{
	  // Try to react the first selected complex (ie unimolecular reaction)
	  if (!P_Complex[nSpecies1]->Test_Reaction(NULL))
	    return FALSE;
	}
      else  // Otherwise...
	{
	  // If the two selected complexes are distinct 
	  // and second complex exists, try and react them both together
	  if ((nSpecies1 != nSpecies2) && (P_Complex[nSpecies2]->Get_Exists()))
	    // Try and react with species 2 controlling the reaction
	    if (!P_Complex[nSpecies2]->Test_Reaction(P_Complex[nSpecies1]))
	      return FALSE;
	}
    }
  return TRUE;
}

	
/*************************************************************************
*
* METHOD NAME:	Identify_Complex_Type
*
* DESCRIPTION:	Identifies a complex type on the basis of the composition
*		string of an unknown complex type, and returns a pointer
*		to the complex type object represented by the string.
*
* PARAMETERS:	char* lpszComposition - Composition string representing
*					the complex to be identified.
*
* RETURNS:	C_Complex_Type*	      - Pointer to the complex type the
*					composition string represented.
*
*************************************************************************/

C_Complex_Type*
C_System::Identify_Complex_Type(const char* lpszComposition)
{
  long i;
  i = 0;
  // Loop through all the complex types and compare their compositino strings
  // to the string passed in
  while ((i < m_nNumComplexTypes)
	 && (strcmp(P_Complex_Type[i]->Display_String(), lpszComposition)
	     != 0))
    i++;
  // If a matching complex type was not found...
  if (i >= m_nNumComplexTypes)
    {
      // Output error
      m_pApp->Message(MSG_TYPE_ERROR, 27, lpszComposition);
      // Return NULL pointer indicating the composition string
      // was not recognised
      return NULL;
    }
  // Return pointer to the complex type identified
  return P_Complex_Type[i];
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Clears up the system and frees up memory. All the memory
*		which was dynamically allocated (in the constructor of
*		C_System) is freed. The objects are destructed in the
*		order in which they were created to minimise any possibility
*		of errors.
*
*************************************************************************/

C_System::~C_System(void)
{
  long i;
  // Delete output mangers
  for (i = 0; i < MAX_NUM_OUTPUT_MANAGERS; i ++)
    if (P_Output_Manager[i] != NULL)
      delete P_Output_Manager[i];
  // Delete reaction matrix
  if (P_R_M != NULL)
    delete P_R_M;
  // Delete event manager
  if (P_Event_Manager != NULL)
    delete P_Event_Manager;
  // If complex list exists
  if (P_Complex != NULL)
    {
      // Loop through all the complexes...
      for (i = 0; i < m_nMaxNumComplexes; i++)
	// Delete every complex
	if (P_Complex[i] != NULL)
	  delete P_Complex[i];
      // Delete complex array
      delete[] P_Complex;
    }
  // Loop through every complex type...
  for (i = 0; i < MIN(m_nNumComplexTypes, MAX_COMPLEX_TYPES); i++)
    // Delete every complex type
    if (P_Complex_Type[i] != NULL)
      delete P_Complex_Type[i];
  // Delete the free complex stack
  if (P_Free_Complex != NULL)
    delete P_Free_Complex;
  // Delete random number generator
  if (P_Ran_Gen != NULL)
    delete P_Ran_Gen;
  // If array of complex arrays exists
  if (P_Complex_Array != NULL)
    {
      // Loop through all the complex arrays...
      for (i = 0; i < MAX_NUM_COMPLEX_ARRAYS; i++)
	// Delete every complex array
	if (P_Complex_Array[i] != NULL)
	  delete P_Complex_Array[i];
      // Delete array of complex arrays
      delete[] P_Complex_Array;
      delete[] m_pnArrayEquilInterval;
      delete[] m_pnArrayEquilItCount;
    }
}

/*************************************************************************
*
* METHOD NAME:	Calc_Complex_Type_Levels
*
* DESCRIPTION:	Calculates current levels of each complex type. All the
*		complexes in the system are looped through; if they exist
*		the complex type object they point to is called to identify
*		itself and then the appropriate Complex Type is incremented.
*
*************************************************************************/

void
C_System::Calc_Complex_Type_Levels(void)
{
  long i;
  long nComplexType;
  // Loop through each complex type...
  for (i = 0; i < m_nNumComplexTypes; i++)
    // Set the level of each complex type to zero
    P_Complex_Type[i]->Reset_Level();
  // Loop through each complex (not complex type) in the system...
  for (i = 0; i < m_nMaxNumComplexes; i++)
    // If the complex exists...
    if (P_Complex[i]->Get_Exists())
      {
	// Work out which complex type it is
	// This ensures all fuzzy complex types are equilibrated
	nComplexType = P_Complex[i]->Get_Type()->Get_Reaction_Index();
	// Increment the level of this complex type
	P_Complex_Type[nComplexType]->Inc_Level(P_Complex[i]);
      }
}
