/*************************************************************************
*
* FILENAME:	_Stch_sct.HH
*
* DESCRIPTION:	Class definitions for all special complex types.
*
* TYPE:		Core
*
*************************************************************************/


#ifndef __STCH_SCT_HH
#define __STCH_SCT_HH


// Special character definitions

#define FLAG_SET_INDICATOR		"+"
#define FLAG_CLEAR_INDICATOR		"-"
#define FLAG_CT_SEPARATOR		":"


// Definitions identifying the special complex types being used

#define _SPECIAL_COMPLEX_TYPE_1_NAME	"Multistate_Complex"
#define _SPECIAL_COMPLEX_TYPE_1_CLASS	C_Complex_Type_Multistate

/*
  // C_Complex_Type_Neighbour_Sensitive is not being initialised as a
  // Special Complex Type!! (See Init_Complex_Types_Neighbour_Sensitive) 
  // This purpose for this was bypassing the Windows GUI, but now that
  // we don't really support it, perhaps these defs should be used in the
  // future.
#define _SPECIAL_COMPLEX_TYPE_2_NAME	"Neighbour_Sensitive_Complex"
#define _SPECIAL_COMPLEX_TYPE_2_CLASS	C_Complex_Type_Neighbour_Sensitive
*/

// Definitions for default values when specific states are not specified

// Default concentration for a given state when none is specified
#define MS_DEFAULT_CONC					0

// Default relative rate for a given state when none is specified
#define MS_DEFAULT_RATE					0

// Default rapid eqm probability for a given state when none is specified
#define MS_DEFAULT_RE_PR_SET			0


// Definitions controlling parameters used in dynamic variables
// for changes in rapid equilibria probabilities
// (Must be the last parameter in the list, as its use is optional)
#define MS_EVENT_PARAM_FLAG		2
#define MS_EVENT_PARAM_WILDCARD_MASK	1
#define MS_EVENT_PARAM_WILDCARD_VALUE	0

// Definitions controlling memory assignment 
// (if any of these are changed, update the StochSim GUI Help file 
//  - topic stochsim_limits)

// Maximum number of characters in each state flag in a multistate type
#define	MAX_MS_FLAG_LENGTH			8

// Maximum number of state flags allowed in a multistate type
#define	MAX_MS_NUM_FLAGS			31

// Maximum number of reactions the multistate complexes can undergo

// TEMPORARILY CHANGING THIS DEFINITION TO SEE IF WE CAN SPEED THINGS UP
// AT THE COST OF SOME MEMORY...
#define MAX_MS_NUM_REACTIONS			128

/*
#define MAX_MS_NUM_REACTIONS                    ((MAX_NUM_REACTIONS_IN_SET    \
                                                  * (MAX_COMPLEX_TYPES)^2)    \
                                                 + REVERSE_REACTION_ID_OFFSET)
*/

// Definitions for Neighbour-Sensitive Complexes
#define NS_MAX_NUM_NEIGHBOURS		 6
#define MAX_NS_COUPLED_STATES		 16
#define MAX_NS_NUM_TYPES		 16

#define INI_NS_TYPES_FILE		 "_NS_INIFile"
#define INI_NS_FILE_PREFIX		 "NS_"
#define INI_NS_FILE_SUFFIX		 ".INI"
#define INI_NS_TYPES_SECTION		 "Neighbour Sensitive Complex Types"
#define INI_NS_GENERAL_SECTION		 "General"
#define INI_NS_NUM_RAPID_EQM		 "NumRapidEqm"
#define INI_NS_NUM_NEIGHBOURS		 "NumNeighbours"
#define INI_NS_RAPID_EQM_SECTION	 "Rapid Equilibrium "
#define INI_NS_COUPLED_STATES		 "CoupledStates"
#define INI_NS_COUPLED_STATES_SEPARATOR	 ","
#define INI_NS_COMPLEXES		 "NeighbourSensitiveComplexes"
#define INI_NS_SEPARATOR		 ","
#define INI_NS_REACTION_EFFECTS		 "EffectOnNeighbour"
#define INI_NS_REACTION_NEIGHBOUR	 "ReactNeighbourID"

