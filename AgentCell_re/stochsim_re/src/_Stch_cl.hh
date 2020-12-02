/*************************************************************************
*
* FILENAME:	_Stch_cl.hh
*
* DESCRIPTION:	Class definitions (excluding random number generator classes).
*		See the file containing the method definitions for more
*		details about a particular class).
*
* TYPE:		Core
*
*************************************************************************/


#ifndef __STCH_CL_HH
#define __STCH_CL_HH

// Forward declaration of classes
class C_Application;
class C_Complex_Array;
class C_Output_Variable_Array_Snaps;

// Base class inherited by all dynamic objects
// (objects which can change over time)

class C_Dynamic_Object
{
  // Methods
public:

  // Destructor
  virtual
  ~C_Dynamic_Object(){}

  // Validates the dynamic value during initialisation
  virtual Bool
  Validate_Dynamic_Values(int, double*) = 0;

  // Executes an event, modifying this dynamic object during the simulation
  virtual void
  Exec_Event(double*, long*) = 0;
};


// Class for every individual reaction in the system

class C_Reaction : public C_Dynamic_Object
{
  // Attributes
private:
  // Pointer to parent application object
  C_Application* m_pApp;
  
  double          p;  // Probability that a given reaction will occur

  // Products of reaction (null terminated list of C_Complex_Types pointers)
  C_Complex_Type* P_Products[MAX_PRODUCTS_IN_REACTION + 1];

  long	          m_nID;  // Reaction identifier

  // Flag indicating if first substrate (row on reaction matrix)
  // is conserved in reaction  
  Bool	          m_bSubstrate_1_Conserved; 

  // Flag indicating if 2nd substrate (col on reaction matrix)
  // is conserved in reaction, for bimolecular reaction
  Bool	          m_bSubstrate_2_Conserved;

  // Array of flags indicating which products are conserved in reactions
 Bool	          m_bProducts_Conserved[MAX_PRODUCTS_IN_REACTION];

  // Methods
public:

  // Constructor (passes parent application object)
  C_Reaction(C_Application*);
  
  // Destructor
  virtual
  ~C_Reaction(void);
  
  // Set up new reaction
  Bool
  New_Reaction(long, C_Complex_Type**, double, long, Bool, Bool, Bool*);

  // Validates a probability
  Bool
  Validate(double);

  // Returns a pointer to a string containing the products
  char*
  Products_String(char*);

  // Returns the probability of a reaction
  double
  Get_Probability(void) {return p;}

  // Set the probability of a reaction to a new value
  void
  Set_Probability(double flPr) {p = flPr;}

  // Get a pointer to the product array for a reaction
  C_Complex_Type**
  Get_Products(void) {return P_Products;}

  // Returns the identifier of a reaction
  long
  Get_ID(void) {return m_nID;}

  // Returns flag indicating if first substrate of this reaction is conserved
  Bool
  Substrate_1_Conserved(void) {return m_bSubstrate_1_Conserved;}

  // Returns flag indicating if 2nd substrate of this reaction is conserved
  // (only valid for bimolecular reactions)
  Bool
  Substrate_2_Conserved(void) {return m_bSubstrate_2_Conserved;}

  // Returns flag indicating if a particular product is conserved
  Bool
  Product_Conserved(long nProductNum)
  {return m_bProducts_Conserved[nProductNum];}

  // Executes an event, modifying this dynamic object during the simulation
  virtual void
  Exec_Event(double* p_flPr, long*) {Set_Probability(p_flPr[0]);}

  // Validates the dynamic value during initialisation
  virtual Bool
  Validate_Dynamic_Values(int, double*);
};



// Class for a set of reactions that a given pair of complex types can undergo

class C_Reaction_Set
{
  // Attributes
private:
  // Pointer to parent application object
  C_Application* m_pApp;

  C_Reaction**	P_R; // Array storing pointers to all the reactions in this set

  int		m_nNumReactions;  // Number of reactions in a given set
	
  // Methods
public:

  // Constructor for class
  C_Reaction_Set(C_Application*);

  // Destructor used to free allocated memory
  ~C_Reaction_Set(void); 

  // Create new reaction in reaction set
  int
  New_Reaction(long, C_Complex_Type**, double, long, Bool, Bool, Bool*,
	       C_Reaction**);

