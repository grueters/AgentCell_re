/*************************************************************************
*
* FILENAME:	_Stch_spc.HH
*
* DESCRIPTION:	Class definitions for spatial structures and related objects.
*		See the file containing the method definitions for more
*		details about a particular class).
*
* TYPE:		Core
*
*************************************************************************/


#ifndef __STCH_SPC_HH
#define __STCH_SPC_HH


// Definitions controlling memory assignment
// (if any of these are changed, update documentation)

#define MAX_NUM_COMPLEX_ARRAYS		4096
#define MAX_NUM_ARRAY_SNAPS_STATES      16
#define	MAX_ARRAY_WIDTH			128
#define	MAX_ARRAY_LENGTH		128
#define MAX_ARRAY_NAME_LENGTH		128
#define MAX_ARRAY_VAR_NAME_LENGTH	128
#define MAX_NUM_ARRAY_WATCH_COORD	1024
#define MAX_ARRAY_COORD_STRING_LENGTH	64
#define ARRAY_AVERAGE_FREQ		0.00001
#define MAX_NUM_ARRAY_SNAPS_VARIABLES   4096
#define MAX_NUM_SNAPS_VARIABLE_SETS     MAX_NUM_ARRAY_SNAPS_VARIABLES
#define MAX_NUM_ARRAY_DUMP_VARIABLES    4096
#define MAX_NUM_SNAPS_VARIABLES_IN_SET  128

// Definitions controlling output format for complex arrays
// (if any of these are changed, update documentation)

#define ARRAY_COLUMN_SEPARATOR		""
#define ARRAY_DUMP_COLUMN_SEPARATOR	" "
#define ARRAY_OUTPUT_FILE_SUFFIX	".OUT"
#define	ARRAY_OUTPUT_SNAPS_SUFFIX	"SNAPS"
#define	ARRAY_OUTPUT_DUMP_SUFFIX	"DUMP"
#define	ARRAY_OUTPUT_COORD_SUFFIX	"COORD"
#define ARRAY_OUTPUT_COLUMN_WIDTH	1
#define ARRAY_DUMP_COLUMN_WIDTH	        8
#define MAX_ARRAY_AVERAGE_VALUE		15
#define ARRAY_OUTPUT_ON_STRING		"*"
#define ARRAY_OUTPUT_OFF_STRING		"."
#define ARRAY_AVERAGE_OUTPUT_COLUMN_WIDTH  4


// Definitions of parameter and section names for complex array INI file
// (for details of each parameter, see INI file)

#define INI_COMPLEX_ARRAY_SECTION	"Complex Array"
#define INI_ARRAY_GENERAL_SECTION	"General"
#define INI_COMPLEX_ARRAY		"Array"
#define INI_COMPLEX_ARRAYS		"Arrays"
#define INI_ARRAY_GEOMETRY	        "Geometry"
#define INI_ARRAY_GEOMETRY_LINE         "Line"
#define INI_ARRAY_GEOMETRY_SQUARE       "Square"
#define INI_ARRAY_GEOMETRY_TRIANGLE     "Triangle"
#define INI_ARRAY_GEOMETRY_HEXAGON      "Hexagon"
#define INI_ARRAY_SEPARATOR		","
#define INI_ARRAY_INI_FILE		"ArrayINIFile"
#define INI_ARRAY_X_DIMENSION		"XDimension"
#define INI_ARRAY_Y_DIMENSION		"YDimension"
#define INI_ARRAY_COMPLEX		"Complex"
#define INI_ARRAY_AVERAGE_INTERVAL	"AverageInterval"
#define INI_ARRAY_FILE_NAME		"ARRAY.INI"
#define INI_ARRAY_OUT_PREFIX		"ArrayOutPrefix"
#define INI_ARRAY_OUTPUT_FILE_PREFIX	"ARRAY"
#define INI_HIGHLIGHTED_STATES		"HighlightedStates"
#define INI_WATCH_COORDINATES	        "WatchCoordinates"
#define INI_ARRAY_COORDINATE_SEPARATOR	","
#define INI_NEIGHBOUR_SENSITIVE		"NeighbourSensitive"
#define INI_ARRAY_INITIAL_STATE		"InitialState"
#define INI_ARRAY_INITIAL_STATE_FILE	"InitialStateFile"
#define INI_ARRAY_INITIAL_STATE_FROM_TIME "InitialStateFromTime"
#define INI_ARRAY_EQUIL_FREQ		"EquilibrationFrequency"
#define INI_ARRAY_EQUIL_INTERVAL	"EquilibrationInterval"
#define INI_ARRAY_CREATE_DUMP_FILE      "CreateDumpFile"
#define INI_ARRAY_BOUNDARY_CONDITION    "BoundaryCondition"
#define INI_ARRAY_TOROIDAL_BOUNDARIES   "Toroidal"
#define INI_ARRAY_NORMAL_BOUNDARIES     "Normal"
#define INI_ARRAY_NUM_SNAPS_VAR         "NumSnapshotVariables"
#define INI_ARRAY_SNAPS_VAR_SECTION	"Snapshot Variable "
#define INI_ARRAY_SNAPS_VAR_NAME	"Name"
#define INI_ARRAY_SNAPS_VAR_STATES	"States"
#define INI_DEFAULT_STORE_INTERVAL	"DefaultSoreInterval"
#define INI_DUMP_INTERVAL	        "DumpInterval"
#define ARRAY_NAME_FORMAT_STRING	"%s_%s"
#define ARRAY_NAME_SEPARATOR            "_"
#define ARRAY_COORDINATE_FORMAT_STRING	"(%ld:%ld)"

