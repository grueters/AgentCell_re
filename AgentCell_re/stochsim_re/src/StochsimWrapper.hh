/*************************************************************************
*
* FILENAME:	StochsimWrapper.hh : Wraps StochSim as a library.
*                              
* CONTRIBUTED BY: Michael North (north@anl.gov)
*
* DESCRIPTION:	The purpose of this class is to facilitate interfacing with
*               other applications (such as AgentCell / REPAST) which wrap
*               multiple StochSim instances and run them in parallel.  The
*               interface (specified in StochsimWrapper.hh) defines methods for:
*
*       	* creating StochSim instances: create() method
*
* 	        * deleting StochSim instances: finalise() method
*
*	        * advancing simulation time in steps: step() method
*
*	        * retrieving the current time: getTime() method
*
*	        * retrieving the current time Increment: getTimeInc() method
*
*	        * retrieving the current amount of a named molecular species:
*                 getCopynumber() method
*
*	        * changing dynamic values during a simulation:
*                 setDynamicValue() method
*
* TYPE:		Wrapper
*
*************************************************************************/

#ifndef __StochsimWrapper_HH
#define __StochsimWrapper_HH

#include "stdio.h"
#include "_Stchstc.hh"
#include "_Stch_cl.hh"
#include "console.hh"
#include "stdlib.h"
#include <strings.h>

#ifdef _UNIX
#include <unistd.h>
#endif

class StochsimWrapper :  public C_Application
{

private:
  C_System* m_pSystem;
  
public:

  StochsimWrapper (const char *, int);

  StochsimWrapper (const char *, const char *, int);

  virtual ~StochsimWrapper (void){};

  void finalise (void);

  void step (double);

  double getTime (void);

  double getTimeInc (void);

  long getCopynumber (const char *);

  long getOVValue (const char *);

  void setDynamicValue (const char *, double);

  virtual Bool Run (void){return TRUE;}

  virtual Bool Run (C_Application *){return TRUE;}

  virtual void ErrOut(const char*, int nErrorCode = -1) {}

  virtual void StatusOut(const char*) {}

  virtual void	Display_Variable_Values(double, C_Output_Variable**, double, long) {}

};
#endif /* __StochsimWrapper_HH */