  // Scales the probabilities of the reactions in the set
  void
  Scale(double);

  // Validates a reaction set
  Bool
  Validate(void);

  // Returns pointer to a string containing the products
  char*
  Products_String(char* lpszProducts, int nReactionNum)
  {return P_R[nReactionNum]->Products_String(lpszProducts);}

  // Returns number of reactions in set
  long
  Get_Num_Reactions(void) {return m_nNumReactions;}

  // Returns ID of a particular reaction
  long
  Get_ID(int nReactionNum) {return P_R[nReactionNum]->Get_ID();}

  // Returns pointer to reaction for easy access
  C_Reaction*
  Get_Reaction(int nReactionNum) {return P_R[nReactionNum];}

  // Returns pointer to array of reactions
  C_Reaction**
  Get_Reaction_Array(void) {return P_R;}
};




// Class used to store the sets of reactions in the system

class C_Reaction_Matrix
{

  // Attributes
private:
  // Pointer to parent application object
  C_Application* m_pApp;

  // 2D Array containing reaction sets, column 0 represents unimol reaction
  C_Reaction_Set*   P_P_Set[MAX_COMPLEX_TYPES + 1][MAX_COMPLEX_TYPES + 1];

  // Size of reaction matrix (square) 
  // ie its width (which is the same as its height)
  long		    m_nSize;

  // Pointer to array of pointers to C_Complex_Types stored in system
  C_Complex_Type**  P_P_Complex_Type;

  // Methods
private:

  // Returns pointer to a string containing the full reaction
  char*
  Reaction_String(char*, long, long, int);

public:

  // Constructor for matrix
  C_Reaction_Matrix(C_Application*, C_Complex_Type**, long);

  // Destructor used to free allocated memory
  ~C_Reaction_Matrix(void); 

  // Create new reaction in reaction matrix
  Bool
  New_Reaction(long, long, C_Complex_Type**, C_Complex_Type**, double, long,
	       C_Reaction**, C_Reaction**);

  // Optimises time increment and reaction probabilities
  Bool
  Optimise(double*, C_Complex_Type**);

  // Outputs reaction matrix to file
  Bool
  Dump(void);

  // Validates reaction matrix
  Bool
  Validate(void);

  // Returns a pointer to a reaction set for easy access
  C_Reaction_Set*
  Get_Set(int nRow, int nCol) {return P_P_Set[nRow][nCol];}

};




// Class for components of molecules ie proteins such as CheY, CheA

class C_Component
{
  // Attributes
private:		
  // Pointer to parent application object
  C_Application* m_pApp;

  // Name of the component eg CheY
  char	m_lpszName[MAX_COMPONENT_NAME_LENGTH];

  // Character symbol for component eg Y
  char	m_lpszSymbol[MAX_COMPONENT_SYMBOL_LENGTH];

  // Methods
public:

  // Returns pointer to name
  char*
  Get_Name(void) {return m_lpszName;}

  // Returns pointer to symbol
  char*
  Get_Symbol(void) {return m_lpszSymbol;}

  // Sets name
  void
  Set_Name(const char* lpszName) {strcpy(m_lpszName, lpszName);}

  // Sets symbol
  void
  Set_Symbol(const char* lpszSymbol) {strcpy(m_lpszSymbol, lpszSymbol);}
};




// Class for each complex type that can exist

class C_Complex_Type
{
  // Attributes
  // Counter used to keep track of levels of a particular complex type;
  // updated at specific intervals
public:
  long		m_nLevel;
private:

  // An array holding pointers to components (C_Components) of complex type
  C_Component*	P_Composition[MAX_COMPONENTS_IN_COMPLEX + 1];

  // String used to display the complex type eg TTWWAA (set in constructor)
  char*		m_lpszDisplayString;

protected:
  // Pointer to parent application object
  C_Application* m_pApp;

  // Pointer to the reaction system containing this complex type
  C_System*	P_Parent;

  // Index number for the complex type
  // (used to identify how to locate reaction within reaction matrix)
  int		m_nIndex;

  // Methods
public:	

  // Constructor used to set up C_Complex_Type based on array of components
  C_Complex_Type(C_Application*, C_Component**, int, C_System*);

  // Destructor used to clear memory allocated to class
  virtual
  ~C_Complex_Type(void);

