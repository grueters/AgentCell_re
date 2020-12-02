/*************************************************************************
*
* FILENAME:	C_Type.cpp
*
* CLASS:	C_Complex_Type
*
* DESCRIPTION:	This class is used to store a particular type of complex.
*		Each individual complex 'moving around' within the system
*		belongs to a particular complex type (eg TTWWAA or TTWW).
*		The complex type keeps track of the levels of the complexes
*		(these levels are updated at certain times, according to
*		C_System::Run). The complex types contain a string which
*		is a list of their components used to identify them as
*		text. Each complex type has a null terminated list of
*		pointers to components, representing their composition.
*
* TYPE:		Core
*
*************************************************************************/


#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Initialises the complex type. The display string (the string
*		representing the composition of the complex type, aka the
*		composition string) is constructed from the list of pointers
*		to components (C_Component). The array of components is also
*		initialised from this array.
*
* PARAMETERS:   C_Application*  p_App          - Pointer to parent application
*               C_Component** P_Component_List - Null terminated list of
*						 pointers to components
*		int	      nIndex           - Index of complex type,
*						 which is stored internally.
*		C_System*     P_System	       - Pointer to the system 
*						 containing the complex
*                                                type (ie the parent system).
*
*************************************************************************/

C_Complex_Type::C_Complex_Type(C_Application* p_App,
			       C_Component** P_Component_List,
			       int nIndex, C_System* P_System)
{
  char lpszTemp[MAX_COMPONENT_SYMBOL_LENGTH * MAX_COMPONENTS_IN_COMPLEX];
  int  nComponentNum;

  m_pApp = p_App;

  // Initialise composition and create temporary display string

  nComponentNum = 0;
  lpszTemp[0] = NULL_CHAR;
  // Loop through component list until the end is reached
  while (P_Component_List[nComponentNum] != NULL)
    {
      // Add the component pointer to the array of components for this object
      P_Composition[nComponentNum] = P_Component_List[nComponentNum];
      // Add component symbol to string and move on to
      // the next component in the list
      strcat(lpszTemp, P_Component_List[nComponentNum]->Get_Symbol());
      nComponentNum ++;
    }
  // Null terminate component list
  P_Composition[nComponentNum] = NULL;

  // Initialise member variables

  // Create new string based on length of composition string
  m_lpszDisplayString = new char[strlen(lpszTemp) + 1];
  // Copy composition string created above into member variable display string
  strcpy(m_lpszDisplayString, lpszTemp);
  m_nIndex = nIndex;  // Initialise index
  P_Parent = P_System; // Copy pointer to parent of system
  m_nLevel = 0;
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Clears up object. Deletes pointer to the display string
*		created in the constructor.
*
*************************************************************************/

C_Complex_Type::~C_Complex_Type(void)
{
  if (m_lpszDisplayString != NULL)
    delete[] m_lpszDisplayString;
}


/*************************************************************************
*
* METHOD NAME:	Create_New_Complex
*
* DESCRIPTION:	Creates a new complex object for this complex type and
*		returns a pointer to the newly created object.
*
* PARAMETERS:  	long nSystemID - ID this complex has in the system.
*				 (Actually the # in the array of C_Complex
*				 in C_System).
*
* RETURNS:	C_Complex*     - Pointer to new complex of this type.
*
*************************************************************************/

C_Complex*
C_Complex_Type::Create_New_Complex(long nSystemID)
{
  C_Complex* P_Complex;
  // Attempt to create new C_Complex object
  P_Complex = new C_Complex(m_pApp, P_Parent, nSystemID);
  if (P_Complex != NULL)  // If creation was successful...
    {
      // Set complex object to be of this complex type
      P_Complex->Set_Type(this);
      // Indicate this complex object exists
      P_Complex->Set_Exists(TRUE);
    }
  else  // Otherwise output error
    m_pApp->Message(MSG_TYPE_ERROR, 8, "new Complex");	
  
  m_nLevel ++;  // Increment the amount of this complex type
  return P_Complex;  // Return pointer to this complex object
}


/*************************************************************************
*
* METHOD NAME:	Create
*
* DESCRIPTION:	Creates a new complex of this type during simulation. A
*		new object is not actually created, instead a placeholder
*		(pre-existing) object is retrieved from a stack and
*		initialised.
*
* PARAMETERS:	C_Reaction*		  - Pointer to reaction in reaction
*					    matrix which is occurring. (not
*                                           used here, but for compatibility
*                                           with descendants)
*
* RETURNS:	Bool	TRUE		  - Complex was created successfully.
*			FALSE		  - An error occurred while creating
*					    complex.
*
*************************************************************************/

Bool
C_Complex_Type::Create(C_Reaction*)
{
  C_Complex* P_Complex;
  P_Complex = P_Parent->P_Free_Complex->Pop();  // Pull free complex off stack
  if (P_Complex == NULL)
    return FALSE;
  // Set complex type of free complex according to product list
  P_Complex->Set_Type(this);

#ifdef _DYNAMIC_UPDATE  // If this is a build with averaging enabled...
  Inc_Level(P_Complex);  // Increment the level of this complex type now
#endif /* _DYNAMIC_UPDATE */


#ifdef _MSDEBUG  // Output message if we're debugging in Microsoft environment
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\t\tC_Complex_Type::Create: Creating '%s' with ID %d\n",
		m_lpszDisplayString, P_Complex->Get_System_ID());	
#endif
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Destroy
*
* DESCRIPTION:	Destroys a complex of this type during simulation. The
*		object is not actually destroyed, instead it is pushed
*		onto the stack to be an empty placeholder object.
*
* PARAMETERS:	C_Complex* P_Complex  - Pointer to the complex to be
*					destroyed
*
* RETURNS:		TRUE
*
*************************************************************************/

Bool
C_Complex_Type::Destroy(C_Complex* P_Complex)
{
#ifdef _MSDEBUG  // Output message if we're debugging in Microsoft environment
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\t\tC_Complex_Type::Destroy: Destroying '%s' with ID %d\n",
		m_lpszDisplayString, P_Complex->Get_System_ID());	
#endif

  // Push this complex onto free complex stack
  P_Parent->P_Free_Complex->Push(P_Complex);
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Get_Variables
*
* DESCRIPTION:	Creates an output variable for storage or display. Only one
*               output variable needs to be created for this complex type.
*
* PARAMETERS:	C_Output_Variable**  P_Variable
*                                               - Pointer to array of variables
*						  to be populated
*		long*		     p_nNumVar  - Pointer to number of 
*						  variables which are to be 
*						  created
*               long                            - not used here, but for
*                                                 compatibility with 
*                                                 descendants
*
* RETURNS:	Bool	TRUE		  - Variables were created successfully
*			FALSE		  - An error occurred while creating
*						variables
*		        &P_Variable	  - Populated array of pointers
*		        &p_nNumVar	  - Number of variables created
*
*************************************************************************/

Bool
C_Complex_Type::Get_Variables(C_Output_Variable** P_Variable, long* p_nNumVar,
			      long)
{
  
  C_Complex_Type* P_List_Of_CT[1];
  // Create list of complex types which the variable will represent
  // (for simple complex types, this list cannot be longer than one)
  P_List_Of_CT[0] = this;
  *p_nNumVar = 1;
  // Create new variable
  P_Variable[0] = new C_Output_Variable_Complex(m_pApp, Display_String(), P_List_Of_CT, 1);
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Get_Variables_From_File
*
* DESCRIPTION:	Creates any additional variables required by this complex
*		type from the INI file. The default implementation creates
*		no extra variables. In the descendant classes, this is
*		overriden to provide variables as necessary.
*
* PARAMETERS:	long*	p_nNumVar         - Pointer to number of variables 
*					    which are to be created	
*               long                      - not used here, but for
*                                           compatibility with descendants
*               Bool                      - not used here, but for
*                                           compatibility with descendants
*
* RETURNS:	Bool	TRUE		  - Variables were created successfully
*			FALSE		  - An error occurred while creating
*					    variables
*			&P_Variable	  - Populated array of pointers
*			&p_nNumVar	  - Number of variables created
*
*************************************************************************/

Bool
C_Complex_Type::Get_Variables_From_File(C_Output_Variable**, long* p_nNumVar,
					long, Bool)
{
  *p_nNumVar = 0;
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Calc_Effective_Total_Pr
*
* DESCRIPTION:	When the reaction matrix is being optimised, this is called
*		to return the  total probability that this complex type 
*		will react. 
*
* PARAMETERS:	C_Reaction** P_P_Reaction  - Pointer to array of reaction
*					     pointers which is being processed
*		int          nNumReactions - Number of reactions in array
*
*************************************************************************/

double
C_Complex_Type::Calc_Effective_Total_Pr(C_Reaction** P_P_Reactions,
					int nNumReactions)
{
  int		i;
  double	flSumPr;
  flSumPr = 0;
  // Loop through all the reactions in the array
  for (i = 0; i < nNumReactions; i++)
    // Update the running total
    flSumPr += P_P_Reactions[i]->Get_Probability();
  return flSumPr;  // Return the total
}