#define ARRAY_NUM_INIT_CONDITIONS	1
#define	ARRAY_FROM_CONC_STRING		"FromConcentration"
#define	ARRAY_FROM_CONC_NUM	        1
#define	ARRAY_FROM_DUMP_STRING		"FromDumpFile"
#define	ARRAY_FROM_DUMP_NUM	        2

// Definitions for output manager types
#define OM_ARRAY                        1

// For backwards compatibility
// Same as INI_ARRAY_GEOMETRY_TRIANGLE
#define INI_ARRAY_GEOMETRY_TRIGONAL     "Trigonal"
// Same as INI_ARRAY_GEOMETRY_HEXAGON
#define INI_ARRAY_GEOMETRY_HEXAGONAL    "Hexagonal"

// Forward declaration of classes
class C_Output_Variable_Array;
class C_Output_Variable_Array_Snaps;

class C_Complex_Array
{
  //Attributes
private:

  long			m_nSystemID;
  
  // Pointer to 2D array of complex objects
  C_Complex_Multistate***  P_P_Complexes;
  
  // Name of this array	
  char			m_lpszName[MAX_ARRAY_NAME_LENGTH];
  
  // Geometry of this array	
  char			m_lpszGeometry[MAX_ARRAY_NAME_LENGTH];
  
protected:

  // Pointer to parent application object
  C_Application* m_pApp;

  // Pointer to dimension values
  C_Integer_Pair*	m_pDimensions;  
  
  // Boundary conditions for this array
  long                  m_nBoundaryCondition;

  // Flag indicating whether the type of the complexes in this array
  // are neighbour-sensitive
  Bool			m_bIsNeighbourSensitive;

  // Pointer to parent system of this complex array
  C_System*			P_Parent;
  
  // Pointer to complex type contained in this complex array
  C_Complex_Type_Multistate*	P_Type;
		

  //Methods
public:
  
  // Constructor
  C_Complex_Array(C_Application*, char*, C_System*, long,
		  C_Complex_Type_Multistate*, C_Integer_Pair*, Bool, long,
		  const char*);
  
  // Destructor
  virtual
  ~C_Complex_Array(void);
  
  // Initialises complex array
  virtual Bool
  Init(void) = 0;
  
  // Retrieves a pointer to a complex at specified address in complex array
  inline C_Complex_Multistate*
  Get_Complex(long nX, long nY)
  {return P_P_Complexes[nX][nY];}
  
  // Retrieves a pointer to a complex at specified address in complex array
  C_Complex_Multistate*
  Get_Complex(C_Integer_Pair*);
  
  // Retrieves the address of a complex, specified by its pointer
  Bool
  Get_Address(C_Integer_Pair*, C_Complex_Multistate*);
  
  // Inserts a complex into the complex array at the specified address
  Bool
  Insert_Complex(C_Integer_Pair*, C_Complex_Multistate*);

  // Retrieves the dimensions of the complex array
  void
  Get_Dimensions(C_Integer_Pair*);

  // Retrieves a pointer to the complex type contained in this complex array
  C_Complex_Type_Multistate*
  Get_Type(void)
  {return P_Type;}

  // Retrieves the name of this complex array
  char*
  Get_Name(void)
  {return m_lpszName;}

  // Equilibrates any rapid equilibria of the complexes in array
  void
  Equilibrate(void);

  // Initialises variables from file for this complex array
  Bool
  Get_Variables_From_File(C_Output_Variable**, long* p_nNumVar,
			  long nMaxNumVar);
  
  // Indicates whether this array contains any complexes with rapid equilibria
  Bool
  Has_RapidEqm(void) {return P_Type->Has_RapidEqm();}

  // Returns the geometry for this array (used for output)
  char*
  Get_Geometry(void) {return m_lpszGeometry;}