  // Increments level by one
  // (not implemented as overloaded operator because of the way it is used)
  virtual void
  Inc_Level(C_Complex*) {m_nLevel ++;}

  // Resets current level of complex types to 0
  // (does not change Complex array; is used when counting complexes)
  virtual void
  Reset_Level(void) {m_nLevel = 0;}
  
  // Returns current level of complex type
  // (does not read Complex array, but is used after complexes are counted)
  long
  Get_Level(void) {return m_nLevel;}

  // Returns pointer to display string
  char*
  Display_String(void) {return m_lpszDisplayString;}
  
  // Get complex type index
  int
  Get_Index(void) {return m_nIndex;}

  // Get complex type index for a reaction
  virtual int
  Get_Reaction_Index(void) {return m_nIndex;}
  
  // Get pointer to composition array
  C_Component**
  Get_Composition(void) {return P_Composition;}


  // Create new complex of this type
  virtual C_Complex*
  Create_New_Complex(long);
  
  // Creates a new complex during simulation
  virtual Bool
  Create(C_Reaction*);

  // Destroys an existing object during simulation
  // (the object is turned into an empty placeholder)
  virtual Bool
  Destroy(C_Complex*);

  // Performs initialisation of complex types (only used in descendants)
  virtual Bool
  Init(C_Complex**, long) {return TRUE;}

  // Initialises variables for this complex type  
  virtual Bool
  Get_Variables(C_Output_Variable**, long*, long);

  // Initialises variables from file for complex type (empty place-holder)  
  virtual Bool
  Get_Variables_From_File(C_Output_Variable**, long*, long, Bool);
  
  // Returns total probability of all reactions in set modified accordingly
  virtual double
  Calc_Effective_Total_Pr(C_Reaction**, int);

// If this is a build with averaging enabled, define method to...
#ifdef _DYNAMIC_UPDATE
  // Decrement level by one
  // (not implemented as overloaded operator because of the way it is used)
  virtual void
  Dec_Level(C_Complex*) {m_nLevel --;}
#endif /* _DYNAMIC_UPDATE */
#ifdef _NS_R_DEBUG
  virtual void
  Inc_Count(C_Reaction*){;}
#endif /* _NS_R_DEBUG */

};

// Class for each complex that exists

class C_Complex
{

  // Attributes
private:	
  // Flag indicating whether complex exists or is just an empty placeholder
  Bool		   m_bExists;
  
  // ID of this complex within the system
  long		   m_nSystemID;

protected:
  // Pointer to parent application object
  C_Application* m_pApp;
  
  
  // Pointer to the reaction system containing this complex
  C_System*	   P_Parent;
    
  // Pointer to the complex type this instance represents
  C_Complex_Type*  P_Type;

  // Methods
private:
  
  // Performs bimolecular reaction
  virtual Bool
  Bimolecular_React(C_Complex*, C_Reaction*);

  // Performs unimolecular reaction
  virtual Bool
  Unimolecular_React(C_Reaction*);

  // Get any modification to the probability of a reaction 
  // (used mainly be descendants)
  virtual double
  Get_Relative_Probability(C_Reaction*) {return 1.0;}
  
public:
  
  // Constructor used to initialise object
  C_Complex(C_Application*, C_System*, long);

  // Virtual destructor must be defined so that derived classes can be
  // deleted correctly
  virtual
  ~C_Complex(void){};

  // Used to see if complex is going to react with another complex
  Bool
  Test_Reaction(C_Complex*);
  
  // Returns flag indicating if complex exists
  Bool
  Get_Exists(void) {return m_bExists;}

  // Sets flag indicating complex exists
  void
  Set_Exists(Bool bExists) {m_bExists = bExists;}

  // Returns pointer to type of complex
  C_Complex_Type*
  Get_Type(void) {return P_Type;}

  // Sets pointer to complex type
  void
  Set_Type(C_Complex_Type* pType) {P_Type = pType;}

  // Returns system ID
  long
  Get_System_ID(void) {return m_nSystemID;}
  
  // Equilibrates all rapid equilibria prior to a reaction
  // (only used in descendants)
  virtual void
  Equilibrate() {;}								
  // Update the complex during reaction - only used in descendants
  virtual void
  Update(C_Reaction*) {return;}
	
};




// Class representing a stack of pointers to complexes which are unused / empty

