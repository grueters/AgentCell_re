/*************************************************************************
*
* FILENAME:	_Stchstc.hh
*
* DESCRIPTION:	Constant definitions, forward class declarations and
*		other include files for the complete project.
*
* TYPE:		Core
*
*************************************************************************/

#ifndef __STCHSTC_HH
#define __STCHSTC_HH

// StochSim version 1.4
#define STOCHSIM_VERSION_MAJOR 1
#define STOCHSIM_VERSION_MINOR 4

// Definitions controlling program compilation

// If this is defined validation is performed on all random numbers generated
#undef	__RAN_NUM_GEN_VALIDATION


// Miscellaneous definitions

// Macro definition for returning the maximum number from two numbers
#define	MIN(a,b)  ((a) > (b) ? (b) : (a))

// Macro definition for returning the minimum number from two numbers
#define	MAX(a,b)  ((a) < (b) ? (b) : (a))

// File name for application INI file
#define APP_INI_FILE_NAME  "STCHSTC.INI"

// Character to be used to separate parameters in parameter list
#define PARAMETER_LIST_SEPARATOR	"|"

// The amount of time / sec spent trying to open values output file
// (see C_Output_Variable_Store::Save_Values)
#define RETRY_TIME_VALUE_FILE_OPEN  30

// An offset added to the reaction number to create
// the reaction ID of a reverse reaction
#define REVERSE_REACTION_ID_OFFSET  65536

// Character to be used to separate path names
#ifdef _WIN32
#define FILE_PATH_SEPARATOR	"\\"
#else
#define FILE_PATH_SEPARATOR	"/"
#endif /* _WIN32 */



// Special character definitions

// Character to be used to terminate a line (ie New line character)
#define	NEW_LINE_CHAR  '\n'

// Character to be used to terminate strings (ie Null terminator)
#define	NULL_CHAR  '\0'

// Character used to signify an off bit in a binary string
#define OFF_BIT_CHAR  '0'

// Character used to signify an on bit in a binary string
#define ON_BIT_CHAR  '1'

// String to be used as a SPACE character
#define SPACE_STR  " "

// Character to be used as a TAB character
#define	TAB_CHAR  '\t'


// Definitions for Boolean logic

// Data type for Boolean variables / functions
#define	Bool  int

// Value of a variable of type Bool when it is TRUE
#define	TRUE  1

// Value of a variable of type Bool when it is FALSE
#define	FALSE  0


// Definitions controlling the format of the output files

// Minimum column width in the output file
#define MIN_COLUMN_WIDTH  12

// Title of the column storing the current time
#define TIME_COLUMN_TITLE  "Time"

// String used to separate columns in the values output file
#define VALUES_COLUMN_SEPARATOR " "


// Definitions of physical constants required for simulation

// Avagadro's constant
#define	AVOGADRO_N  6.0221367e23

// Approximate time taken to perform one simulation iteration
// relative to the duration of the bench test
#define BENCH_TEST_ITERATIONS  1000000

// Maximum floating point number that can be represented by a double
// (usually 8 bytes) - unsigned
#define MAX_DOUBLE  1e308

// Maximum integer that can be represented by a long
// (usually 4 bytes) - signed
#define	MAX_LONG  2147483647L

// Maximum integer that can be represented by a long
// (usually 4 bytes) - unsigned
#define	MAX_UNSIGNED_LONG  4294967295UL

// Maximum integer that can be represented by a long long
// (usually 8 bytes) - signed
#define	MAX_LONG_LONG  9223372036854775807LL

// Maximum integer that can be represented by a long long
// (usually 8 bytes) - unsigned
#define	MAX_UNSIGNED_LONG_LONG  18446744073709551615ULL

// Maximum allowed probability of any reaction
// (any higher and inaccuracies will occur)
#define	MAX_PROBABILITY_REACTION  0.6

// Maximum probability to be used for scaling purposes
// (must be slightly less than maximum probability above)
#define	MAX_PR_REACTION_FOR_SCALING  0.599