// Definitions of parameter and section names for multistate complex INI file
// (for details of each parameter, see INI file)

#define INI_EFFECT_CLEAR		"-"
#define	INI_EFFECT_SET			"+"
#define INI_EFFECTS_SEPARATOR		","
#define	INI_FLAG_SEPARATOR		","
#define INI_FORWARDS_REACTION		"F"
#define INI_INITIAL_STATES_SECTION	"Initial States"
#define INI_MS_DISPLAY_VAR_NAME		"Name"
#define INI_MS_DISPLAY_VAR_SECTION	"Display Variable "
#define INI_MS_DISPLAY_VAR_STATES	"States"
#define INI_MS_GENERAL_SECTION		"General"
#define INI_MS_NUM_DISPLAY_VAR		"NumDisplayVariables"
#define INI_MS_NUM_RAPID_EQM		"NumRapidEqm"
#define INI_MS_RAPID_EQM_SECTION	"Rapid Equilibrium "
#define INI_MS_REACTION_SEPARATOR	","
#define	INI_PR_DYN_CODE_SEPARATOR	"@"
#define	INI_RE_DYN_CODE_SEPARATOR	"@"
#define INI_RPD_EQM_SEPARATOR		","
#define INI_RPD_EQM_FLAG		"Flag"
#define INI_REACTION_EFFECTS		"Effect"
#define INI_REACTION_IDS		"Reactions"
#define	INI_REVERSE_REACTION		"R"
#define	INI_STATE_FLAGS			"StateFlags"
#define INI_VAR_STATES_SEPARATOR	","
#define INI_WILDCARD_CHAR		'?'
#define INI_WILDCARD_SEPARATOR		","
#define INI_WILDCARD_STRINGS		"Wildcards"

// Kept for backwards compatibility (replaced by "Flag")
#define INI_RPD_EQM_STATE		"State"

// Forward declarations for classes

class  C_Output_Variable_Multistate;
class  C_Complex_Type_Special;
class  C_Complex_Type_Multistate;
class  C_Complex_Type_Multistate_Reaction;
class  C_Complex_Multistate;
struct C_Integer_Pair;
class  C_Complex_Neighbour_Sensitive;
class  C_Complex_Type_Neighbour_Sensitive_Reaction;


// Abstract base class for all special complex types

class C_Complex_Type_Special : public C_Complex_Type
{

  // Attributes
private:
		
  // Methods
public:
  
  // Constructor used to set up C_Complex_Type_Special
  C_Complex_Type_Special(C_Application*, C_Complex_Type*, C_System*);
  
  // Must be overriden. Otherwise, ancestor will pull C_Complex ptrs off stack,
  // which will not able to represent special CTs.
  virtual Bool
  Create(C_Reaction*) = 0;

  // Must be overriden. Otherwise, ancestor will push the C_Complex-derived
  // ptr on stack which is stack of free C_Complex only
  virtual Bool
  Destroy(C_Complex*) = 0;

};


// Class used to store the states represented by each output variable
// (each variable refers to a C_Output_Variable in C_System)

class C_Output_Variable_Multistate : public C_Output_Variable_Complex
{

  // Attributes
protected:
  
  // These attributes were changed to protected to allow access to them
  //  by the averaging descendant class

  long*	m_pState;  // Pointer to array of states this variable represents
  
  long	m_nNumStates;  // Number of states this variable represents

  // Methods
public:
  
  // Constructs output variable with states and base class information
  C_Output_Variable_Multistate(C_Application*, char*, C_Complex_Type**,
			       long, long*, long);

  // Destroys output variable
  virtual
  ~C_Output_Variable_Multistate(void);

  // Calculates and returns the value of this variable
  virtual long
  Get_Value(void);

};


// Descendant class of C_Output_Variable_Multistate with output averaging

