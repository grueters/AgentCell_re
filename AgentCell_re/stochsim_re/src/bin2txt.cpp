/*************************************************************************
*
* FILENAME:	bin2txt.cpp
*
* DESCRIPTION:	Auxiliary program for converting binary output of StochSim
*               variable values into ascii text format.  This uses the
*               binary I/O class "Binfstream.h" by Thierry Emonet.
*
* TYPE:		Aux
*
*************************************************************************/

#include <fstream>
#include <vector>
#include "_Stchstc.hh"
#include "Binfstream.hh"


int main(int argc, char** argv)
{

  const int H=BINARY_HEADER_TEXT_LENGTH;
  char headerText[H];
  int NIn,SIn;
  std::vector<double> time;
  std::vector<long>   value[MAX_NUM_DISPLAY_VARIABLES];
  int i,j, num_records;
  char* colTitle[MAX_NUM_DISPLAY_VARIABLES];
  for (i=0;i<MAX_NUM_DISPLAY_VARIABLES;i++)
    colTitle[i] = NULL;

  if (argc != 2)
    {
      cerr << "Error: Wrong number of arguments!" << endl;
      cerr << "Usage: " << argv[0] << " binary_filename" << endl;
      exit(1);
    }

  Binfstream ifs(argv[1],ios::in);
#if ( defined(_ALPHA) || defined(_AIX) )
  if (!ifs.rdbuf()->is_open())
#else
  if (!ifs.is_open())
#endif /* ( defined(_ALPHA) || defined(_AIX) ) */
    {
      cerr << "Error: Cannot open the input file '" << argv[1] << "'." << endl;
      cerr << "Usage: " << argv[0] << " binary_filename" << endl;
      exit(1);
    }

  ifs.read(headerText,H);
  ifs.read(NIn);
  ifs.read(SIn);
  for (i=0;i<MAX_NUM_DISPLAY_VARIABLES;i++)
    colTitle[i] = new char [SIn];
  for(i=0;i<NIn+1;i++)
    ifs.read(colTitle[i],SIn);
  i=0;
  while(!ifs.eof())
    {
      time.push_back(0);
      ifs.read(time[i]);
      for(j=0;j<NIn;j++)
	{
	  value[j].push_back(0);
	  ifs.read(value[j][i]);
	}
      i++;
    }
  num_records = i-1;
  ifs.close();


  //print to stdout
  cout << headerText << endl;
  cout << NIn << endl;
  cout << SIn << endl;
  cout << colTitle[0];     // Time column title
  for(i=1;i<NIn+1;i++)
    cout << "\t" << colTitle[i];
  cout << endl;
  for(i=0;i<num_records;i++)
    {
      cout << time[i];
      for(j=0;j<NIn;j++)
	cout << "\t" << value[j][i];
      cout << endl;
    }

  for (i=MAX_NUM_DISPLAY_VARIABLES-1;i>=0;i--)
    if (colTitle[i] != NULL)
      delete[] colTitle[i];
}