// Minimum accuracy for a probability 
// eg 10 means resolution of random no.s is 10 times smaller than probabilities
#define MIN_PR_ACCURACY  10

// The approximate accuracy of the random number generator
#define	RAN_NUM_RESOLUTION  MAX_LONG


// Definitions controlling the numerical precision for output purposes

// Precision for storing floating points: minimum number of digits after most significant digit
#define PREC_FP_POST_DP  3

// Precision for displaying the percentage completion of the simulation
#define PREC_PERC_COMPLETE 1

// Precision for displaying the probability of a reaction
// (eg when outputing reaction matrix)
#define	PREC_PROBABILITY 9
#define	PREC_PROBABILITY_WIDTH 1

// Precision for displaying the frequencies numbers are selected
// in the randon number generator test
#define PREC_RAN_GEN_SAMPLE_FREQ 12

// Precision for displaying which numbers are selected
// in the randon number generator test
#define PREC_RAN_GEN_SAMPLE_INDEX  7

// Precision for displaying statistics for the randon number generator test
#define PREC_RAN_GEN_SAMPLE_STATS  2

// Precision for displaying any times during simulation
#define PREC_SIMULATION_TIME  2

// Precision for displaying the time increment used in the simulation
#define PREC_TIME_INC  2
#define PREC_TIME_INC_WIDTH 4

// Precision for displaying variable values when outputing to the screen
#define PREC_VAR_VALUE  7

// Precision for displaying snapshot values
#define PREC_SNAPSHOT_TIME  4

// Definitions controlling memory assignment
// (if any of these are changed, update documentation)

// Maximum length of the name of the components
#define	MAX_COMPONENT_NAME_LENGTH  32

// Maximum length of the symbol of the components
#define MAX_COMPONENT_SYMBOL_LENGTH  8

// Maximum different types of complexes
#define	MAX_COMPLEX_TYPES 128

// Maximum number of components in system
#define	MAX_COMPONENTS  128

// Maximum number of components allowed in any complex
#define	MAX_COMPONENTS_IN_COMPLEX  64

// Maximum number of character in a file name
#define	MAX_FILE_NAME_LENGTH  1024

// Maximum length of a string used for formatting purposes
#define MAX_FORMAT_STR_LENGTH  64

// Maximum number of characters in a line in a INI file
#define MAX_INI_LINE_LENGTH  8192

// Maximum number of characters in a parameter name in a INI file
#define MAX_INI_PARAMETER_LENGTH  256

// Maximum number of characters in a section title in a INI file
#define MAX_INI_SECTION_LENGTH  64

// Maximum number of characters in a value in a INI file
#define MAX_INI_VALUE_LENGTH  512

// Maximum number of characters in a long value in a INI file
// (some values, such as wildcard lists, tend to be long)
#define MAX_INI_VALUE_LENGTH_LONG  8192

// Maximum number of complex instances in system (memory constraint)
// (each C_Complex is approx 13 bytes)
#define	MAX_LIMIT_COMPLEXES  16777216

// Maximum length of a message
#define	MAX_MESSAGE_LENGTH  2048

// Maximum number of display variables
#define MAX_NUM_OUTPUT_MANAGERS  8

// Maximum number of display variables
#define MAX_NUM_DISPLAY_VARIABLES  128

// Maximum number of dump variables
#define MAX_NUM_DUMP_VARIABLES  8192

// Maximum number of parameters that are used by events
// to specify what is changing
#define MAX_NUM_DYNAMIC_PARAMETERS  8

// Maximum number of values that can change in each event
#define MAX_NUM_DYNAMIC_VALUES  16

// Maximum number of events which can occur in the simulation
#define MAX_NUM_EVENTS  128

// Maximum number of variables to store
#define	MAX_NUM_VALUES_STORED  32768

// Maximum number of parameters in message
#define MAX_NUM_MESSAGE_PARAMETERS  8

