/*************************************************************************
*
* FILENAME:	CA_Hexagonal.cpp
*
* CLASS:	C_Complex_Array_Hexagonal
*
* DESCRIPTION:	This class represents a two-dimensional array of complexes
*               with a hexagonal geometry.
*
* TYPE:		Core
*
*************************************************************************/


#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Calls constructor of base class and returns. Note that a
*               string specifying the geometry of this array type 
*               (INI_ARRAY_GEOMETRY_HEXAGON) is passed to the base class
*                constructor.
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

C_Complex_Array_Hexagonal::C_Complex_Array_Hexagonal(C_Application* p_App,
						     char* lpszName,
					       C_System* P_System,
					       long nSystemID,
					       C_Complex_Type_Multistate* P_CT,
					       C_Integer_Pair* pDimensions,
					       Bool bIsNeighbourSensitive,
					       long nBoundaryCondition)
  : C_Complex_Array(p_App, lpszName, P_System, nSystemID, P_CT, pDimensions,
		    bIsNeighbourSensitive, nBoundaryCondition,
		    INI_ARRAY_GEOMETRY_HEXAGON)
{}

/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Free memory allocated during construction or initialisation.
*
* PARAMETERS:	NONE
*
*************************************************************************/

C_Complex_Array_Hexagonal::~C_Complex_Array_Hexagonal(void)
{}

/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	This method initialises the state of the complex array.
*
* PARAMETERS:	NONE
*
*************************************************************************/

