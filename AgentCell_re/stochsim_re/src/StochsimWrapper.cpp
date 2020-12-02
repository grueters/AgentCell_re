/*************************************************************************
*
* FILENAME:	StochsimWrapper.cpp : Wraps StochSim as a library.
*                              
* BASED ON CONTRIBUTION BY: Michael North (north@anl.gov)
*
* DESCRIPTION:	Defines and implements interface
*
* TYPE:		Wrapper
*
*************************************************************************/

#include "StochsimWrapper.hh"


/*
 * Class:     StochsimWrapper
 * Method:    constructor
 */
StochsimWrapper::StochsimWrapper (const char * lpszFileName, int rID)
  : C_Application(lpszFileName)
{
  Bool bSuccess;
  m_nRunID=rID;
  bSuccess = this->Init();
  if (bSuccess) {
    m_pSystem = new C_System(this);
    if(!m_pSystem->Init())
      {
		fprintf(stderr,MSG_ERROR_APPLICATION_INIT);
      }
    else	
      {	
		m_pSystem->m_nNumIterations = 0;
      }
  } else {
      fprintf(stderr,MSG_ERROR_APPLICATION_INIT);
  }
}

/*
 * Class:     SSWrapper (with explicit CWD setting)
 * Method:    constructor
 */
StochsimWrapper::StochsimWrapper (const char * lpszFileName, const char * lpszWorkingDir, int rID)
  : C_Application(lpszFileName, lpszWorkingDir)
{
  Bool bSuccess;
  m_nRunID=rID;
  bSuccess = this->Init();
  if (bSuccess) {
    m_pSystem = new C_System(this);
    if(!m_pSystem->Init())
      {
	fprintf(stderr,MSG_ERROR_APPLICATION_INIT);
      }
    else
      {
	m_pSystem->m_nNumIterations = 0;
      }
  } else {
    fprintf(stderr,MSG_ERROR_APPLICATION_INIT);
  }
}


/*
 * Class:     StochsimWrapper
 * Method:    finalise
 */
void StochsimWrapper::finalise (void)
{
	delete m_pSystem;
}

/*
 * Class:     StochsimWrapper
 * Method:    step
 */
void StochsimWrapper::step (double deltaT)
{
	m_pSystem->Step(deltaT);
}

/*
 * Class:     StochsimWrapper
 * Method:    getTime
 */
double StochsimWrapper::getTime (void)
{
	return (m_pSystem->m_nIterationsDone * m_pSystem->m_flTimeInc);
}

/*
 * Class:     StochsimWrapper
 * Method:    getTimeInc
 */
double StochsimWrapper::getTimeInc (void)
{
	return (m_pSystem->m_flTimeInc);
}

/*
 * Class:     StochsimWrapper
 * Method:    getCopynumber (does not work for multistate complex types)
 *                          
 */
long StochsimWrapper::getCopynumber (const char * cName)
{
	long results = -1;
	int index = -1;
  
	int counter = 0;
	for (counter = 0; counter < MAX_COMPLEX_TYPES; counter++) {
		if ((m_pSystem->P_Complex_Type[counter] != NULL) &&
		 (strcasecmp(m_pSystem->P_Complex_Type[counter]->Display_String(), cName)
		 == 0)) {
			index = counter;
			break;
		}
	}
	if (index >= 0) {
		results = m_pSystem->P_Complex_Type[counter]->m_nLevel;
	}
	return results;
}

/*
 * Class:     StochsimWrapper
 * Method:    getOVValue
 *              ( works for both simple and multistate complex types,
 *                but requires a conjugate output variable with the name
 *                specified by the "cName" argument to be specified in
 *                the INI files )
 *
 */
long StochsimWrapper::getOVValue (const char * cName)
{
        C_Output_Variable* P_OV;
	P_OV = NULL;
	long results = -1;
  
	int countMn, countVar;
	for (countMn = 0; countMn < MAX_NUM_OUTPUT_MANAGERS; countMn++) {
	  if (m_pSystem->P_Output_Manager[countMn] == NULL)
	    break;
	  for (countVar = 0; countVar < MAX_NUM_DISPLAY_VARIABLES; countVar++){
	    P_OV = (m_pSystem->P_Output_Manager[countMn]
		    ->Get_DisplayVarList())[countVar];
	    if (P_OV != NULL && strcasecmp(P_OV->Get_Name(), cName) == 0)
	      break;
	  }
	}
	if (P_OV != NULL) {
	  results = P_OV -> Get_Value();
	}
	return results;
}

/*
 * Class:     StochsimWrapper
 * Method:    setDynamicValue
 */
void StochsimWrapper::setDynamicValue (const char * cName,
				      double newDynamicValue)
{
	// find index of Dynamic value to change by comparing the names
	int dynValueIndex = -1;
        int counter = 0;
	for (counter = 0; counter < m_pSystem->P_Event_Manager->m_nNumEvents; counter++) {
	  if (strcasecmp(m_pSystem->P_Event_Manager->P_Event_List[counter]->m_lpszName,cName) == 0)
	    {
	      dynValueIndex = counter;
	      break;
	    }
	}

	// make the change
	m_pSystem->P_Event_Manager->m_nNextEventTime = 
	  m_pSystem->m_nIterationsDone;
	m_pSystem->P_Event_Manager->P_Event_List[dynValueIndex]->m_nTime = 
	  m_pSystem->m_nIterationsDone;
	m_pSystem->P_Event_Manager->P_Event_List[dynValueIndex]->m_flNewValues[0] = newDynamicValue;
	//set index of next event to be executed to zero. This is 
	//to ensure that the C_Event_Manager::Exec_Event() goes through the 
	//entire list of events (dynamic values): the one that we just modified AND other
	//events (dynamic values) that we might have modified (or will modify) 
	//in another call to StochsimWrapper::setDynamicValue.  
	m_pSystem->P_Event_Manager->m_nNextEvent = 0;
}




