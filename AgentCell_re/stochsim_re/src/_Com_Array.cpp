/*************************************************************************
*
* FILENAME:	Com_Array.cpp
*
* CLASS:	C_Complex_Array
*
* DESCRIPTION:	This class represents a two-dimensional array of complexes.
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
* DESCRIPTION:	Initialises variables and allocates memory.
*
* PARAMETERS:	C_Application*  p_App          - Pointer to parent application
*
*               char*           lpszName              - Name of this complex
*                                                       array
*               C_System*       P_System              - Pointer to parent
*                                                       system
*               long            nSystemID             - Numerical identifier
*                                                       for this complex array
*               C_Complex_Type_Multistate* P_CT       - Pointer to the complex
*                                                       type contained by this
*                                                       complex array
*               C_Integer_Pair* pDimensions           - Dimensions of this
*                                                       complex arary
*               Bool            bIsNeighbourSensitive - Whether the complex
*                                                       type contained in this
*                                                       array is neighbour-
*                                                       sensitive or not.
*
*************************************************************************/

C_Complex_Array::C_Complex_Array(C_Application* p_App, char* lpszName,
				 C_System* P_System,
				 long nSystemID,
				 C_Complex_Type_Multistate* P_CT,
				 C_Integer_Pair* pDimensions,
				 Bool bIsNeighbourSensitive,
				 long nBoundaryCondition,
				 const char* lpszGeometry)
{
  int i, j;

  // Copy parameters to member variables
  m_pApp = p_App;
  m_nSystemID = nSystemID;
  P_Parent = P_System;
  P_Type = P_CT;
  m_bIsNeighbourSensitive = bIsNeighbourSensitive;
  m_nBoundaryCondition = nBoundaryCondition;
  strcpy(m_lpszName, lpszName);
  strcpy(m_lpszGeometry, lpszGeometry);

  // Allocate memory to store dimensions
  m_pDimensions = new C_Integer_Pair(pDimensions->nX, pDimensions->nY);

  // Allocate memeory for the 2-D array of C_Complex_Multistate pointers
  P_P_Complexes = new C_Complex_Multistate** [m_pDimensions->nX];
  for(i=0; i < m_pDimensions->nX; i++)
    P_P_Complexes[i] = new C_Complex_Multistate* [m_pDimensions->nY];

  // Initialise all elements of 2-D array of C_Complex_Multistate pointers to NULL
  for(i=0; i < m_pDimensions->nX; i++)
    for(j=0; j < m_pDimensions->nY; j++)
      P_P_Complexes[i][j] = NULL;
}

/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Free memory allocated during construction or initialisation.
*
* PARAMETERS:	NONE
*
*************************************************************************/

C_Complex_Array::~C_Complex_Array(void)
{

  // Don't know why this doesn't work, but it doesn't....
  // for(int i = 0; i < m_pDimensions->nX ; i++)

  // So I use this...
  for(int i = m_pDimensions->nX - 1; i >= 0; i--)
    if (P_P_Complexes[i] != NULL)
      delete[] P_P_Complexes[i];
  if (P_P_Complexes != NULL)
    delete[] P_P_Complexes;

  delete m_pDimensions;
}

/*************************************************************************
*
* METHOD NAME:	Get_Address
*
* DESCRIPTION:	Returns the address (coordinates within the array) of a
*		complex specified by its pointer.
*
* PARAMETERS:	C_Integer_Pair*	pAddress - Pointer to the memory location
*					   to which address is to be
*					   given.
*		C_Complex_Multistate*	pComplex 
*                                        - Pointer to the complex for
*					   which the address is to be
*					   retrieved.
*
*************************************************************************/

Bool
C_Complex_Array::Get_Address(C_Integer_Pair* pAddress,
			     C_Complex_Multistate* pComplex)
{
  for(int i=0; i < m_pDimensions->nX; i++)
    for(int j=0; j < m_pDimensions->nY; j++)
      if(P_P_Complexes[i][j] == pComplex)
	{
	  pAddress->Set((long)i, (long)j);
	  return TRUE;
	}
  return FALSE;
}