Bool
C_Complex_Array_Hexagonal::Init(void)
{
  long			i;
  long			j;
  C_Integer_Pair	Address;
  C_Integer_Pair	TmpAddress;
  C_Complex_Multistate*	P_Neighbours[6];
  enum Direction        {nNorth, nEast, nSouth, nWest, nNE, nSE, nSW, nNW};
  enum Neighbour        {nA, nB, nC, nD, nE, nF};
  int                   nNeighbour;
  char                  lpszParaList[MAX_MESSAGE_LENGTH];
	
  // NS-complexes of this array must have exactly six neighbours
  if (m_bIsNeighbourSensitive &&
      ((C_Complex_Type_Neighbour_Sensitive*)P_Type)->Get_Num_Neighbours() != 6)
    {
      sprintf(lpszParaList, "%ld%s%s%s%s%s%s",
	      ((C_Complex_Type_Neighbour_Sensitive*)P_Type)
	      -> Get_Num_Neighbours(),
	      PARAMETER_LIST_SEPARATOR, P_Type->Display_String(),
	      PARAMETER_LIST_SEPARATOR, Get_Geometry(),
	      PARAMETER_LIST_SEPARATOR, Get_Name());
      m_pApp->Message(MSG_TYPE_ERROR, 170, lpszParaList);
      return FALSE;
    }

  // Ensure that the number of rows are odd if boundary toroidal conditions
  // are specified. (This is ncecessary because the hexagonal lattice wraps
  // around cleanly only if the number of rows are even.)
  if (m_nBoundaryCondition == 0 && m_pDimensions->nY % 2 == 1)
    {
      sprintf(lpszParaList, "%ld%s%s%s%s", m_pDimensions->nY,
	      PARAMETER_LIST_SEPARATOR, Get_Name(),
	      PARAMETER_LIST_SEPARATOR, Get_Geometry());
      m_pApp->Message(MSG_TYPE_ERROR, 171, lpszParaList);
      return FALSE;
    }
	
  // If the complex type contained in this array is neighbour-sensitive
  if (m_bIsNeighbourSensitive)
    // Loop through all the complexes in the array
    for (i = 0; i < m_pDimensions->nX; i++)
      for (j = 0; j < m_pDimensions->nY; j++)
	{
	  Address.Set(i, j);  // Set the address variable
	  // Loop through all the neighbours of this complex
	  for(nNeighbour = 0; nNeighbour < 6; nNeighbour ++)
	    {
	      // Retrieve pointer to each neighbour (A, B, C, D, E and F)
	      switch(nNeighbour)
		{
		case nA:
		  // If j is even, neighbour A is to the northwest.
		  // Otherwise, j is odd, and neighbour A is to the north.
		  // If there is no northern neighbour, there is no
		  // northwestern neighbour, either.
		  // So start by getting northern neighbour.
		  P_Neighbours[nA] = Get_Neighbour(&Address, nNorth);
		  // If there was a northern neighbour, and j is even...
		  if (P_Neighbours[nA] != NULL && j % 2 == 0)
		    {
		      // Try to get western neighbour of northern neighbour
		      Get_Address(&TmpAddress, P_Neighbours[nA]);
		      P_Neighbours[nA] = Get_Neighbour(&TmpAddress, nWest);
		      if (m_nBoundaryCondition == 0 && P_Neighbours[nA] == NULL)
			{
			  m_pApp->Message(MSG_TYPE_ERROR, 172, 3,
					nA, Address.nX, Address.nY);
			  return FALSE;
			}
		    }
		  break;
		case nB:
		  // If j is even, neighbour B is to the north.
		  // Otherwise, j is odd, and neighbour A is to the northeast.
		  // If there is no northern neighbour, there is no
		  // northeastern neighbour, either.
		  // So start by getting northern neighbour.
		  P_Neighbours[nB] = Get_Neighbour(&Address, nNorth);
		  // If there was a northern neighbour, and j is odd...
		  if (P_Neighbours[nB] != NULL && j % 2 == 1)
		    {
		      // Try to get eastern neighbour of northern neighbour
		      Get_Address(&TmpAddress, P_Neighbours[nB]);
		      P_Neighbours[nB] = Get_Neighbour(&TmpAddress, nEast);
		      if (m_nBoundaryCondition == 0 && P_Neighbours[nB] == NULL)
			{
			  m_pApp->Message(MSG_TYPE_ERROR, 172, 3,
					nB, Address.nX, Address.nY);
			  return FALSE;
			}
		    }
		  break;
		case nC:
		  // Neighbour C is always to the east
		  P_Neighbours[nC] = Get_Neighbour(&Address, nEast);
		  if (m_nBoundaryCondition == 0 && P_Neighbours[nC] == NULL)
		    {
		      m_pApp->Message(MSG_TYPE_ERROR, 172, 3,
				    nC, Address.nX, Address.nY);
		      return FALSE;
		    }
		  break;
		case nD:
		  // If j is even, neighbour D is to the south.
		  // Otherwise, j is odd and neighbour A is to the southeast.
		  // If there is no southern neighbour, there is no
		  // southeastern neighbour, either.
		  // So start by getting southern neighbour.
		  P_Neighbours[nD] = Get_Neighbour(&Address, nSouth);
		  // If there was a northern neighbour, and j is odd...
		  if (P_Neighbours[nD] != NULL && j % 2 == 1)
		    {
		      // Try to get eastern neighbour of southern neighbour
		      Get_Address(&TmpAddress, P_Neighbours[nD]);
		      P_Neighbours[nD] = Get_Neighbour(&TmpAddress, nEast);
		      if (m_nBoundaryCondition == 0 && P_Neighbours[nD] == NULL)
			{
			  m_pApp->Message(MSG_TYPE_ERROR, 172, 3,
					nD, Address.nX, Address.nY);
			  return FALSE;
			}
		    }
		  break;
		case nE:
		  // If j is even, neighbour D is to the southwest.
		  // Otherwise, j is odd and neighbour A is to the south.
		  // If there is no southern neighbour, there is no
		  // southwestern neighbour, either.
		  // So start by getting southern neighbour.
		  P_Neighbours[nE] = Get_Neighbour(&Address, nSouth);
		  // If there was a northern neighbour, and j is even...
		  if (P_Neighbours[nE] != NULL && j % 2 == 0)
		    {
		      // Try to get western neighbour of southern neighbour
		      Get_Address(&TmpAddress, P_Neighbours[nE]);
		      P_Neighbours[nE] = Get_Neighbour(&TmpAddress, nWest);
		      if (m_nBoundaryCondition == 0 && P_Neighbours[nE] == NULL)
			{
			  m_pApp->Message(MSG_TYPE_ERROR, 172, 3,
					nE, Address.nX, Address.nY);
			  return FALSE;
			}
		    }
		  break;
		case nF:
		  // Neighbour F is always to the west
		  P_Neighbours[nF] = Get_Neighbour(&Address, nWest);
		  if (m_nBoundaryCondition == 0 && P_Neighbours[nF] == NULL)
		    {
		      m_pApp->Message(MSG_TYPE_ERROR, 172, 3,
				    nF, Address.nX, Address.nY);
		      return FALSE;
		    }
		  break;
		}
	    }
	  // Set all six nearest neighbours
	  ((C_Complex_Neighbour_Sensitive*)(Get_Complex(&Address)))
	    -> Set_Neighbours(P_Neighbours, 6);
	}
  return TRUE;
}
