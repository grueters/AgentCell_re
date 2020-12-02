/*************************************************************************
 *
 * FILENAME:		CONSOLE.H
 *
 * DESCRIPTION:	Class definitions for console-specific objects
 *
 * TYPE:			Console
 *
 *************************************************************************/


#ifndef __CONSOLE_H
#define __CONSOLE_H


#include "_Stchstc.hh"


// Console-specific definitions

#define MAX_LINE_LENGTH		60		// Length of the line displayed on the screen. Used for formatting purposes



// Forward declaration for classes

class C_Console_App;




// Class definitions

class C_Console_App : public C_Application
{

  // Methods
private:
  // Outputs error message to standard output device
  virtual void	ErrOut(const char*, int nErrorCode = -1);
  // Outputs status message to standard output device
  virtual void	StatusOut(const char*);
  // Outputs any message to console
  void  	TextToConsole(const char*);
public:
  // Constructs object
  C_Console_App(const char*);
  // Constructs object (with explicit CWD setting)
  C_Console_App(const char*, const char*);
  // Destructor
  virtual
  ~C_Console_App(void){}
  // Runs simulation	  
  virtual Bool	Run(C_Application*);
  // Void version kept for compatibility
  virtual Bool	Run(void){return FALSE;};
  // Display variable values at a particular time
  virtual void	Display_Variable_Values(double, C_Output_Variable**, double, long);

};

#endif
