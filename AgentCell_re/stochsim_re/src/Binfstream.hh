/**
   CLASS:       Binfstream
   
   DESCRIPTION: Extends the class fstream to automatically handle reading 
                and writing to binary files.
                We only changed the read() and write() methods.
		Their arguments is either:
		     a number (any type), or 
                     an array of numbers and the first dimension of the array

   EXAMPLE:     N=30;     
                char xheader[N]="This is my header";
                int a1d[N];
                long int a2d[N][N];
		float x1d[N];
		double x2d[N][N];

		Binfstream to("file.dat",ios::out);
                to.write(xheader,K);
		to.write(N);
                to.write(a1d,N);
                to.write(x1d,N);
		to.write(a2d,N);
		to.write(x2d,N);
		to.close();

                Although mixed types in a single file can be handled,
                note that in general it is not a good idea to do so
                because then one has to remember the order of the
                write statements in order to be able to read.

   AUTHORS:     Timur Linde and Thierry Emonet, Mar 21 2003
*/

   
#include <iostream>

class Binfstream : public fstream {
public:
  // constructors
  Binfstream() : fstream() { }
#ifdef _STD_CPP
  Binfstream(const char *name, int mode) : fstream(name, (std::ios::openmode) mode) { }
#else
  Binfstream(int fd) : fstream(fd) { }
  Binfstream(const char *name, int mode) : fstream(name, mode) { }
#endif /* _STD_CPP */
  
  // 
  template <class T> void read(T& o) {
    fstream::read((char*)((void*)(&o)),sizeof(o));
  }
  template <class T> void read(T* o,int n) {
    fstream::read((char*)((void*)o),n*sizeof(o[0]));
  }
  template <class T> void write(const T& o) {
    fstream::write((char*)((void*)(&o)),sizeof(o));
  }
  template <class T> void write(const T* o,int n) {
    fstream::write((char*)((void*)o),n*sizeof(o[0]));
  }
};

 
