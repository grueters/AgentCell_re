/*************************************************************************
*
* FILENAME:	EventMn.cpp
*
* CLASS:	C_Event_Manager
*
* DESCRIPTION:	This class is organised the events in the system, which
*		are the changes which occur to objects during the
*		simulation. The event manager maintains a sorted list
*		of events and is responsible for executing the events.
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

C_Event_Manager::C_Event_Manager(C_Application* p_App)
{
  m_pApp = p_App;
  long nCount;
  m_nNextEventTime = 0;
  m_nNextEvent = 0;
  m_nNumEvents = 0;
  for (nCount = 0; nCount < MAX_NUM_EVENTS; nCount ++)
    P_Event_List[nCount] = NULL;
}




/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	This method initialises the event manager.The times are
*		scaled if needed (if they are specified in seconds).
*
* PARAMETERS:	int   nTimeUnits - 0 => Time being stored in INIs as iterations
*				   1 => Time is specified in INIs as seconds
*		double flTimeInc - Size of time increment / slice used
*				   during the simulation
*
* RETURNS:	Bool TRUE	 - Initialisation was successful.
*		     FALSE	 - An error occurred during initialisation.
*
*************************************************************************/

Bool
C_Event_Manager::Init(int nTimeUnits, double flTimeInc)
{
  long nCount;
  if (nTimeUnits == 1)
    for (nCount = 0; nCount < m_nNumEvents; nCount ++)
      P_Event_List[nCount]->Scale_Time(flTimeInc);
    
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Exec_Event
*
* DESCRIPTION:	Executes the next event. Any events which also occur at
*		this time are also executed. The events are executed
*		by calling the appropriate dynamic object.
*
*************************************************************************/

void
C_Event_Manager::Exec_Event(void)
{
  // While there are still events to execute,
  // and the next event occurs at this time...
  while ((m_nNextEvent < m_nNumEvents)
	 && (P_Event_List[m_nNextEvent]->Get_nTime() == m_nNextEventTime))
    {
      P_Event_List[m_nNextEvent]->Exec_Event();  // Execute this event
      m_nNextEvent ++;  // Move to the next event in the list
    }
  // If the end of the event list wasn't reached update the next event time
  if (m_nNextEvent < m_nNumEvents)
    m_nNextEventTime = P_Event_List[m_nNextEvent]->Get_nTime();
  else
    m_nNextEventTime = 0;  // Indicate there are no more events to come
}




/*************************************************************************
*
* METHOD NAME:	Add_Event
*
* DESCRIPTION:	Adds an event to the event list at the appropriate place
*		so that it remains sorted by time.
*
* PARAMETERS:	double	          flTime	- Time at which this event
*						  occurs.
*		char*	          lpszName	- Name of this event.
*		C_Dynamic_Object* pDynObject	- Pointer to the dynamic object
*						  being changed as a result.
*		double*		  p_flNewValues	- Pointer to array of new
*                                                 values used to change the
*						  dynamic object.
*		int		  nNumValues	- Number of values that change
*						  at each time point.
*		long*		  p_nParameter	- Optional list of parameters
*						  indicating which part of the
*                                                 object is changing over time.
*		int		  nNumParameters- Optional number of parameters
*                                                 in list.
*
* RETURNS:	Bool TRUE	- The event was added successfully.
*		     FALSE	- An error occurred adding an event.
*
*************************************************************************/

Bool
C_Event_Manager::Add_Event(double flTime, char* lpszName,
			   C_Dynamic_Object* pDynObject, double* p_flNewValues,
			   int nNumValues, long* p_nParameter,
			   int nNumParameters)
{
  long nInsertPosition;
  long nCount;

  // Ensure there is sufficient space in the array to add a new event
  if (m_nNumEvents >= MAX_NUM_EVENTS)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 111);
      return FALSE;
    }
	
  // Go through the sorted list to find the point
  // at which this new event will be inserted
  nInsertPosition = 0;
  while ((nInsertPosition < m_nNumEvents)
	 && (flTime > P_Event_List[nInsertPosition]->Get_flTime()))
    nInsertPosition ++;

  // Move all the events between insert point and end of the list
  // up one position to make room for the new event
  for (nCount = m_nNumEvents; nCount > nInsertPosition; nCount --)
    P_Event_List[nCount] = P_Event_List[nCount - 1];

  // Attempt to create a new event
  P_Event_List[nInsertPosition] = new C_Event(m_pApp);  

  // If there was insufficient memory to create the event, output error
  if (P_Event_List[nInsertPosition] == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8, "new Events");
      return FALSE;
    }
  
  // Initialise the new event
  P_Event_List[nInsertPosition] -> Init(flTime, lpszName, pDynObject,
					p_flNewValues, nNumValues,
					p_nParameter, nNumParameters);

  m_nNumEvents ++;  // Update the number of events stored
  // Set the time of the first event accordingly
  // prior to the start of the simulation
  m_nNextEventTime = P_Event_List[0]->Get_nTime();
  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	This method clears up the memory used by the events
*		stored in the event manager.
*
*************************************************************************/

C_Event_Manager::~C_Event_Manager(void)
{
  long nCount;
  for (nCount = 0; nCount < m_nNumEvents; nCount ++)
    if (P_Event_List[nCount] != NULL)
      delete P_Event_List[nCount];
}




