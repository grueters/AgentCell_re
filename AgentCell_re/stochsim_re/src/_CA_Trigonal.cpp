/*************************************************************************
*
* FILENAME:	CA_Trigonal.cpp
*
* CLASS:	C_Complex_Array_Trigonal
*
* DESCRIPTION:	This class represents a two-dimensional array of complexes
*               with a trigonal geometry.
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
*               (INI_ARRAY_GEOMETRY_TRIANGLE) is passed to the base class
*                constructor.
*
* PARAMETERS:	C_Application*  p_App          - Pointer to parent application
*
*            	char*           lpszName              - Name of this complex
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

C_Complex_Array_Trigonal::C_Complex_Array_Trigonal(C_Application* p_App,
						   char* lpszName,
					       C_System* P_System,
					       long nSystemID,
					       C_Complex_Type_Multistate* P_CT,
					       C_Integer_Pair* pDimensions,
					       Bool bIsNeighbourSensitive,
					       long nBoundaryCondition)
  : C_Complex_Array(p_App, lpszName, P_System, nSystemID, P_CT, pDimensions,
		    bIsNeighbourSensitive, nBoundaryCondition,
		    INI_ARRAY_GEOMETRY_TRIANGLE)
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

C_Complex_Array_Trigonal::~C_Complex_Array_Trigonal(void)
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
C_Complex_Array_Trigonal::Init(void)
{
  long			i;
  long			j;
  C_Integer_Pair	Address;
  C_Complex_Multistate*	P_Neighbours[3];
  enum Direction        {nNorth, nEast, nSouth, nWest};
  enum Neighbour        {nA, nB, nC};
  int                   nNeighbour;
  char                  lpszParaList[MAX_MESSAGE_LENGTH];
	
  // NS-complexes of this array must have exactly three neighbours
  if (m_bIsNeighbourSensitive &&
      ((C_Complex_Type_Neighbour_Sensitive*)P_Type)->Get_Num_Neighbours() != 3)
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
  // Ensure that the number of rows are even if toroidal boundary conditions
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
	  for(nNeighbour = 0; nNeighbour < 3; nNeighbour++)
	    {
	      // Retrieve pointer to each neighbour (A, B and C)
	      switch(nNeighbour)
		{
		case nA:
		  // Neighbour A is always to the west
		    P_Neighbours[nA] = Get_Neighbour(&Address, nWest);
		  break;
		case nB:
		  // Neighbour B is always to the east
		  P_Neighbours[nB] = Get_Neighbour(&Address, nEast);
		  break;
		case nC:
		  // If i+j is even, neighbour C is to the south
		  if ((i + j) % 2 == 0)
		    P_Neighbours[nC] = Get_Neighbour(&Address, nSouth);
		  // Otherwise i+j is odd, so neighbour C is to the north
		  else
		    P_Neighbours[nC] = Get_Neighbour(&Address, nNorth);
		  break;
		}
	    }
	  // Set all three nearest neighbours
	  ((C_Complex_Neighbour_Sensitive*)(Get_Complex(&Address)))
	    -> Set_Neighbours(P_Neighbours, 3);
	}
  return TRUE;
}
