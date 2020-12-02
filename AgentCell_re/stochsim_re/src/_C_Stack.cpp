/*************************************************************************
*
* FILENAME:	C_Stack.cpp
*
* CLASS:	C_Complex_Stack
*
* DESCRIPTION:	This class is used to maintain a stack of C_Complex objects.
*		In particular, this is used to keep track of free complexes
*		within the system, ie complexes which exist as a program
*		object, but do not represent a physical complex at a
*		particular time. For instance, when a reaction occurs,
*		the substrates are pushed onto the stack, representing
*		their destruction, and the complexes are pulled off the
*		stack for the products, representing their creation.
*		This class performs the memory management and checking
*		required to maintain the stack. The stack is implemented
*		as an array of pointers to C_Complex objects within the
*		system.
*
* TYPE:		Core
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
* DESCRIPTION:	Initialises member variables
*
* PARAMETERS:   C_Application*  p_App     - Pointer to parent application
*
*************************************************************************/

C_Complex_Stack::C_Complex_Stack(C_Application* p_App)
{
  m_pApp = p_App;
  P_Complex = NULL;
}


/*************************************************************************
*
* METHOD NAME:	Init
*
* DESCRIPTION:	Initialises the complex stack so that it contains a
*		specified number of free complexes.
*
* PARAMETERS:	long nStackSize	- Number of complexes to be stored on stack.
*
*************************************************************************/

Bool
C_Complex_Stack::Init (long nStackSize)
{
  long i;

  // Set member variable representing the size of the stack
  m_nStackSize = nStackSize;
  // Ensure that the requested stack size can be accommodated
  // by the C_Complex* array
  if (m_nStackSize > MAX_LIMIT_COMPLEXES)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 18);
      return FALSE;
    }
  m_nStackTop = 0;  // Set the stack top (so that stack is empty)

  // Create array to store free complexes
  P_Complex = new C_Complex* [m_nStackSize];
  // If memory allocation failed
  if (P_Complex == NULL)
    {
      m_pApp->Message(MSG_TYPE_STATUS, 8, "Free Complexes");  // Output error
      return FALSE;
    }
  // Clear all the pointers (stack is currently empty)
  for (i = 0; i < m_nStackSize; i++)
    P_Complex[i] = NULL;
  return TRUE;
}


/*************************************************************************
*
* METHOD NAME:	Pop
*
* DESCRIPTION:	Removes a pointer to a C_Complex object from the stack and 
*		returns it. A check is made to ensure that the stack is
*		not empty. The Exists flag (which signifies whether or not
*		the complex exists physically within the system) of the
*		C_Complex object pointer is set before the pointer is
*		returned.
*
* RETURNS:	C_Complex*	- Pointer to C_Complex object
*
*************************************************************************/

C_Complex*
C_Complex_Stack::Pop(void)
{
  C_Complex* P_Complex_Top;
  // Check that the stack is not empty
  if (m_nStackTop < 1)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 3);
      return NULL;
    }
  m_nStackTop --;  // Lower stack top
  // Retrieve C_Complex pointer from stack
  P_Complex_Top = P_Complex[m_nStackTop];
  P_Complex[m_nStackTop] = NULL;  // Remove pointer from stack
  // Set Exists flag for C_Complex being returned
  P_Complex_Top->Set_Exists(TRUE);

// Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\t\t\tC_Complex_Stack::Pop: Popped complex off stack with ID %d\n",
		P_Complex_Top->Get_System_ID());	
#endif

  return P_Complex_Top;  // Return pointer
}


/*************************************************************************
*
* METHOD NAME:	Push
*
* DESCRIPTION:	Adds (pushes) a pointer to a C_Complex object onto stack.
*		The Exists flag (which signifies whether or not
*		the complex exists physically within the system) of the
*		C_Complex object pointer is cleared before returning.
*		A check is made to ensure that the stack is not full.
*
* PARAMETERS:	C_Complex* P_Complex_Top - C_Complex pointer to be added
*					   to stack.
*
*************************************************************************/

void
C_Complex_Stack::Push(C_Complex* P_Complex_Top)
{
// Output message if we're debugging in Microsoft environment
#ifdef _MSDEBUG
  _CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
		"\t\t\tC_Complex_Stack::Push: Pushing complex onto stack with ID %d\n",
		P_Complex_Top->Get_System_ID());	
#endif

  // Check that the stack top is not exceeded
  if (m_nStackTop >= m_nStackSize)
    {
      m_pApp->Message(MSG_TYPE_ERROR, 18);
      return;
    }
  // Clear the Exists flag for C_Complex being added to stack
  P_Complex_Top->Set_Exists(FALSE);
  // Add pointer to stack
  P_Complex[m_nStackTop] = P_Complex_Top;
  m_nStackTop ++;  // Raise stack top
  return;
}


/*************************************************************************
*
* METHOD NAME:	Destructor
*
* DESCRIPTION:	Frees memory allocated during initialisation
*
*************************************************************************/

C_Complex_Stack::~C_Complex_Stack(void)
{
  if (P_Complex != NULL)  // If complex array exists
    delete[] P_Complex;  // Delete complex array
}