class C_Complex_Stack
{

  // Attributes
private:	
  // Pointer to parent application object
  C_Application* m_pApp;
  
  // Dynamically created array pointing to members of Complex in C_System
  // which are unused
  C_Complex**		P_Complex;

  // Array index of the top of the stack
  long			m_nStackTop;

  // Size of stack
  long			m_nStackSize;

  // Methods
public:

  // Constructor used to initialise member variables
  C_Complex_Stack(C_Application*);

  // Destructor frees allocated memory
  ~C_Complex_Stack(void);

  // Initialise stack to a specified size
  Bool
  Init(long);

  // Remove and return top pointer from stack
  C_Complex*
  Pop(void);

  // Push pointer onto stack
  void
  Push(C_Complex*);

  // Returns the number of free spaces in the stack
  long
  Free_Space(void) {return (m_nStackSize - m_nStackTop);}

};


// Abstract class inherited by various output variable objects

class C_Output_Variable
{
  // Attributes
protected:
  // Pointer to parent application object
  C_Application* m_pApp;
  
  // Name of this variable
  char		   m_lpszName[MAX_OUTPUT_VAR_NAME_LENGTH];

  // Methods
public:
  
  // Constructs variable with specified name
  C_Output_Variable(C_Application*, char*);

  // Virtual method declaration for descendant classes
  virtual
  ~C_Output_Variable(void){;}

  // Gets the name of this variable
  char*
  Get_Name(void) {return m_lpszName;}

  // Virtual method declaration for descendant classes
  virtual long
  Get_Value(void) {return 0;}

  // Virtual method declaration for descendant classes
  virtual long**
  Get_Values(void) {return NULL;}

  // Virtual method declaration for descendant classes
  virtual void
  Update(void) {;}

  // Virtual method declaration for descendant classes
  virtual void
  Reset(void) {;}
};

// Class used to store the output variables used for display
// and storage purposes

class C_Output_Variable_Complex : public C_Output_Variable
{

  // Attributes
protected:

  // Number of types this variable represents
  int		   m_nNumTypes;

  // List of complex types this variable represents
  C_Complex_Type*  P_Complex_Type[MAX_TYPES_IN_OUTPUT_VAR];

  // Methods
public:

  // Constructs variable with specified name and list of complex types
  C_Output_Variable_Complex(C_Application*, char*, C_Complex_Type**, long);

  // Destroys output variable - must be declared as virtual for derived classes
  virtual
  ~C_Output_Variable_Complex(void) {}

  // Calculates and returns the value of this variable
  virtual long
  Get_Value(void);
};

// Descendant class of C_Output_Variable with averaging functionality

class C_Output_Variable_Complex_Averaged : public C_Output_Variable_Complex
{

  // Attributes
private:
  
  // Cumulative value of sampled variable values
  long			m_nCumValue;

  // Current number of values which have been sampled 
  long			m_nNumSampled;

  // Methods
public:
  
  // Constructs averaging variable with specified name
  // and list of complex types
  C_Output_Variable_Complex_Averaged(C_Application*, char*, C_Complex_Type**,
				     long);

  // Destroys averaging output variable
  ~C_Output_Variable_Complex_Averaged(void) {}
  
  // Returns current averaged value for this variable
  long
  Get_Value(void);
  
  // Sample present value of this variable and add to cumulative value
  void
  Update(void);
  
  // Resets the cumulative value of this variable
  void
  Reset(void) {m_nCumValue=0; m_nNumSampled=0;}
};
		
// Class used to store variable values for particular iterations

class C_Output_Variable_Store
{

  // Attributes
private:
  // Pointer to parent application object
  C_Application* m_pApp;

  // Pointer to array of pointers to variables stored in output manager
  C_Output_Variable**  P_P_Variable;

  // Array of pointers to memory used to store variable values
  //  at different times
  long*                m_pnValue[MAX_NUM_DUMP_VARIABLES];
  
  // Pointer to the times at which the variable values are stored
  double*	       m_pflTime;

  // Number of values currently stored
  long		       m_nNumValuesStored;

  // Number of variables being stored
  int		       m_nNumVariables;

  // File name of the file used to store the values in
  char		       m_lpszValuesFileName[MAX_FILE_NAME_LENGTH];

  // Width of columns in file
  int		       m_nColumnWidth;

