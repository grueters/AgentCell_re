/*************************************************************************
*
* FILENAME:	Event.cpp
*
* CLASS:	C_Event
*
* DESCRIPTION:	This class is used to store a single event, the modification
*		of the values of an object during the simulation eg changing
*		the rate constant. The object being changed is referred
*		to as a dynamic object (C_Dynamic_Object).
*
* TYPE:		Core
*
*************************************************************************/


#include "_Stchstc.hh"




/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	This method initialises the event. All the
*		attributes are cleared.
*
* PARAMETERS:   C_Application*  p_App  - Pointer to parent application
*
*
*************************************************************************/

C_Event::C_Event(C_Application* p_App)
{
  m_pApp = p_App;
  m_lpszName = NULL;
  m_pParameter = NULL;
}




/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	This method clears up the memory used by the event.
*
*************************************************************************/

C_Event::~C_Event(void)
{
  if (m_lpszName != NULL)
    delete[] m_lpszName;
  if (m_pParameter != NULL)
    delete[] m_pParameter;
}




/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the event.
*
* PARAMETERS:	unsigned long long nTime	- Time at which this event will
*						  occur.
*		char*		  lpszName	- Name of this event.
*		C_Dynamic_Object* pDynObject	- Pointer to the dynamic object
*						  itself.
*		double*		  p_flNewValues	- Pointer to new values.
*		int		  nNumValues	- Number of values that change
*						  at each time point.
*		long*		  p_nParameter	- Parameter list used to
*                                                 identify
*						  how object is changing.
*		int		  nNumParameters- Number of parameters in list.
*
*************************************************************************/

Bool
C_Event::Init(double flTime, char* lpszName, C_Dynamic_Object* pDynObject,
	      double* p_flNewValues, int nNumValues, long* p_nParameter,
	      int nNumParameters)
{
  int nCount;

  for (nCount = 0; nCount < nNumValues; nCount ++)
    m_flNewValues[nCount] = p_flNewValues[nCount];

  // Attempt to create a new string to hold object name
  m_lpszName = new char[MAX_INI_PARAMETER_LENGTH];
  // If there was insufficient memory to create the string, output error
  if (m_lpszName == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8, "new Event Names");
      return FALSE;
    }
  strcpy(m_lpszName, lpszName); // Copy the name

  // Attempt to create a new array to hold parameters
  m_pParameter = new long[nNumParameters];
  // If there was insufficient memory to create the string, output error
  if (m_pParameter == NULL)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 8, "new Event Parameters");
      return FALSE;
    }
  
  // Copy all the parameters
  for (nCount = 0; nCount < nNumParameters; nCount ++)
    m_pParameter[nCount] = p_nParameter[nCount];

  m_flTime = flTime;
  m_nTime = (unsigned long long) flTime;
  m_pDynObject = pDynObject;

  return TRUE;
}




/*************************************************************************
*
* METHOD NAME:	Exec_Event
*
* DESCRIPTION:	Executes this event by calling the dynamic object it refers
*		to. Then displays message indicating completion of event.
*
*************************************************************************/

void
C_Event::Exec_Event(void)
{
  char lpszParaList[MAX_MESSAGE_LENGTH];
  char lpszTime[MAX_MESSAGE_LENGTH];
  strstream writeStream;

  // Execute event
  m_pDynObject->Exec_Event(m_flNewValues, m_pParameter);
  // Convert event time to string
  writeStream << m_nTime;
  writeStream.getline(lpszTime, sizeof(lpszTime));
  // Copy name to parameter list
  strcpy(lpszParaList, m_lpszName);
  // Add parameter list separator
  strcat(lpszParaList, PARAMETER_LIST_SEPARATOR);
  // Add name of event to parameter list
  strcat(lpszParaList, lpszTime);
  // Output simulation length, display frequency,
  // and how long the simulation will take to run
  m_pApp->Message(MSG_TYPE_STATUS, 121, lpszParaList);
}
                             