class C_Output_Variable_Multistate_Averaged
  : public C_Output_Variable_Multistate
{

  // Attributes
private:
  
  long	m_nCumValue;  // Pointer to array of states this variable represents

  long	m_nNumSampled;  // Number of states this variable represents

  // Methods
public:

  // Constructs output variable with states and base class information
  C_Output_Variable_Multistate_Averaged(C_Application*, char*,
					C_Complex_Type**, long, long*,
					long);

  // Virtual destructor to ensure proper cleanup of derived classes
  virtual
  ~C_Output_Variable_Multistate_Averaged(void){}
  
  // Calculates and returns the averaged value of this variable
  virtual long
  Get_Value(void);
  
  // Updates the current cumulative value for this variable
  void
  Update(void);
  
  // Resets the current cumulative value for this variable
  void
  Reset(void) {m_nCumValue=0; m_nNumSampled=0;}

};


// Class for all multistate complex types

class C_Complex_Type_Multistate
  : public C_Complex_Type_Special, public C_Dynamic_Object
{
  // Attributes
private:
  // Array of probabilities that a flag is set - valid for rpd eqm flags only
  double*		m_pPrFlagSet[MAX_MS_NUM_FLAGS];

  // Pointer to number of complexes in each state;
  // this is only calculated at certain times
  long*			m_pStateLevel;

  // Name of INI file containing information for this multistate complex
  char			m_lpszINIFile[MAX_FILE_NAME_LENGTH];

protected:
  
  // Array of values indicating if a particular flag is set by a rpd eqm
  Bool			m_bIsRpdEqm[MAX_MS_NUM_FLAGS];

  // Array of strings describing the state flags
  char			m_lpszFlag[MAX_MS_NUM_FLAGS][MAX_MS_FLAG_LENGTH + 1];

  // Number of state flags this complex type has
  int			m_nNumFlags;
  
  // Number of distinct states this complex type can be in
  int			m_nNumStates;
  
  // Number of reactions involving this complex type
  long			m_nNumReactions;

  // Array of pointers to the reactions involving this complex type
  C_Complex_Type_Multistate_Reaction*  P_Reactions[MAX_MS_NUM_REACTIONS];
	

  // Flag indicating whether this multistate complex contains rapid equilibria
  Bool  m_bHasRapidEqm;

  // Methods
private:
  
  // Extracts probabilities of rapid equilibrium from a string from INI file
  virtual Bool
  Extract_Rapid_Eqm_Probability(char*, long, double*, char*);

public:
  
  // Constructor used to set up C_Complex_Type_Multistate
  C_Complex_Type_Multistate(C_Application*, C_Complex_Type*, C_System*);

  // Destroys object
  virtual
  ~C_Complex_Type_Multistate(void);
  
  // Performs initialisation of special complex type
  virtual Bool
  Init(C_Complex**, long);

  // Create new complex of this type
  virtual C_Complex*
  Create_New_Complex(long);

  // Get probability for a particular reaction and multistate complex state
  virtual double
  Get_Relative_Probability(C_Reaction*, long);

  // Initialises variables for this complex type
  virtual Bool
  Get_Variables(C_Output_Variable**, long*, long);
  
  // Initialises variables from file for this complex type
  virtual Bool
  Get_Variables_From_File(C_Output_Variable**, long*, long, Bool);

  // Increments level by one; the state of the Complex is recorded
  virtual void
  Inc_Level(C_Complex*);
  
  // Resets current level of complex types to 0
  // (it must set the levels of all states to zero)
  virtual void
  Reset_Level(void);
  
  // Extracts a mask from a bit string containing wildcards
  Bool
  Extract_Wildcard_Mask(char*, long*, long*);
  
  // Creates display string for a specific state
  void
  Get_State_Display_String(long, char*);
  
  // Gets bit strings indicating which bits should be set/clred for reaction
  void
  Get_Flag_Modifiers(C_Reaction*, long*, long*);
  
  // Returns the level of this complex type in a particular state
  long
  Get_State_Level(long nState) {return m_pStateLevel[nState];}

  // Returns the number of flags this complex type has
  int
  Get_Num_Flags(void) {return m_nNumFlags;}

  // Returns true if a flag represents a rapid equilibrium
  Bool
  Get_Is_Rpd_Eqm(int nFlag) {return m_bIsRpdEqm[nFlag];}

  // Returns the probability a flag should be set, for rapid equilibria
  double
  Get_Pr_Flag_Set(int nFlag, long nState) {return m_pPrFlagSet[nFlag][nState];}

  // Validates the dynamic value during initialisation
  virtual Bool
  Validate_Dynamic_Values(int, double*);
  
  // Executes an event, modifying this dynamic object during the simulation
  virtual void
  Exec_Event(double*, long*);

  // Creates new special complex types during a simulation
  virtual Bool
  Create(C_Reaction*);

  // Destroys special complex types during a simulation
  virtual Bool
  Destroy(C_Complex*);

  // Returns total probability of all reactions in set modified accordingly
  virtual double
  Calc_Effective_Total_Pr(C_Reaction**, int);

  // Returns a pointer to the Multistate Reaction identified by its ID.
  C_Complex_Type_Multistate_Reaction*
  Get_Reaction_From_ID(long);

// If this is a build with averaging enabled, define method to...
#ifdef _DYNAMIC_UPDATE
  // Decrement level by one 
  virtual void
  Dec_Level(C_Complex*);
#endif /* _DYNAMIC_UPDATE */

  // Flag indicating whether this multistate complex contains rapid equilibria
  Bool
  Has_RapidEqm(void){return m_bHasRapidEqm;}
};