  // Floating point precision for output streams
  int		       m_nFPPrecision;

  // Flag indicating if variables should be stored in memory
  // during the simulation (possible for calculations)
  Bool		       m_bMemoryStore;

  // Flag indicating if variables should be saved in binary files
  Bool		       m_bBinaryStore;

  // Methods
private:
  
  // Writes a set of values to a file in ascii format
  Bool
  Save_Values(long);
  
  // Writes a set of values to a binary file
  Bool
  Save_Values_Bin(long);
  
public:

  // Constructor used to set member variables
  C_Output_Variable_Store(C_Application*, C_Output_Variable**, long, char*,
			  Bool, double, double, Bool bBinaryStore = FALSE);
  
  // Destroys arrays used to hold values
  ~C_Output_Variable_Store(void);

  // Initialises the variables store
  Bool
  Init(long);
  
  // Stores variable values at specified times and saves them in a file
  Bool
  Store(double);

  // Updates variable values (for output averaging)
  void
  Update();

  // Resets variable values (for output averaging)
  void
  Reset();

};



// Class used to store information for objects which change over time - events

class C_Event
{

  // Attributes
private:
  // Pointer to parent application object
  C_Application* m_pApp;

public:
  
  // Time at which this event will occur (measured in iterations)
  double	             m_flTime;
  
  // Time at which this event will occur (measured in iterations)
  unsigned long long	     m_nTime;
  
  // New values the dynamic object will have
  double	     m_flNewValues[MAX_NUM_DYNAMIC_VALUES];

  // Number of values which will change
  int		     m_nNumValues;
    
  // Pointer to parameter list used to specify which part
  // of dynamic object is changing (depends on what object is)
  long*		     m_pParameter;

  char*		     m_lpszName;  // Name of the event

  // Pointer to the object which will be changed over time
  C_Dynamic_Object*  m_pDynObject;

  // Methods
public:

  // Constructor for object
  C_Event(C_Application*);

  // Destructor for object
  ~C_Event(void);

  // Initialises the event object
  Bool
  Init(double, char*, C_Dynamic_Object*, double*, int, long*, int);

  // Executes this event
  void
  Exec_Event(void);

  // Returns the time at which this event will occur
  double
  Get_flTime(void) {return m_flTime;}

  // Returns the time at which this event will occur
  unsigned long long
  Get_nTime(void) {return m_nTime;}

  // Returns the name of the event
  char*
  Get_Name(void) {return m_lpszName;}
  
  // Scales the time to convert from seconds to iterations
  void
  Scale_Time(double flTimeInc)
  {m_nTime = (unsigned long long) floor(m_flTime / flTimeInc);}	
};




// Class used to manage the events in the system

class C_Event_Manager
{

  // Attributes
private:
  // Pointer to parent application object
  C_Application* m_pApp;

public:
  
  // Sorted list of pointers to C_Event objects
  C_Event*	P_Event_List[MAX_NUM_EVENTS];
  
  // Time at which the next event will occur (measured in iterations)
  unsigned long long m_nNextEventTime;

  long		m_nNextEvent;  // Index number of the next event

  long		m_nNumEvents;  // Number of events in the event list
		
  // Methods
private:
  
  // Adds event to event list in initialisation
  Bool
  Add_Event(double, char*, C_Dynamic_Object*, double*, int,
	    long* p_nParameter = NULL, int nNumParameters = 0);

public:

  // Constructor for object
  C_Event_Manager(C_Application*);

  // Destructor for object
  ~C_Event_Manager(void);

  // Initialise the event manager
  Bool
  Init(int, double);
  
  // Return the time at which the next event will occur
  unsigned long long
  Get_Next_Event_Time(void) {return m_nNextEventTime;}
  
  // Executes the next event
  void
  Exec_Event(void);
  
  // Initialises object from INI
  Bool
  Init_Dynamic_Object(C_Dynamic_Object*, char*, double, int nNumValues = 1,
		      long* p_nParameter = NULL, int nNumParameters = 0);

};


class C_Output_Manager
{
  // Attributes
protected:
  // Pointer to parent application object
  C_Application* m_pApp;

  // Pointer to the system to which this object belongs
  C_System*                       P_Parent;

  // Methods
public:
  // Constructor
  C_Output_Manager(C_Application*, C_System*);

