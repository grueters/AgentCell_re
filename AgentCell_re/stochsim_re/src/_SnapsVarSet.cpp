/*************************************************************************
*
* FILENAME:	SnapsVarSet.cpp
*
* STRUCT:	C_Snaps_Variable_Set
*
* DESCRIPTION:	Simple structure for sets of snapshot variables with common
*               storage and display related intervals.
*
*************************************************************************/

#include "_Stchstc.hh"


/*************************************************************************
*
* METHOD NAME:	Constructor
*
*************************************************************************/

C_Snaps_Variable_Set::C_Snaps_Variable_Set(void)
{
  long i;
  nStoreItvl = 0;
  bAveraging = 0;
  nSampleItvl = 0;
  nAveItvl = 0;
  nSampleStartIt = 0;
  nStoreItCount = 0;
  nSampleItCount = 0;

  for (i = 0; i < MAX_NUM_SNAPS_VARIABLES_IN_SET; i ++)
    P_Var[i] = NULL;
}