/*************************************************************************
*
* METHOD NAME:	Insert_Complex
*
* DESCRIPTION:	Inserts the specified complex at the specified address
*		within the array.
*
* PARAMETERS:	C_Integer_Pair*	pAddress - Pointer to the memory location
*					   in which address information
*					   is stored.
*		C_Complex*	pComplex - Pointer to the complex which
*					   is being inserted into the
*					   array.
*
*************************************************************************/

Bool
C_Complex_Array::Insert_Complex(C_Integer_Pair* pAddress,
				C_Complex_Multistate* pComplex)
{
  if(pAddress->nX > m_pDimensions->nX || pAddress->nY > m_pDimensions->nY)
    return FALSE;
  else
    {
      P_P_Complexes[pAddress->nX][pAddress->nY] = pComplex;
      if (m_bIsNeighbourSensitive)
	{
	  ((C_Complex_Neighbour_Sensitive*) pComplex) -> Set_IsArrayMember();
	}
    }
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Get_Complex
*
* DESCRIPTION:	Returns a pointer to a complex within the array specified
*		by its address (coordinates within the array).
*
* PARAMETERS:	C_Integer_Pair* pAddress - Pointer to pair of integers
*					   corresponding to the X and
*					   Y coordinates of the address
*
*************************************************************************/

C_Complex_Multistate*
C_Complex_Array::Get_Complex(C_Integer_Pair* pAddress)
{
  return P_P_Complexes[pAddress->nX][pAddress->nY];
}


/*************************************************************************
*
* METHOD NAME:	Get_Dimensions
*
* DESCRIPTION:	Gives the dimensions of this array object to a specified
*		integer-pair pointer.
*
* PARAMETERS:	C_Integer_Pair* pDimensions - Pointer to memory space to
*					      which dimension information
*					      is to be given.
*
*************************************************************************/

void
C_Complex_Array::Get_Dimensions(C_Integer_Pair* pDimensions)
{
  pDimensions->Set(m_pDimensions->nX, m_pDimensions->nY);
}


/*************************************************************************
*
* METHOD NAME:	Equilibrate
*
* DESCRIPTION:	Equilibrates all rapid equilibria in member complexes.
*
* PARAMETERS:	None
*
*************************************************************************/

void
C_Complex_Array::Equilibrate(void)
{
  long i;
  long nRand;
  C_Complex_Neighbour_Sensitive* P_NSComplex;
  C_Complex_Type_Neighbour_Sensitive* P_NSCT;

  if (m_bIsNeighbourSensitive && this->Has_RapidEqm())
    {
      for (i = 0; i < m_pDimensions->nX * m_pDimensions->nY; i++)
	{
	  nRand = (P_Parent->P_Ran_Gen
		   -> New_Num(m_pDimensions->nX * m_pDimensions->nY));
	  P_NSComplex = (C_Complex_Neighbour_Sensitive *)
	    Get_Complex(nRand % m_pDimensions->nX, nRand / m_pDimensions->nX);
#ifdef _DYNAMIC_UPDATE
  // Decrement the level of this state.
	  P_NSCT = ((C_Complex_Type_Neighbour_Sensitive *)
		    P_NSComplex -> Get_Type());
	  P_NSCT -> Dec_Level(P_NSComplex);
#endif /* _DYNAMIC_UPDATE */
	  P_NSComplex -> Equilibrate();
#ifdef _DYNAMIC_UPDATE
	  P_NSCT -> Inc_Level(P_NSComplex);
#endif /* _DYNAMIC_UPDATE */
	}
    }
}	

/*************************************************************************
*
* METHOD NAME:	Get_Variables_From_File
*
* DESCRIPTION:	Creates any additional variables required by this complex
*		array from the INI file.
*
* PARAMETERS:	C_Output_Variable** P_Variable
*                                         - Pointer to array of
*					    variables to be populated
*         	long*	p_nNumVar         - Pointer to number of variables 
*					    which are to be created	
*		long	nMaxNumVar        - Maximum number of variables
*					    which can be created
*
* RETURNS:	Bool	TRUE		  - Variables were created successfully
*			FALSE		  - An error occurred while creating
*					    variables
*			&P_Variable	  - Populated array of pointers
*			&p_nNumVar	  - Number of variables created
*
*************************************************************************/

Bool
C_Complex_Array::Get_Variables_From_File(C_Output_Variable** P_Variable,
					 long* p_nNumVar, long nMaxNumVar)
{
  char                        lpszINIFile[MAX_FILE_NAME_LENGTH];
  char                        lpszSection[MAX_INI_SECTION_LENGTH];
  char                        lpszSnapsVarName[MAX_INI_VALUE_LENGTH];
  char                        lpszSnapsVarStates[MAX_INI_VALUE_LENGTH];
  char                        lpszArrayName[MAX_INI_VALUE_LENGTH];
  long                        nNumVar;
  long			      nNumWildcards;
  long                        p_nWildcardValue[MAX_NUM_ARRAY_SNAPS_STATES];
  long                        p_nWildcardMask[MAX_NUM_ARRAY_SNAPS_STATES];
  char*                       lpszPos;
  long                        nCountVar;

  // Get array INI file name from the system INI file
  if (!m_pApp->Get_INI(INI_FILE_NAME_SECTION, INI_ARRAY_INI_FILE,
		     lpszINIFile))
    return FALSE;
  // Get the number of variables from the file
  if (!m_pApp->Get_INI(INI_ARRAY_GENERAL_SECTION, INI_ARRAY_NUM_SNAPS_VAR,
		     &nNumVar, lpszINIFile))
      return FALSE;

  // For each variable, read in the information from the INI file
  for (nCountVar = 0; nCountVar < nNumVar; nCountVar ++)
    {
      // Construct section title
      sprintf(lpszSection, "%s%ld", INI_ARRAY_SNAPS_VAR_SECTION, nCountVar+1);
      // Read name of snapshot variable
      if (!m_pApp->Get_INI(lpszSection, INI_ARRAY_SNAPS_VAR_NAME,
			 lpszSnapsVarName, lpszINIFile))
	return FALSE;

      // Check if this variable represents this complex array
      if (!m_pApp->Get_INI(lpszSection, INI_COMPLEX_ARRAY,
			 lpszArrayName, lpszINIFile))
	return FALSE;
      if (strcmp(lpszArrayName, m_lpszName) == 0)
	{
	  // Read states this snapshot variable represents
	  if (!m_pApp->Get_INI(lpszSection, INI_ARRAY_SNAPS_VAR_STATES,
			     lpszSnapsVarStates, lpszINIFile))
	    return FALSE;

	  // Initialise string stream for display string with array name
	  ostrstream DispStrStream;
	  DispStrStream << m_lpszName << ":";

	  // Identify the states this variable represents from the list.
	  // The states in this list can be wildcard strings.

	  nNumWildcards = 0;
	  // Find first state
	  lpszPos = strtok(lpszSnapsVarStates, INI_VAR_STATES_SEPARATOR);
	  // While the end of the list is not reached...
	  while (lpszPos != NULL)
	    {
	      // Ensure the length of this string matches the number of flags
	      if ((signed) strlen(lpszPos) != P_Type->Get_Num_Flags())
		{
		  m_pApp->Message(MSG_TYPE_ERROR, 107, lpszPos);
		  return FALSE;
		}
	  
	      // Append this wildcard string to display string
	      DispStrStream << lpszPos;
      
	      // Work out the mask and value from the wildcard string
	      P_Type -> Extract_Wildcard_Mask(lpszPos,
					      &p_nWildcardMask[nNumWildcards],
					      &p_nWildcardValue[nNumWildcards]);
	  
	      nNumWildcards ++;
	      // Find next bit string in list
	      lpszPos = strtok(NULL, INI_VAR_STATES_SEPARATOR);
	      // Append comma to display if there are more wildcard strings
	      if (lpszPos != NULL) DispStrStream << ",";
	    }
	  DispStrStream << ends;
	  // If the end of the list wasn't reached, output error
	  if (lpszPos != NULL)
	    {
	      m_pApp->Message(MSG_TYPE_ERROR, 100, lpszSnapsVarName);
	      return FALSE;
	    }

	  // Create the output variable 
	  P_Variable[nCountVar]
	    = new C_Output_Variable_Array_Snaps(m_pApp, lpszSnapsVarName,
						DispStrStream.str(), this,
						p_nWildcardMask,
						p_nWildcardValue,
						nNumWildcards);
	  // Update number of variables created
	  (*p_nNumVar) ++;
	}
    }
  return TRUE;
}

/*************************************************************************
*
* METHOD NAME:	Get_Neighbour
*
* DESCRIPTION:	Gives the dimensions of this array object to a specified
*		integer-pair pointer.
*
* PARAMETERS:	C_Integer_Pair* pDimensions - Pointer to memory space to
*					      which dimension information
*					      is to be given.
*
*************************************************************************/

C_Complex_Multistate*
C_Complex_Array::Get_Neighbour(C_Integer_Pair* pAddress, int nNeighbour)
{
  C_Integer_Pair         NeighbourAddress;
  enum Direction         {nNorth, nEast, nSouth, nWest/*, nNE, nSE, nSW, nNW*/};
  enum BoundaryCondition {nToroidal, nNormal};
  Bool                   bNoNeighbour = FALSE;

  switch(nNeighbour)
    {
    case nNorth:
      // Unless we're at the northern edge of the complex array
      if (pAddress->nY != 0)
	// Set neighbour's address to pAddress->nY - 1 (one above)
	NeighbourAddress.Set(pAddress->nX, pAddress->nY - 1);
      else  // Otherwise (if we ARE at the northern edge), 
	// Handle boundary conditions
	switch (m_nBoundaryCondition)
	  {
	    // If boundaries are toroidal
	  case nToroidal:
	    // Set neighbour to complex at other end
	    NeighbourAddress.Set(pAddress->nX, m_pDimensions->nY - 1);
	    break;
	    // If boundaries are normal
	  case nNormal:
	    // Set flag indicating no neighbour
	    bNoNeighbour = TRUE;
	    break;
	  }
      break;
    case nEast:
      // Unless we're at the eastern edge of the complex array
      if (pAddress->nX != m_pDimensions->nX - 1)
	// Set neighbour's address to pAddress->nX + 1 (one to the right)
	NeighbourAddress.Set(pAddress->nX + 1, pAddress->nY);
      else  // Otherwise (if we ARE at the eastern edge), 
	// Handle boundary conditions
	switch (m_nBoundaryCondition)
	  {
	    // If boundaries are toroidal
	  case nToroidal:
	    // Set neighbour to complex at other end
	    NeighbourAddress.Set(0, pAddress->nY);
	    break;
	    // If boundaries are normal
	  case nNormal:
	    // Set flag indicating no neighbour
	    bNoNeighbour = TRUE;
	    break;
	  }
      break;
    case nSouth:
      // Unless we're at the southern edge of the complex array
      if (pAddress->nY != m_pDimensions->nY - 1)
	// Set neighbour's address to j + 1 (one below)
	NeighbourAddress.Set(pAddress->nX,  pAddress->nY + 1);
      else  // Otherwise (if we ARE at the southern edge), 
	//  Handle boundary conditions
	switch (m_nBoundaryCondition)
	  {
	    // If boundaries are toroidal
	  case nToroidal:
	    // Set neighbour to complex at other end
	    NeighbourAddress.Set(pAddress->nX, 0);
	    break;
	    // If boundaries are normal
	  case nNormal:
	    // Set to false value to indicate no neighbour
	    bNoNeighbour = TRUE;
	    break;
	  }
      break;
    case nWest:
      // Unless we're at the wetern edge of the complex array
      if (pAddress->nX != 0)
	// Set neighbour's address to pAddress->nX - 1 (one to the left)
	NeighbourAddress.Set(pAddress->nX - 1, pAddress->nY);
      else  // Otherwise (if we ARE at the western edge), 
	// Handle boundary conditions
	switch (m_nBoundaryCondition)
	  {
	    // If boundaries are toroidal
	  case nToroidal:
	    // Set neighbour to complex at other end
	    NeighbourAddress.Set(m_pDimensions->nX - 1, pAddress->nY);
	    break;
	    // If boundaries are normal
	  case nNormal:
	    // Set flag indicating no neighbour
	    bNoNeighbour = TRUE;
	    break;
	  }
      break;
      /*
	case nNE:
      // If we're at the northern edge of the complex array
      if (pAddress->nY == 0)
	{
	  // Handle boundary conditions
	  switch (m_nBoundaryCondition)
	    {
	      // If boundaries are toroidal
	    case nToroidal:
	      // Set Y coordinate to other end
	      nY = m_pDimensions->nY - 1;
	      // If boundaries are normal
	    case nNormal:
	      // Set flag indicating no neighbour
	      bNoNeighbour = TRUE;
	    }
	}
      else
	// Otherwise, set Y coordinate to one above
	nY = pAddress->nY - 1;
	
      // If we're at the eastern edge of the complex array
      if (pAddress->nX == m_pDimensions->nX - 1)
	{
	  // Set neighbour to complex at other end
	  // Handle boundary conditions
	  switch (m_nBoundaryCondition)
	    {
	      // If boundaries are toroidal
	    case nToroidal:
	      // Set X coordinate to other end
	      nX = 0;
	      // If boundaries are normal
	    case nNormal:
	      // Set flag indicating no neighbour
	      bNoNeighbour = TRUE;
	    }
	}
      else
	// Set neighbour to complex at other end
	NeighbourAddress.Set(pAddress->nX+1, pAddress->nY-1);
      break;
    case nSE:
      // If we're at the southern edge of the complex array
      if (pAddress->nY == m_pDimensions->nY - 1)
	{
	  switch (m_nBoundaryCondition)
	    {
	      // If boundaries are toroidal
	    case nToroidal:
	      // Unless we're also at the eastern edge
	      if (pAddress->nX != m_pDimensions->nX - 1)
		// Set neighbour to complex at other end
		NeighbourAddress.Set(pAddress->nX + 1, 0);
	      // If we are at the eastern edge, too, set X to zero
	      else
		NeighbourAddress.Set(0, 0);
	      // If boundaries are normal
	    case nNormal:
	      // Set flag indicating no neighbour
	      bNoNeighbour = TRUE;
	    }
	}
      else
	// Set neighbour to complex at other end
	NeighbourAddress.Set(pAddress->nX+1, pAddress->nY+1);
      break;
    case nSW:
      // If we're at the southern edge of the complex array
      if (pAddress->nY == m_pDimensions->nY - 1)
	{
	  switch (m_nBoundaryCondition)
	    {
	      // If boundaries are toroidal
	    case nToroidal:
	      // Unless we're also at the western edge
	      if (pAddress->nX != 0)
		// Set neighbour to complex at other end
		NeighbourAddress.Set(pAddress->nX - 1, 0);
	      // If we are at the eastern edge, too, set X to other end
	      else
		NeighbourAddress.Set(m_pDimensions->nX + 1, 0);
	      // If boundaries are normal
	    case nNormal:
	      // Set flag indicating no neighbour
	      bNoNeighbour = TRUE;
	    }
	}
      else
	// Set neighbour to complex at other end
	NeighbourAddress.Set(pAddress->nX-1, pAddress->nY+1);
      break;
    case nNW:
      // If we're at the northern edge of the complex array
      if (pAddress->nY == 0)
	{
	  switch (m_nBoundaryCondition)
	    {
	      // If boundaries are toroidal
	    case nToroidal:
	      // Unless we're also at the western edge
	      if (pAddress->nX != 0)
		// Set neighbour to complex at other end
		NeighbourAddress.Set(pAddress->nX - 1, 0);
	      // If we are at the eastern edge, too, set X to other end
	      else
		NeighbourAddress.Set(m_pDimensions->nX + 1, 0);
	      // If boundaries are normal
	    case nNormal:
	      // Set flag indicating no neighbour
	      bNoNeighbour = TRUE;
	    }
	}
      else
	// Set neighbour to complex at other end
	NeighbourAddress.Set(pAddress->nX-1, pAddress->nY-1);
      break;
      */
    }
  // If there is no neighbour in the specified direction
  if (bNoNeighbour)
    // Return NULL pointer indicating that there is no neighbour in this dir
    return NULL;
  else  //Otherwise (if there is a neighbour),
    // Get a pointer to the neighbouring complex
    return Get_Complex(&NeighbourAddress);
}


/*************************************************************************
*
* METHOD NAME:	Get_State_From_Dump
*
* DESCRIPTION:	Retrieves the state of the complex in the specified coorinate
*               from the specified dump file at the specified time.
*
* PARAMETERS:	char*           lpszDumpFile - The name of the dump file.
*
*               double          flTime       - Time at which state is to be
*                                              retrieved from.
*               
*               long**          P_P_nState   - Pointer to memory space to
*					       which state values are to be
*					       stored.
* RETURNS:      TRUE                         - The state of the complex array
*                                              was retrieved successfully.
*               FALSE                        - On error.
*
*************************************************************************/

Bool
C_Complex_Array::Get_State_From_Dump(char* lpszDumpFile, double flTime,
				     long** P_P_nState)
{
  ifstream       Dump_Stream;
  strstream      Line_Stream;
  char           lpszLine[MAX_INI_LINE_LENGTH];
  char           lpszParaList[MAX_INI_VALUE_LENGTH];
  ostringstream  ParaListStream;
  char*          lpszPos;
  C_Integer_Pair Dimensions;
  double         flFrameTime;
  int            i;
  int            j;
  long           nCurrentLine;
  long           nStartLine;

  // Open dump file
  Dump_Stream.open(lpszDumpFile, ios::in);
  // Check for error in opening file
#if ( defined(_ALPHA) || defined(_AIX) )
  if (!Dump_Stream.rdbuf()->is_open())
#else
  if (!Dump_Stream.is_open())
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
    {
      m_pApp->Message(MSG_TYPE_ERROR, 29, lpszDumpFile);
      return FALSE;
    }
  
  nCurrentLine =0;
  // Skip the first line
  Dump_Stream.getline(lpszLine, sizeof(lpszLine));
  nCurrentLine ++;
  // Read the second line, which contains the geometry
  Dump_Stream.getline(lpszLine, sizeof(lpszLine));
  nCurrentLine ++;

  // Scan the Geometry line and check for errors
  if (strncmp(lpszLine, INI_ARRAY_GEOMETRY, strlen(INI_ARRAY_GEOMETRY)) != 0)
    {
      // Output error
      sprintf(lpszParaList, "%s%s%s", lpszDumpFile,
	      PARAMETER_LIST_SEPARATOR, m_lpszName);
      m_pApp->Message(MSG_TYPE_ERROR, 181, lpszParaList);
      return FALSE;
    }

  lpszPos = strtok(lpszLine, " =");
  if (lpszPos == NULL)
    {
      // Output error
      sprintf(lpszParaList, "%s%s%s", lpszDumpFile,
	      PARAMETER_LIST_SEPARATOR, m_lpszName);
      m_pApp->Message(MSG_TYPE_ERROR, 181, lpszParaList);
      return FALSE;
    }

  lpszPos = strtok(NULL, " =");
  if (lpszPos == NULL)
    {
      // Output error
      sprintf(lpszParaList, "%s%s%s", lpszDumpFile,
	      PARAMETER_LIST_SEPARATOR, m_lpszName);
      m_pApp->Message(MSG_TYPE_ERROR, 181, lpszParaList);
      return FALSE;
    }

  if (strcmp(lpszPos, m_lpszGeometry) != 0)
    {
      sprintf(lpszParaList, "%s%s%s", m_lpszName,
	      PARAMETER_LIST_SEPARATOR, lpszDumpFile);
      m_pApp->Message(MSG_TYPE_ERROR, 179, lpszParaList);
      return FALSE;
    }

  // Read the third line, which contains the dimensions
  Dump_Stream.getline(lpszLine, sizeof(lpszLine));
  nCurrentLine ++;
  lpszPos = strtok(lpszLine, " x");
  // If this line is empty...
  if (lpszPos == NULL)
    {
      // Output error
      sprintf(lpszParaList, "%s%s%s", lpszDumpFile,
	      PARAMETER_LIST_SEPARATOR, m_lpszName);
      m_pApp->Message(MSG_TYPE_ERROR, 181, lpszParaList);
      return FALSE;
    }
  Dimensions.nX = (long) atoi(lpszPos);

  lpszPos = strtok(NULL, " x");
  if (lpszPos == NULL)
    {
      // Output error
      sprintf(lpszParaList, "%s%s%s", lpszDumpFile,
	      PARAMETER_LIST_SEPARATOR, m_lpszName);
      m_pApp->Message(MSG_TYPE_ERROR, 181, lpszParaList);
      return FALSE;
    }
  Dimensions.nY = (long) atoi(lpszPos);

  if (Dimensions.nX != m_pDimensions->nX || Dimensions.nY != m_pDimensions->nY)
    {
      sprintf(lpszParaList, "%s%s%s", m_lpszName,
	      PARAMETER_LIST_SEPARATOR, lpszDumpFile);
      m_pApp->Message(MSG_TYPE_ERROR, 180, lpszParaList);
      return FALSE;
    }
  
//    // Read the next line
//    Dump_Stream.getline(lpszLine, sizeof(lpszLine));
//    // If this line is the beginning of a frame
//    if (strncmp(lpszLine, "TIME:", strlen("TIME:")) == 0)
//      {
//        // Get the time, which is the second word
//        lpszPos = strtok(lpszLine, " ");
//        lpszPos = strtok(NULL, " ");
//        flFrameTime = atof(lpszPos);
//      }
//    else
//      {
//        flFrameTime = 0;
//      }

  // If the specified time is zero, find the last frame
  if (flTime == 0)
    {
      nStartLine = 0;
      // While the end of the file has not been reached,
      while (!Dump_Stream.eof())
	{
	  // Read the next line
	  Dump_Stream.getline(lpszLine, sizeof(lpszLine));
	  nCurrentLine ++;
	  // Get the time, which is the second word
	  lpszPos = strtok(lpszLine, " ");
	  // If the line is empty...
	  if (lpszPos == NULL)
	    {
	      // Skip this line
	      continue;
	    }
	  // If this line is the beginning of a frame
	  else if (strcmp(lpszPos, "TIME:") == 0)
	    {
	      lpszPos = strtok(NULL, " ");
	      if (lpszPos == NULL)
		{
		  // Output error
		  sprintf(lpszParaList, "%s%s%s", lpszDumpFile,
			  PARAMETER_LIST_SEPARATOR, m_lpszName);
		  m_pApp->Message(MSG_TYPE_ERROR, 181, lpszParaList);
		  return FALSE;
		}
	      // Record the number of the current line
	      flTime = atof(lpszPos);
	      nStartLine = nCurrentLine;
	    }
	}
      // If no frames were found...
      if (nStartLine == 0)
	{
	  // Output error
	  sprintf(lpszParaList, "%s%s%s", lpszDumpFile,
		  PARAMETER_LIST_SEPARATOR, m_lpszName);
	  m_pApp->Message(MSG_TYPE_ERROR, 181, lpszParaList);
	  return FALSE;
	}
      // Close the dump stream once
      Dump_Stream.close();

      // Output status message...
      ParaListStream << m_lpszName << PARAMETER_LIST_SEPARATOR;
      ParaListStream << lpszDumpFile << PARAMETER_LIST_SEPARATOR;
      ParaListStream << flTime << std::ends;
      m_pApp->Message(MSG_TYPE_STATUS, 174, ParaListStream.str().c_str());

      // Reopen dump stream
      Dump_Stream.open(lpszDumpFile, ios::in);
      // Check for error in opening file
#if ( defined(_ALPHA) || defined(_AIX) )
      if (!Dump_Stream.rdbuf()->is_open())
#else
	if (!Dump_Stream.is_open())
#endif /*  ( defined(_ALPHA) || defined(_AIX) ) */
	  {
	    m_pApp->Message(MSG_TYPE_ERROR, 29, lpszDumpFile);
	    return FALSE;
	  }
      nCurrentLine = 0;
      while (nCurrentLine < nStartLine)
	{
	  Dump_Stream.getline(lpszLine, sizeof(lpszLine));
	  nCurrentLine++;
	}
    }
  else
    {
      // Output status message...
      // Output status message...
      ParaListStream.str("");
      ParaListStream << m_lpszName << PARAMETER_LIST_SEPARATOR;
      ParaListStream << lpszDumpFile << PARAMETER_LIST_SEPARATOR;
      ParaListStream << flTime << std::ends;
      m_pApp->Message(MSG_TYPE_STATUS, 177, ParaListStream.str().c_str());

      flFrameTime = -1;
      // While the end of the file has not been reached,
      // look for the time frame to use
      while (!Dump_Stream.eof() && flFrameTime != flTime)
	{
	  // Read the next line
	  Dump_Stream.getline(lpszLine, sizeof(lpszLine));
	  // Get the time, which is the second word
	  lpszPos = strtok(lpszLine, " ");
	  // If the line is empty...
	  if (lpszPos == NULL)
	    {
	      // Skip this line
	      continue;
	    }
	  // If this line is the beginning of a frame
	  else if (strcmp(lpszPos, "TIME:") == 0)
	    {
	      lpszPos = strtok(NULL, " ");
	      if (lpszPos == NULL)
		{
		  // Output error
		  sprintf(lpszParaList, "%s%s%s", lpszDumpFile,
			  PARAMETER_LIST_SEPARATOR, m_lpszName);
		  m_pApp->Message(MSG_TYPE_ERROR, 181, lpszParaList);
		  return FALSE;
		}
	      flFrameTime = atof(lpszPos);
	    }
	}
      // If the end of the file was reached before finding the time frame...
      if (Dump_Stream.eof())
	{
	  // Output Error
	  ParaListStream.str("");
	  ParaListStream << m_lpszName << PARAMETER_LIST_SEPARATOR;
	  ParaListStream << lpszDumpFile << PARAMETER_LIST_SEPARATOR;
	  ParaListStream << flTime << std::ends;
	  m_pApp->Message(MSG_TYPE_ERROR, 176, ParaListStream.str().c_str());
	  return FALSE;
	}
    }  

  // Loop through all of the coordinates and fill array with states
  for (j = 0; j < Dimensions.nY; j++)
    {
      for (i = 0; i < Dimensions.nX; i++)
	{
	  // Get the state of the next complex from dump file
	  Dump_Stream >> P_P_nState[i][j];
	  // Uncomment this for debugging
	  // printf("(%d, %d): %ld\n", i, j, P_P_nState[i][j]);
	}
    }
  // Close dump file
  Dump_Stream.close();

  return TRUE;
}
