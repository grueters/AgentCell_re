/*************************************************************************
*
* FILENAME:	IP.cpp
*
* CLASS:	C_Integer_Pair
*
* DESCRIPTION:	Simple structure for storing (X,Y) coordinates.
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
*************************************************************************/

C_Integer_Pair::C_Integer_Pair(void)
{
  nX = 0;		//Initialise coordinate values to 0
  nY = 0;
}


/*************************************************************************
*
* METHOD NAME:	Constructor
*
*************************************************************************/

C_Integer_Pair::C_Integer_Pair(long nXX, long nYY)
{
  nX = nXX;		//Initialise coordinate values to parameter values
  nY = nYY;
}


/*************************************************************************
*
* METHOD NAME:	Set
*
* DESCRIPTION:	This method is used to set the values in a
*		C_Integer_Pair object.
*				
*
* PARAMETERS:	long	nXX		- value of first integer in pair
*		long	nYY		- value of second integer in pair
*
*
*************************************************************************/

void
C_Integer_Pair::Set(long nXX,long nYY)
{
  nX = nXX;
  nY = nYY;
}