// Class representing a reaction the multistate complex undergoes

class C_Complex_Type_Multistate_Reaction : public C_Dynamic_Object
{
  // Attributes
protected:

  // Pointer to parent application object
  C_Application* m_pApp;
  
  // Maximum state the multistate CT, which this reaction refers to, can be in 
  long	m_nMaxState;
  
  // Reaction identifier
  long	m_nID;
  
  // Pointer to array of probabilities
  double* m_pPrArray;
  
  // Mask representing the state flags to set when this reactions occurs
  long	m_nFlagsToSet;
  
  // Mask representing the state flags to clear when this reaction occurs
  long	m_nFlagsToClear;
  // Methods
public:
  
  // Constructor used to set up C_Complex_Type_Multistate_Reaction
  C_Complex_Type_Multistate_Reaction(C_Application*, long);
  
  // Destroys object
  virtual
  ~C_Complex_Type_Multistate_Reaction(void);
  
  // Initialises the probabilities for object
  virtual Bool
  Init(char*, char*, int, char lpszFlag[][MAX_MS_FLAG_LENGTH + 1],
       C_Complex_Type_Multistate*, C_Event_Manager*);
  
  // Get the probability for a particular multistate complex state
  double
  Get_Relative_Probability(long nState) {return m_pPrArray[nState];}

  // Get the ID for this reaction
  long
  Get_Reaction_ID() {return m_nID;}
  
  // Get the maximum state the multistate CT,
  // which this reaction refers to, can be in
  long
  Get_Max_State() {return m_nMaxState;}
  
  // Gets the bit strings indicating which bits should be set and cleared
  void
  Get_Flag_Modifiers(long*, long*);
  
  // Validates the dynamic value during initialisation
  virtual Bool
  Validate_Dynamic_Values(int, double*);
  
  // Executes an event, modifying this dynamic object during the simulation
  virtual void
  Exec_Event(double*, long*);
};



// Class representing a complex representing a multistate complex type

class C_Complex_Multistate: public C_Complex
{

  // Attributes
protected:

  // State of the complex (bit string representing the individual state flags)
  long	m_nState;

  // Methods
private:
  
  // Get any modification to the probability of a reaction
  virtual double
  Get_Relative_Probability(C_Reaction*);
  
public:
  
  // Constructor used to initialise object
  C_Complex_Multistate(C_Application*, C_System*, long);
  
  // Set the state of this complex
  void
  Set_State(long nState) {m_nState = nState;}

  // Equilibrates all rapid equilibria prior to a reaction
  virtual void
  Equilibrate();
  