/*************************************************************************
*
* METHOD NAME:	Init_Dynamic_Object
*
* DESCRIPTION:	Initialises a dynamic object by accessing the INI file
*		containing values for these objects at different times.
*
* PARAMETERS:	C_Dynamic_Object* P_Dyn_Object  - Pointer to dynamic object.
*		char*		  lpszDynCode   - Code identifying dynamic
*						  object in INI file.
*		double		  flScaleFactor - Scale factor for object
*                                                 value.
*		int		  nNumValues	- Number of values that change
*						  at each time point.
*		long*		  p_nParameter	- Optional ist of parameters
*                                                 indicating which part of the
*						  object is changing over time.
*		int		  nNumParameters- Optional number of parameters
*                                                 in list
*
* RETURNS:	Bool TRUE	- The object was initialised successfully.
*		     FALSE	- An error occurred during initialisation.
*
*************************************************************************/

Bool
C_Event_Manager::Init_Dynamic_Object(C_Dynamic_Object* P_Dyn_Object,
				     char* lpszDynCode, double flScaleFactor,
				     int nNumValues, long* p_nParameter,
				     int nNumParameters)
{
  char			lpszDynObjectFile[MAX_FILE_NAME_LENGTH];
  char			lpszSetTimes[MAX_INI_VALUE_LENGTH];
  char			lpszTimes[MAX_NUM_EVENTS][MAX_INI_VALUE_LENGTH];
  char			lpszTimeSection[MAX_INI_SECTION_LENGTH];
  char			lpszListValues[MAX_INI_VALUE_LENGTH];
  char*			lpszPos;
  long			nNumSets;
  long			nNumTimes;
  double		flTime;
  double		flDynValue[MAX_NUM_DYNAMIC_VALUES];
  double		flRawDynValue;
  int			nCurrValue;
  int			i;

  // Get name of file containing dynamic object information
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_DYNAMIC_FILE,
		     lpszDynObjectFile))
    return FALSE;

  // Get number of sets of times from file
  if (!m_pApp->Get_INI(INI_DYN_GENERAL_SECTION, INI_DYN_NUM_SETS, &nNumSets,
		     lpszDynObjectFile))
    return FALSE;

  nNumTimes = 0;
  // For each set of times in the INI file, strip out each time
  for (i = 0; i < nNumSets; i++)
    {
      // Construct name of set (line name in INI file)
      sprintf(lpszSetTimes, "%s%d", INI_DYN_SET_OF_TIMES, i + 1);
      // Read set of times
      if (!m_pApp->Get_INI(INI_DYN_GENERAL_SECTION, lpszSetTimes, lpszSetTimes,
			 lpszDynObjectFile))
	return FALSE;

      // Strip out times from set

      // Find first time
      lpszPos = strtok(lpszSetTimes, INI_DYN_TIME_SEPARATOR);
      // While the end of the set is not reached,
      // and there is space in the array...
      while ((lpszPos != NULL) && (nNumTimes < MAX_NUM_EVENTS))
	{
	  strcpy(lpszTimes[nNumTimes], lpszPos);  // Copy the time string
	  nNumTimes ++;
	  lpszPos = strtok(NULL, INI_DYN_TIME_SEPARATOR);  // Find next time
	}
      if (lpszPos != NULL)  // If the end of the list wasn't reached
	{
	  m_pApp->Message(MSG_TYPE_ERROR, 116);  // Output error
	  return FALSE;
	}
    }

  // For each time specified, read in the values and create the new events

  for (i = 0; i < nNumTimes; i ++)
    {
      // Construct section name for this time point
      sprintf(lpszTimeSection, "%s%s", INI_DYN_TIME_SECTION, lpszTimes[i]);
      
      // Read value of this parameter at this time point
      if (!m_pApp->Get_INI(lpszTimeSection, lpszDynCode, lpszListValues,
			 lpszDynObjectFile))
	return FALSE;
      
      // If a value has been specified...
      // (it might not have been, indicating no change to value)
      if (strlen(lpszListValues) > 0)
	{
	  // Find first value
	  lpszPos = strtok(lpszListValues, INI_DYN_VALUE_SEPARATOR);
	  flTime = atof(lpszTimes[i]);  // Convert the string time to a double
	  nCurrValue = 0;
	  // While the end of the list of values is not reached...
	  while (lpszPos != NULL)
	    {
	      // Convert the string value to a double
	      flRawDynValue = atof(lpszPos);
	      // If too many dynamic values have been specified, output error
	      if (nCurrValue >= MAX_NUM_DYNAMIC_VALUES)
		{
		  m_pApp->Message(MSG_TYPE_ERROR, 117, lpszTimes[i]);  
		  return FALSE;
		}
	      // Scale the dynamic value
	      flDynValue[nCurrValue] = flScaleFactor * flRawDynValue;
	      nCurrValue ++;  // Keep track of the number of values read so far
	      // Find next value
	      lpszPos = strtok(NULL, INI_DYN_VALUE_SEPARATOR);
	    }
	  // Validate the dynamic value
	  if (!P_Dyn_Object->Validate_Dynamic_Values(nCurrValue, flDynValue))
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 114, lpszTimes[i]);
	      return FALSE;
	    }
	  // Ensure the correct number of values were read in
	  if (nCurrValue != nNumValues)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 117, lpszTimes[i]);
	      return FALSE;
	    }

	  // Attempt to add the event to the list of events
	  if (!Add_Event(flTime, lpszDynCode, P_Dyn_Object, flDynValue,
			 nNumValues, p_nParameter, nNumParameters))
	    return FALSE;
	}
    }

  return TRUE;  // Return without error
}                