  // Returns the specified complex's neighbour in the specified direction
  C_Complex_Multistate*
  Get_Neighbour(C_Integer_Pair*, int);

  // Gets the state of the complex array from the specified dump file at
  // the specified time
  Bool
  Get_State_From_Dump (char*, double, long**);
};


class C_Complex_Array_Line : public C_Complex_Array
{
  //Attributes
private:

protected:

  //Methods
public:
  
  // Constructor
  C_Complex_Array_Line(C_Application*, char*, C_System*, long, C_Complex_Type_Multistate*,
			 C_Integer_Pair*, Bool, long);
  
  // Destructor
  ~C_Complex_Array_Line(void);
  
  // Initialises complex array
  Bool
  Init(void);
};

class C_Complex_Array_Square : public C_Complex_Array
{
  //Attributes
private:

protected:

  //Methods
public:
  
  // Constructor
  C_Complex_Array_Square(C_Application*, char*, C_System*, long,
			 C_Complex_Type_Multistate*,
			 C_Integer_Pair*, Bool, long);
  
  // Destructor
  ~C_Complex_Array_Square(void);
  
  // Initialises complex array
  Bool
  Init(void);
};


class C_Complex_Array_Trigonal : public C_Complex_Array
{
  //Attributes
private:

protected:

  //Methods
public:
  
  // Constructor
  C_Complex_Array_Trigonal(C_Application*, char*, C_System*, long,
			   C_Complex_Type_Multistate*,
			 C_Integer_Pair*, Bool, long);

  // Destructor
  ~C_Complex_Array_Trigonal(void);
  
  // Initialises complex array
  Bool
  Init(void);
};


class C_Complex_Array_Hexagonal : public C_Complex_Array
{
  //Attributes
private:

protected:

  //Methods
public:
  
  // Constructor
  C_Complex_Array_Hexagonal(C_Application*, char*, C_System*, long,
			    C_Complex_Type_Multistate*,
			 C_Integer_Pair*, Bool, long);
  
  // Destructor
  ~C_Complex_Array_Hexagonal(void);
  
  // Initialises complex array
  Bool
  Init(void);
};


// Class used to store the state of complex arrays

class C_Output_Variable_Array : public C_Output_Variable
{
  //Attributes
private:

  // Output file name of this variable
  char			m_lpszFileName[MAX_FILE_NAME_LENGTH];

protected:

  //Pointer to array of values that are stored in this variable
  long**                P_P_nValue;
  
  // Pointer to Array this variable represents
  C_Complex_Array*	m_pArray;

  // Pointer to Dimension values
  C_Integer_Pair*	m_pDimensions;

  //Methods
public:
		
  //Constructor
  C_Output_Variable_Array(C_Application*, char*, C_Complex_Array*);

  // Destructor
  virtual
  ~C_Output_Variable_Array(void);

  // Initialises the complex array output variable
  virtual Bool
  Init(void);

  // Gets the output file name of this variable
  char*
  Get_FileName(void)
  {return m_lpszFileName;}

  // Sets the output file name of this variable
  void
  Set_FileName(char*);

  // Initialises this variable
  virtual Bool
  Init(long*, long*, long, Bool, long, long, long) {return TRUE;}

  // Retrieves flag indicating whether this variable is averaging
  virtual Bool
  Get_IsAveraging(void) {return FALSE;}

  // Update variable values
  virtual void
  Update(void);

  // Update variable values
  virtual void
  Reset(void){}

  // Retrieve state values of all complexes from array
  long**
  Get_Values(void) {return P_P_nValue;}

  // Retrieve dimensions of the array this variable represents
  void
  Get_Dimensions(C_Integer_Pair*);

  // Retrieve pointer to the complex array that this variable represents
  C_Complex_Array*
  Get_Array(void){return m_pArray;}
};


// Class used to store the state of the complex array

class C_Output_Variable_Array_Snaps : public C_Output_Variable_Array
{
  //Attributes
private:
  
  // Array of states to highlight
  long*			P_nState;

  // A wildcard mask-value pair is used to store the states
  //  that this variable highlights
  // (these are used instead of storing an explicit list of states, because
  //  of the computational efficiency in matching states)
  
  // The wildcard mask indicates which bits define
  // which states the wildcard string applies to
  long			m_pnWildcardMask[MAX_NUM_ARRAY_SNAPS_STATES];

  // The wildcard value gives the value of the bits in the mask
  // for the states this wildcard string applies to 
  long			m_pnWildcardValue[MAX_NUM_ARRAY_SNAPS_STATES];

  // Number of values that have been sampled for averaging
  long                  m_nNumWildcards;

  // Number of values that have been sampled for averaging
  long                  m_nNumSampled;