  // Get the state of this complex
  long
  Get_State(void) {return m_nState;}
  
  // Updates the state of the complex according to the specified reaction
  virtual void
  Update(C_Reaction* P_Reaction);

};


class C_Complex_Type_Neighbour_Sensitive : public C_Complex_Type_Multistate
{
  // Attributes
private:
  
  char	     m_lpszINIFile[MAX_FILE_NAME_LENGTH];
  Bool	     m_bIsNSRpdEqm[MAX_MS_NUM_FLAGS];
  double**   m_pNSPrFlagSet[MAX_MS_NUM_FLAGS];
  long	     m_nRENumCoupledStates[MAX_MS_NUM_FLAGS];
  long	     m_nRECoupledStateWCMask[MAX_MS_NUM_FLAGS][MAX_NS_COUPLED_STATES];
  long	     m_nRECoupledStateWCValue[MAX_MS_NUM_FLAGS][MAX_NS_COUPLED_STATES];
  long       m_nNumNSReactions;
  long       m_nNumNeighbours;
  Bool       m_bIsNSReaction[MAX_MS_NUM_REACTIONS];

  // Methods
private:
  
  // Extracts probabilities of rapid equilibrium from a string from INI file
  virtual Bool
  Extract_Rapid_Eqm_Probability(char*, long, double*);

public:
  
  //Constructor
  C_Complex_Type_Neighbour_Sensitive(C_Application*, C_Complex_Type*,
				     C_System*, long);

  //Destroys object
  virtual
  ~C_Complex_Type_Neighbour_Sensitive(void);

  //Performs initialisation
  Bool
  Init(C_Complex**, long);

  //Create new complex of this type
  C_Complex*
  Create_New_Complex(long);

  // Returns true if a flag represents a neighbour-sensitive rapid equilibrium
  Bool
  Get_Is_NSRpd_Eqm(int nFlag) {return m_bIsNSRpdEqm[nFlag];}

  // Returns true if the reaction is neighbour-sensitive
  Bool
  Get_Is_NS_Reaction(C_Reaction*);

  // Returns true if the specified state is a coupled state
  // for the rapid equilibrium of the specified flag.
  Bool
  Is_Coupled_State(long nFlag, long nState);

  // Returns the rapid-equilibrium probability for the specified flag,
  // given the state of this complex, and the number of neighbours
  // in a coupled state
  double
  Get_Pr_Flag_Set(long nFlag, long nState, long nCouplingValue)
  {return m_pNSPrFlagSet[nFlag][nState][nCouplingValue];}
  
  // Returns the relative probability for a reaction
  // (used during simulation)
  virtual double
  Get_Relative_Probability(C_Reaction*, long, C_Complex_Neighbour_Sensitive**);

  // Returns the relative probability for a reaction
  // (used during optimisation at initialisation time)
  virtual double
  Get_Relative_Probability(C_Reaction*, long, long);
  
  // Retrieves information about which flags of a reacting neighbour need
  // to  be modified after an NS reaction
  void
  Get_Flag_Modifiers(C_Reaction*, long*, long*);
  
  // Returns the maximum total probability of reaction 
  // (used when optimising reaction matrix during initialisaton)
  double
  Calc_Effective_Total_Pr(C_Reaction**, int);
  
  // Returns a pointer to the Neighbour-Sensitive Reaction identified by its ID
  C_Complex_Type_Neighbour_Sensitive_Reaction*
  Get_NS_Reaction_From_ID(long);

  long
  Get_Num_Neighbours(void)
  {return m_nNumNeighbours;}

#ifdef _NS_R_DEBUG  // For debugging neighbour-sensitive reactions
  // Get the count of how many times an NS-reaction has been executed
  long
  Get_NS_R_Count(long nReaction);

  // Increment the NS-reaction counter
  virtual void
  Inc_Count(C_Reaction*);
#endif /* _NSDEBUG */
};


// Class representing a complex representing a Neighbour_Sensitive complex type

class C_Complex_Neighbour_Sensitive: public C_Complex_Multistate
{