  // Destructor
  virtual
  ~C_Output_Manager(void);

  // Initialises the output manager
  virtual Bool
  Init(long, double, unsigned long long){return TRUE;}

  // Handles output operations for during every simulation iteration
  virtual Bool
  Handle_Output(double){return TRUE;}

  // Returns a pointer to the list of display variables
  virtual C_Output_Variable**
  Get_DisplayVarList(void) {return NULL;}

  // Returns the number of display variables in the list
  virtual long
  Get_NumDisplayVar(void) {return 0;}

  // Adds a newly constructed dump variable to the list
  virtual Bool
  Add_DumpVariable(C_Output_Variable*){return TRUE;}

  // Adds a newly constructed dump variable to the list
  virtual Bool
  Add_DisplayVariable(C_Output_Variable*){return TRUE;}

  // Adds a newly constructed dump variable to the list
  virtual Bool
  Add_SnapsVariable(C_Output_Variable*, unsigned long long, Bool, unsigned long long,
		    unsigned long long)
  {return TRUE;}

  // Displays and stores the current values of all variables
  virtual Bool
  Display_And_Store(double){return TRUE;}
};

// Class for handling output of complex level information

class C_Output_Manager_Complex : public C_Output_Manager
{
  // Attributes
private:
  // Array containing all the variables to be displayed
  // and stored during the simulationn
  C_Output_Variable_Complex*	  P_Display_Var[MAX_NUM_DISPLAY_VARIABLES];

  // Array containing all the variables to be stored
  // in a dump file during the simulation
  C_Output_Variable_Complex*	  P_Dump_Var[MAX_NUM_DUMP_VARIABLES];
  
  // Pointer to class storing the display variable values at certain times
  C_Output_Variable_Store*        P_Output_Variable_Disp_Store;
  
  // Pointer to class storing the dump variable values at certain times
  C_Output_Variable_Store*        P_Output_Variable_Dump_Store;
  
  // How often values are stored (in terms of number of iterations)
  unsigned long long		          m_nStoreInterval;

  // How often values are stored (in terms of number of iterations)
  unsigned long long		          m_nDumpStoreInterval;

  // How often values are displayed (in terms of number of iterations)
  unsigned long long		          m_nDisplayInterval;

  // The number of dump variables in the system
  long			          m_nNumDumpVar;

  // The number of display variables in the system
  long			          m_nNumDisplayVar;

  // Flag specifying whether output should be averaged
  Bool			          m_bAveraging;

  // Sampling frequency for averaged output
  unsigned long long		          m_nSampleInterval;

  // Iteration counter for display variable storage
  unsigned long long                   m_nStoreItCount;

  // Iteration counter for variable display
  // Iteration counter for dump variable storage
  unsigned long long                   m_nDumpStoreItCount;

  unsigned long long                   m_nDisplayItCount;

  // Iteration counter for variable sampling
  unsigned long long                   m_nSampleItCount;

  // The number of iterations to be performed in the simulation
  unsigned long long              m_nNumIterations;

  // The size of the time increment
  double                          m_flSimDuration;

  // Methods
private:

  // Initialises the objects used to store variables
  Bool
  Init_Variable_Store(double, double, Bool bBinaryStore);
  
public:
  // Constructor
  C_Output_Manager_Complex(C_Application*, C_System*);

  // Destructor
  ~C_Output_Manager_Complex(void);

  // Handles output operations for during every simulation iteration
  virtual Bool
  Handle_Output(double);

  // Displays and stores the current values of all variables
  Bool
  Display_And_Store(double);
  
  // Initialises the output manager
  virtual Bool
  Init(long, double, unsigned long long);

  // Adds a newly constructed dump variable to the list
  Bool
  Add_DumpVariable(C_Output_Variable*);

  // Adds a newly constructed dump variable to the list
  Bool
  Add_DisplayVariable(C_Output_Variable*);

  // Handles any output operations for a specified iteration
  Bool
  Output(long);

  // Returns a pointer to the list of display variables
  C_Output_Variable**
  Get_DisplayVarList(void) {return (C_Output_Variable**) P_Display_Var;}

  // Returns the number of display variables in the list
  long
  Get_NumDisplayVar(void) {return m_nNumDisplayVar;}
};


// Class specifying the reaction system: complexes and reactions

class C_System
{