// Maximum number of reactions in each reaction set
// for a given set of substrates
#define	MAX_NUM_REACTIONS_IN_SET  64 //32

// Maximum number of possible complex types a fuzzy complex can represent
#define MAX_NUM_TYPE_IN_FUZZY  16

// Maximum length of name of an output variable
#define MAX_OUTPUT_VAR_NAME_LENGTH  64

// Maximum number of products produced by a reaction
#define	MAX_PRODUCTS_IN_REACTION  4

// Maximum length of character array used to store various names and messages
//increased from 128 to 4048 to allow for long paths in file names
#define	MAX_STRING_LENGTH  4048 

// Maximum number of complex types an output variable can represent
#define MAX_TYPES_IN_OUTPUT_VAR  16

// Maximum length of a line for reading from a dump file
#define MAX_INPUT_FROM_DUMP_LINE_LENGTH  1048576


// Definitions of parameter and section names for application INI file 
// (for details of each parameter, see INI file)

#define INI_ABORT_ON_RES_ERR		"AbortOnResolutionErr"
#define INI_COMPLEX_INI_FILE		"ComplexINIFile"
#define	INI_CREATE_DUMP_FILE		"CreateDumpFile"
#define INI_DISPLAY_ALL_LEVELS		"DisplayAllLevels"
#define INI_DISPLAY_INTERVAL		"DisplayInterval"
#define INI_DUMP_OUT			"DumpOut"
#define INI_INITIAL_LEVELS_FROM_DUMP_FILE "InitialLevelsFromDumpFile"
#define INI_INITIAL_LEVELS_FILE	        "InitialLevelsFile"
#define INI_INITIAL_LEVELS_FROM_TIME    "InitialLevelsFromTime"
#define INI_DYNAMIC_FILE		"DynamicValues"
#define INI_FILE_NAME_SECTION		"File Names"
#define INI_INPUT_SEED_FILE		"SeedsIn"
#define INI_LOG_FILE			"LogFile"
#define INI_MAX_NUM_COMPLEXES		"MaximumNumberComplexes"
#define	INI_MESSAGE_INI_FILE		"MessageINIFile"
#define INI_OPTIMISE_TIME_INC		"OptimiseTimeIncrement"
#define INI_OPTIONS_SECTION		"Options"
#define INI_OUTPUT_SEED_FILE		"SeedsOut"
#define INI_RAN_NUM_GEN			"RandomNumberGenerator"
#define	INI_REACTION_INI_FILE		"ReactionINIFile"
#define INI_REACTION_VOLUME		"ReactionVolume"
#define INI_R_M_OUT			"ReactionMatrixOut"
#define INI_SIMULATION_DURATION		"SimulationDuration"
#define INI_SIMULATION_SECTION		"Simulation Parameters"
#define INI_STORE_INTERVAL		"StoreInterval"
#define INI_DUMP_INTERVAL		"DumpInterval"
#define INI_TIME_INCREMENT		"TimeIncrement"
#define INI_TIME_UNITS			"TimeUnits"
#define INI_VALUES_OUT			"VariablesOut"
#define INI_USE_SPATIAL_EXTENSIONS      "UseSpatialExtensions"
// defines for averaging of variables
#define INI_AVERAGED_OUTPUT		"AveragedOutput"
#define INI_SAMPLE_INTERVAL		"SampleInterval"
// For binary output
#define INI_BINARY_STORE		"StoreInBinary"
#define BINARY_HEADER_TEXT_LENGTH	512
#define BINARY_COLUMN_TITLE_LENGTH	64
// Same as INI_SAMPLE_INTERVAL (kept for backwards compatibility)
#define INI_SAMPLE_FREQ			"SampleFreq"

// Same as INI_STORE_INTERVAL (kept for backwards compatibility)
#define INI_STORE_FREQ		        "StoreFrequency" 
// Same as INI_DISPLAY_INTERVAL (kept for backwards compatibility)
#define INI_DISPLAY_FREQ		"DisplayFrequency"