  // Attributes
private:
  C_Complex_Neighbour_Sensitive** P_Neighbour;
  Bool				  m_bIsArrayMember;
  long                            m_nNumNeighbours;
  long                            m_nDefaultCouplingValue;

#ifdef _CA_DEBUG  // For debugging complex arrays
public:
  C_Integer_Pair*                 m_pAddress;
#endif /* _CA_DEBUG */

  // Methods
public:
  
  // Constructor used to initialise object
  C_Complex_Neighbour_Sensitive(C_Application*, C_System*, long, long);

  // Destructor
  ~C_Complex_Neighbour_Sensitive(void);

  // Sets flag indicating that this complex is in a complex array
  void
  Set_IsArrayMember(void) {m_bIsArrayMember = TRUE;}
  
  // Retrieves flag indicating whether this complex is in a complex array
  Bool
  Get_IsArrayMember(void) {return m_bIsArrayMember;}
  
  // Equilibrates all rapid equilibria prior to a reaction
  virtual void
  Equilibrate();

  // Set up the pointer list for the neighbours of this complex
  void
  Set_Neighbours(C_Complex_Multistate**, long);

  // Get the NS_Relative probability for a specified reaction
  virtual double
  Get_Relative_Probability(C_Reaction*);
  
  // Update the state of this complex and its neighbour according
  // to the specified reaction
  void
  Update(C_Reaction*);
  
  // Update the state of this complex (called by neighbouring complex)
  void
  Update(long, long);
};


// Class representing a (non-Rapid-Equilibrium) reaction
// that the neighbour-sensitive complex undergoes 

class C_Complex_Type_Neighbour_Sensitive_Reaction
  : public C_Complex_Type_Multistate_Reaction
{
	  
  // Attributes
private:
  // Pointer to array to store reaction probability for each state
  double*                       m_pNSPrArray;
  // Specifies which neighbour this reaction is sensitive to
  long                          m_nNeighbourID;
  // Mask representing the neighbour's state flags
  // to set when this reactions occurs
  long	                        m_nNeighbourFlagsToSet;
  // Mask representing the state flags to clear when this reaction occurs
  long	                        m_nNeighbourFlagsToClear;

#ifdef _NS_R_DEBUG  // For debugging neighbour-sensitive reactions
  // Counts how many times this reaction has occured when debugging
  long                          m_nCount;
public:
  // Resets the counter
  void
  Reset_Count(void) {m_nCount = 0;}
  // Returns value of the counter
  long
  Get_Count(void) {return m_nCount;}
  // Returns value of the counter
  void
  Inc_Count(void) {m_nCount ++;}
private:
#endif /* NS_R_DEBUG */
  
  // Methods
public:
  
  // Constructor used to set up C_Complex_Type_Neighbour_Sensitive_Reaction
  C_Complex_Type_Neighbour_Sensitive_Reaction(C_Application*, long);
  
  // Constructor used to create a CT_NS_R based on pre-initialised CT_MS_R
  C_Complex_Type_Neighbour_Sensitive_Reaction(C_Application*,
					      C_Complex_Type_Multistate_Reaction*);
  
  // Destroys object
  virtual
  ~C_Complex_Type_Neighbour_Sensitive_Reaction(void);
  
  // Initialises the probabilities for object
  virtual Bool
  Init(char*, char*, int, char lpszFlag[][MAX_MS_FLAG_LENGTH + 1],
       C_Complex_Type_Neighbour_Sensitive*, C_Event_Manager*, long);

  // Get the probability for a particular neighbour state
  double
  Get_Relative_Probability(long nNeighbourState)
  {return m_pNSPrArray[nNeighbourState];} 
 
  // Executes an event, modifying this dynamic object during the simulation
  virtual void
  Exec_Event(double*, long*);
  
  // Gets the bit strings indicating which bits should be set and cleared
  void
  Get_Flag_Modifiers(long*, long*);

  // Gets the ID of the neighbour that this reaction is sensitive to.
  long
  Get_Neighbour_ID() {return m_nNeighbourID;} 
};


#endif /* __STCH_SCT_HH */
