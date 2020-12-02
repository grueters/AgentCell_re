/*************************************************************************
*
* FILENAME:	CA_Square.cpp
*
* CLASS:	C_Complex_Array_Square
*
* DESCRIPTION:	This class represents a two-dimensional array of complexes
*               with a square geometry.
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
*               (INI_ARRAY_GEOMETRY_TRIGONAL) is passed to the base class
*                constructor.
*
* PARAMETERS:	C_Application*  p_App          - Pointer to parent application
*
*       :	char*           lpszName              - Name of this complex
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

C_Complex_Array_Square::C_Complex_Array_Square(C_Application* p_App,
					       char* lpszName,
					       C_System* P_System,
					       long nSystemID,
					       C_Complex_Type_Multistate* P_CT,
					       C_Integer_Pair* pDimensions,
					       Bool bIsNeighbourSensitive,
					       long nBoundaryCondition)
  : C_Complex_Array(p_App, lpszName, P_System, nSystemID, P_CT, pDimensions,
		    bIsNeighbourSensitive, nBoundaryCondition,
		    INI_ARRAY_GEOMETRY_SQUARE)
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

C_Complex_Array_Square::~C_Complex_Array_Square(void)
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
C_Complex_Array_Square::Init(void)
{
  long			i;
  long			j;
  C_Integer_Pair	Address;
  enum Neighbour        {nNorth, nEast, nSouth, nWest};
  int                   nNeighbour;
  C_Complex_Multistate*	P_Neighbours[4];
  char                 lpszParaList[MAX_MESSAGE_LENGTH];

  // NS-complexes of this array must have exactly four neighbours
  if (m_bIsNeighbourSensitive &&
      ((C_Complex_Type_Neighbour_Sensitive*)P_Type)->Get_Num_Neighbours() != 4)
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
	
  // If the complex type contained in this array is neighbour-sensitive
  if (m_bIsNeighbourSensitive)
    // Loop through all the complexes in the array
    for (i = 0; i < m_pDimensions->nX; i++)
      for (j = 0; j < m_pDimensions->nY; j++)
	{
	  Address.Set(i, j);  // Set the address variable
	  // Loop through all the neighbours of this complex
	  for(nNeighbour = 0; nNeighbour < 4; nNeighbour++)
	    {
	      // Retrieve pointer to each neighbour
	      // (north, south east and west)
	      switch(nNeighbour)
		{
		case nNorth:
		  P_Neighbours[nNorth] = Get_Neighbour(&Address, nNorth);
		  break;
		case nEast:
		  P_Neighbours[nEast] = Get_Neighbour(&Address, nEast);
		  break;
		case nSouth:
		  P_Neighbours[nSouth] = Get_Neighbour(&Address, nSouth);
		  break;
		case nWest:
		  P_Neighbours[nWest] = Get_Neighbour(&Address, nWest);
		  break;
		}
	    }
	  // Set all four nearest neighbours
	  ((C_Complex_Neighbour_Sensitive*)(Get_Complex(&Address)))
	    -> Set_Neighbours(P_Neighbours, 4);
	}
  return TRUE;
}