// Definitions of parameter and section names for complex INI file (for details of each parameter, see INI file)

#define INI_GENERAL_SECTION		"General"
#define INI_COMPONENT_SECTION		"Component "
#define INI_COMPLEX_TYPE_SECTION	"Complex Types"
#define INI_SPECIAL_TYPES_SECTION	"Special Complex Types"
#define INI_INITIAL_LEVELS_SECTION	"Initial Levels"
#define INI_DISPLAY_VAR_SECTION		"Display Variable "
#define INI_COMPLEX_SEPARATOR		","
#define INI_COMPONENT_NAME		"Name"
#define INI_DISPLAY_VAR_NAME		"Name"
#define INI_DISPLAY_VAR_TYPES		"Types"
#define INI_NUM_DISPLAY_VAR		"NumDisplayVariables"
#define INI_NUM_SETS			"NumberOfSets"
#define INI_SET_OF_COMPLEX_TYPES	"Set"
#define INI_SPECIAL_TYPES_FILE		"INIFile"
#define INI_SPECIAL_TYPES		"SpecialTypes"
#define INI_SYMBOLS			"Symbols"
#define INI_VAR_TYPES_SEPARATOR         ","

/*** THESE WERE USED BY FUZZY COMPLEX TYPES, WHICH ARE NOW OBSOLETE ***
#define INI_RAPID_EQM_SECTION		"Rapid Equilibrium "
#define	INI_NUM_RAPID_EQM		"NumRapidEqm"
#define INI_FUZZY_POSSIBILITIES		"PossibleTypes"
*/


// Definitions of parameter and section names for reaction INI file
// (for details of each parameter, see INI file)

#define INI_REACT_GENERAL_SECTION	"General"
#define INI_REACTION_SECTION		"Reaction "
#define INI_K_FORWARD			"kf"
#define INI_K_REVERSE			"kr"
#define INI_NUM_OF_REACTIONS		"NumberOfReactions"
#define INI_PRODUCTS			"Products"
#define INI_RATE_DYN_CODE_SEPARATOR	"@"
#define INI_REACTION_SEPARATOR		","
#define INI_SUBSTRATES			"Substrates"


// Definitions of parameter and section names for dynamic object INI file
// (for details of each parameter, see INI file)

#define INI_DYN_GENERAL_SECTION		"General"
#define INI_DYN_TIME_SECTION		"Time "
#define INI_DYN_NUM_SETS		"NumberOfSets"
#define INI_DYN_SET_OF_TIMES		"Set"
#define INI_DYN_TIME_SEPARATOR		","
#define INI_DYN_VALUE_SEPARATOR		","


// Definitions of parameter and section names for message INI file
// (for details of each parameter, see INI file)

#define	INI_MESSAGES_SECTION		"Messages"
#define INI_MESSAGE_PARAMETER		"[?]"


// Built-in messages (normally only displayed if there was an
//                    error displaying another error message)
#define MSG_CANNOT_FIND_MESSAGE_INI "Cannot locate the message INI file '%s' !"
#define MSG_CANNOT_FIND_SYSTEM_INI  "Cannot locate the system INI file '%s' !"
#define MSG_ERROR_APPLICATION_INIT "An error has occurred while initialising the application.\n"
#define MSG_ERROR_CODE_PREFIX	    " (Error "
#define MSG_ERROR_CODE_SUFFIX	    ")"
#define MSG_ERROR_RETRIEVING_MSG    "Cannot read from message file!"
#define MSG_LOG_FILE_ERROR	    "There was an error writing to the Log File!"
#define MSG_TOO_MANY_PARAMETERS	    "Too many parameters in parameter list for message!"
#define MSG_WRONG_NUMBER_PARAMETERS "Wrong number of parameters specified for message!"
#define MSG_ERROR_TOO_LONG          "A message generated exceeded the maximum length allowed!"


// Definitions for message types

#define MSG_TYPE_ERROR   0
#define MSG_TYPE_STATUS  1
#define MSG_TYPE_NOLOG   2