  // Attributes
private:
  // Pointer to the parent application object
  C_Application* m_pApp;

  // The reaction matrix specifying all the reactions
  // between different complex types
  C_Reaction_Matrix*		  P_R_M;
  
public:
  // Pointer to the event manager
  C_Event_Manager*		  P_Event_Manager;

  // Pointer to the list of output managers
  C_Output_Manager*       P_Output_Manager[MAX_NUM_OUTPUT_MANAGERS];
  
private:

  // Array containing all the type of components in the system
  C_Component		          Component[MAX_COMPONENTS];
  
  // Array containing pointers to all the possible complex types in the system
public:
  C_Complex_Type*	          P_Complex_Type[MAX_COMPLEX_TYPES];
private:

  // Dynamically created array containing all the complexes in the system,
  // plus empty placeholders
  C_Complex**		          P_Complex;
  
  // Array containing all of the complex arrays in the system
  // (memory allocated in constructor)
  C_Complex_Array**	          P_Complex_Array;
  
  // Volume in which the reactions are occurring 
  double			  m_flReactionVolume;

public:
  // Duration of each time interval / slice during simulation
  double			  m_flTimeInc;

  // Duration of each time interval / slice during simulation
  unsigned long long		  m_nIterationsDone;

  // Number of iterations to perform in simulation
  unsigned long long		  m_nNumIterations;
private:

  // Maximum number of complexes in simulation
  // (used to calculate probability parameters)
  long			          m_nMaxNumComplexes;

  // The number of 'empty' complexes used to decide
  // if a unimolecular reaction will occur
  long			          m_nNumUniPseudoComplexes;

  // The number of output managers in the system
  long			          m_nNumOutputManagers;

  // Frequency at which complex arrays are equilibrated
  unsigned long long* 		  m_pnArrayEquilInterval;

  // Iteration counter for array equilibration
  unsigned long long*                  m_pnArrayEquilItCount;

  // The number of components in the system
  int				  m_nNumComponents;

  // The number of complex types in the system
  int				  m_nNumComplexTypes;

  // The number of complex arrays in the system
  int				  m_nNumArrays;

  // Flag specifying whether spatial extensions are being used
  Bool			          m_bUsingSpatialExtensions;

  // Flag specifying whether arrays of complexes are being used
  Bool			          m_bUsingArrays;

public:
  // Pointer to the random number generator object
  // (the exact type of generator is assigned dynamically)
  C_Random_Generator*	          P_Ran_Gen;
  
  // Pointer to class containing pointers to members
  // of Complex which do not exist - ie are empty
  C_Complex_Stack*	          P_Free_Complex;

#ifdef _NSDEBUG
  long	EQUIL_COUNT;
#endif

  // Methods
private:	

  // Initialises complex levels (called by constructor)
  Bool
  Init_Complex_Levels(void);
  
  // Initialises complex types (called by constructor)
  Bool
  Init_Complex_Types(void);

  // Post-initialises complex types (called by constructor) 
  // after reactions have been created
  Bool
  Init_Complex_Types_Post(void);

  // Initialises fuzzy complex types (called by constructor)
  Bool
  Init_Complex_Types_Fuzzy(void);

  // Initialises special complex types (called by constructor)
  Bool
  Init_Complex_Types_Special(void);

  // Initialises neighbour-sensitive complex types (called by constructor)
  Bool
  Init_Complex_Types_Neighbour_Sensitive(void);

  // Initialises complex arrays (called by constructor);
  Bool
  Init_Complex_Arrays(void);
  
  // Initialises components from file (called by constructor)
  Bool
  Init_Components(void);

  // Initialises the event manager (called by constructor)
  Bool
  Init_Event_Manager(int);
  
  // Initialises complexes and matrix (constructor)
  Bool
  Init_Reactions(long*, C_Reaction**,char lpszDynCode[][MAX_INI_VALUE_LENGTH],
		 double*);
  
  // Optimises matrix. sets up dyn reactions (constructor)
  Bool
  Init_Reactions_Optimise(long, C_Reaction**,
			  char lpszDynCode[][MAX_INI_VALUE_LENGTH], double*);
  
  // Initialises the dump variables
  Bool
  Init_Dump_Variables(void);
  
  // Initialises the display variables
  Bool
  Init_Display_Variables(void);
  