  // String used to display the complex array eg "ARRAY1:?1000,1????"
  char*                  m_lpszDisplayString;

  //Methods
public:
		
  //Constructor
  C_Output_Variable_Array_Snaps(C_Application* p_App, char*, char*,
				C_Complex_Array*, long*, long*, long);

  // Destructor
  ~C_Output_Variable_Array_Snaps(void);

  // Initialises this variable
  Bool
  Init(void);

  // Update variable values
  void
  Update(void);

  // Update variable values
  void
  Reset(void);

  // Retrieves the number of values that have been sampled for averaging
  long
  Get_NumSampled(void) {return m_nNumSampled;}

  // Returns pointer to display string
  char*
  Display_String(void) {return m_lpszDisplayString;}
};


// Simple structure for storing complex array addresses and dimensions

struct C_Integer_Pair
{
  // Attributes
public:

  //Stores value of first integer
  long				nX;
  //Stores value of second integer
  long				nY;

  // Methods

  //Constructor
  C_Integer_Pair(void);
  
  //Constructor
  C_Integer_Pair(long, long);

  //Sets values of x and y
  void
  Set(long, long);
};

struct C_Snaps_Variable_Set
{
  // Attributes

  // Pointer to a set of snapshot variables with common store and 
  // averaging-related intervals
  C_Output_Variable_Array_Snaps* P_Var[MAX_NUM_SNAPS_VARIABLES_IN_SET];

  // The store interval for this set
  unsigned long long		    nStoreItvl;

  // Flag indicating whether or not output should be averaged
  Bool			    bAveraging;

  // Interval between sampling times for this set
  unsigned long long		    nSampleItvl;

  // Interval over which this set should be sampled
  unsigned long long		    nAveItvl;

  // The number of iterations from previous store event after which sampling
  // must start (nStoreItvl - nAveItvl)
  unsigned long long		    nSampleStartIt;

  // Iteration counter for variable storage
  unsigned long long		    nStoreItCount;

  // Iteration counter for variable sampling
  unsigned long long		    nSampleItCount;

  // Methods

  // Constructor
  C_Snaps_Variable_Set(void);
};

class C_Output_Manager_Array : public C_Output_Manager
{
  // Attributes
private:

  // Array containing all the array_variables (variables for storing the state
  // of the two-dimensional complex array) to be stored in snapshot files
  C_Output_Variable_Array_Snaps*    P_Snaps_Var[MAX_NUM_ARRAY_SNAPS_VARIABLES];

  // Array containing all the array_variables (variables for storing the state
  // of the two-dimensional complex array) to be stored in a dump file
  C_Output_Variable_Array*        P_Dump_Var[MAX_NUM_ARRAY_DUMP_VARIABLES];
  
  // The number of complex array snapshot variables in the system
  long		            m_nNumSnapsVar;

  // The number of complex array snapshot variables in the system
  long			    m_nNumDumpVar;

  // How often dump values are stored (in terms of number of iterations)
  unsigned long long	            m_nDumpInterval;

  // Iteration counter for variable dumping
  unsigned long long	            m_nDumpItCount;

  // List of sets of snapshot variables with common store and 
  // averaging-related intervals
  C_Snaps_Variable_Set      P_SnapsVarSet[MAX_NUM_SNAPS_VARIABLE_SETS];

  // The number of sets of snapshot variables with common store intervals
  long  		    m_nNumSnapsVarSets;

  // Width of columns in file
  int			    m_nColumnWidth;

  // Width of columns in dump file
  int			    m_pnDumpColumnWidth[MAX_NUM_ARRAY_DUMP_VARIABLES];

  // Floating point precision for output streams
  int			    m_nFPPrecision;

  // Methods
private:
  // Initialises the array variables (called by constructor)
  Bool
  Init_Array_Variables(void);

  // Dumps state of complex array to a file
  Bool
  Save_Dump(double);

  // Saves a snapshot of the complex array to a file
  Bool
  Save_Snaps(long, double);

public:
  // Constructor
  C_Output_Manager_Array(C_Application*, C_System*);

  // Destructor
  ~C_Output_Manager_Array(void);
  
  // Initialises the output manager
  Bool
  Init(long, double, unsigned long long);

  // Adds a newly constructed dump variable to the list
  Bool
  Add_DumpVariable(C_Output_Variable*);

  // Adds a newly constructed dump variable to the list
  Bool
  Add_SnapsVariable(C_Output_Variable*, unsigned long long, Bool, unsigned long long,
		    unsigned long long);

  // Handles any output operations for a specified iteration
  Bool
  Handle_Output(double);

  // Displays and stores the current values of all variables
  Bool
  Display_And_Store(double);
};

#endif /* __STCH_SPC_HH */