// Definitions for output control of random number generator test
// (see C_Random_Generator::Test)

#define RG_TEST_NO_OUTPUT	0
#define RG_TEST_STATISTICS	1
#define RG_TEST_DISTRIBUTION	2

// Definitions for output manager types
#define OM_COMPLEX              0

// Definitions for wrapper-application compatibility
// (e.g. AgentCell / REPAST)
#define MAX_NUM_RUN_IDS         128

// Forward declarations for classes

class C_Component;
class C_Complex_Type;
class C_Complex;
class C_Dynamic_Object;
class C_Event;
class C_Event_Manager;
class C_Random_Generator;
class C_Random_Generator_Shuffle;
class C_Random_Generator_Bit_String;
class C_Random_Generator_Internal;
class C_Random_Generator_Quick;
class C_Random_Generator_Dummy;
class C_Complex_Stack;
class C_Output_Variable;
class C_Output_Variable_Complex;
class C_Output_Variable_Store;
class C_System;
class C_Application;
/*** OBSOLETE
class C_Complex_Type_Fuzzy;
*/


// Global variable extern declarations (see 'main' for complete declaration)

// If we're building on a multithreaded platform
#ifdef _WINDOWS
// Declare the application pointer local to this thread (Microsoft extension)
_declspec(thread)
#endif

// Global pointer to application object
extern C_Application* pApp;

// Include libraries and other user-defined include files

// Include file used for file stream handling (eg fstream classes)
#ifdef _STD_CPP

#include	<iosfwd>
#include	<iostream>
#include	<fstream>
#include	<strstream>
#include	<sstream>
using namespace std;
// Include file for manipulating input and output streams (eg setw)
#include	<iomanip>

#else

#include	<fstream.h>
// Include file for manipulating input and output streams (eg setw)
#include	<iomanip.h>
// Include file for manipulating string streams
#ifdef _UNIX
#include	<strstream.h>
#endif /* _UNIX */
#ifdef _WINDOWS
#include	<strstrea.h>
#endif /* _WINDOWS */

#endif /* _STD_CPP */


// Include file for mathematical functions (eg div)
#include	<math.h>

// Include file for string formatting functions (eg sprintf)
#include	<stdio.h>

// Include file for standard functions (eg rand)
#include	<stdlib.h>

// Include file for string related functions (eg strlen)
#include	<string.h>

// Include file for time related functions (eg clock)
#include	<time.h>

// User include file containing class definitions
#include	"_Stch_cl.hh"

// User include file containing random generator class definitions
#include	"_Stch_clr.hh"

// User include file containing special complex type class definitions
#include	"_Stch_sct.hh"

// User include file containing spatial structure class definitions 
#include	"_Stch_spc.hh"


// Global function declarations

// Convert a long integer into a bit string
char*
LongToBitString(long, char*, const int);

// Convert a bit string into a long integer
long
BitStringToLong(C_Application*, char*);

// Calculates two to the power of a number
long
PowerOfTwo(int);

// Safely opens files 
Bool
Safe_Open(C_Application*, ofstream*, const char*, int, const char*);


// If we are debugging using Microsoft extensions...
#ifdef _MSDEBUG
#define _CRTDBG_MAP_ALLOC
// Include support for advanced memory checking
#include <crtdbg.h>
// Define macro to output memory structure with command
// eg MEMCHECK("In simulation")
#define	MEMCHECK(a)	                                      \
{					                      \
	_CrtMemState crtCheck;		                      \
	_RPT0(_CRT_WARN, "\n\n"a"\n");	                      \
	_RPT0(_CRT_WARN, "--------------------------------"); \
	_RPT0(_CRT_WARN, "\n");				      \
	_CrtMemCheckpoint(&crtCheck);			      \
	_CrtMemDumpStatistics(&crtCheck);		      \
	_RPT0(_CRT_WARN, "\n");				      \
}
#endif  /* _MSDEBUG */ 

#endif  /* __STCHSTC_HH */