  // Initialises the complex array variables
  Bool
  Init_Array_Variables(void);
  
  // Initialises the output manager
  Bool
  Init_Output_Managers(long);
    
  // Get information about what spatial extensions are being used
  Bool
  Init_Spatial_Extensions(void);
    
  // Runs an individual iteration of the simulation
  Bool
  Run_Iteration(void);

  // Returns a pointer to the complex type represented by a string
  C_Complex_Type*
  Identify_Complex_Type(const char*);
  
public:
  
  // Constructor with explicit passing of application pointer
  C_System(C_Application*);

  // Destructor - close files, clear memory etc.
  virtual
  ~C_System(void);

  // Initialises all objects in system
  Bool
  Init(void);

  // Runs the simulation - selects random Complexes and reacts them
  Bool
  Run(void);

  // Steps the simulation by the time interval specified by argument
  Bool
  Step(double);

  // Returns a pointer to the reaction matrix
  C_Reaction_Matrix*
  Get_Reaction_Matrix(void) {return P_R_M;}

  // Returns a pointer to the event manager
  C_Event_Manager*
  Get_Event_Manager(void) {return P_Event_Manager;}

  // Replaces a complex in the array with another
  // (used by multistate complexes)
  void
  Replace_Complex(long nSystemID, C_Complex* P_New_Complex)
  {P_Complex[nSystemID] = P_New_Complex;}
  
  // Updates the current complex type levels
  void
  Calc_Complex_Type_Levels(void);
};


// Abstract base class specifying the application
// - it is inherited for the Console and Windows-based programs

class C_Application
{

  // Attributes
public:
  // ID of this StochSim application instance (for wrapper code)
  int m_nRunID;

  // Path name for explicit specification of working directory
  char			m_lpszWorkingDir[MAX_FILE_NAME_LENGTH];

private:
  
// Name of log file where all output will be stored
  char			m_lpszLogFileName[MAX_FILE_NAME_LENGTH];

  // Name of application INI file
  char			m_lpszAppINIFile[MAX_FILE_NAME_LENGTH];

  // Methods
private:

  // Must be overriden to output error message to display;
  // base function outputs string to file
  virtual void
  ErrOut(const char*, int nErrorCode = -1) = 0;
  
  // Must be overriden to output string to display;
  // base function outputs string to file
  virtual void
  StatusOut(const char*) = 0;

public:	
  
  // Constructor for application
  C_Application(const char*);
  
  // Constructor for application (with explicit CWD setting)
  C_Application(const char*, const char*);
  
  // Destructor
  virtual
  ~C_Application(void){}

  // Initialises class (opens files)
  Bool
  Init(void);
  
    // Runs the application (must be overriden)
  virtual Bool
  Run(void) = 0;
  
    // Runs the application (must be overriden)
  virtual Bool
  Run(C_Application*) = 0;
  
  // Displays messages from message file with all string parameters
  void
  Message(int, long, const char* lpszConstParaList = NULL);

  // Displays messages from message file with up to 5 numerical parameters
  void
  Message(int, long, int, long nPara1, long nPara2=0, long nPara3=0,
	  long nPara4=0, long nPara5=0);
 
  // Displays messages from message file with up to 5 string parameters
  void
  Message(int, long, int, const char* nPara1, const char* nPara2=NULL,
	  const char* nPara3=NULL, const char* nPara4=NULL,
	  const char* nPara5=NULL);

  // Method to retrieve string parameter from an INI file
  Bool
  Get_INI(const char*, const char*, char*, const char* lpszINIFile = NULL,
	  Bool bSilent = FALSE);

  // Method to retrieve long parameter from an INI file
  Bool
  Get_INI(const char*, const char*, long*, const char* lpszINIFile = NULL,
	  Bool bSilent = FALSE);
  
  // Method to retrieve double parameter from an INI file
  Bool
  Get_INI(const char*, const char*, double*, const char* lpszINIFile = NULL,
	  Bool bSilent = FALSE);

  // Performs any required initialisation of output device
  // based on display variables
  virtual void
  Init_Display_Variables(C_Output_Variable**, long);

  // Displays variable values at a certain time (must be overriden)
  virtual void
  Display_Variable_Values(double, C_Output_Variable**, double, long) = 0;
};


#endif /* __STCH_CL_HH */
