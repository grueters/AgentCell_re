###################################################### 
# Options to build using
# Standard C++ Library (e.g. RedHat Linux 8.0 and later)
######################################################

### Commands
SHELL = /bin/sh
CP = cp
RM = rm -f

### Test Name
TEST_NAME = TestStochsimWrapper

### Directories for the test
ROOTDIR = $(PWD)
DATADIR = $(ROOTDIR)/../data
TEST_LOG = $(ROOTDIR)/$(TEST_NAME).OUT

### Location of Stochsim includes, library and executable
LIBDIR = $(ROOTDIR)/../../../lib/
SRCDIR = $(ROOTDIR)/../../../src/
BINDIR = $(ROOTDIR)/../../../bin/
STOCHSIM_LIB = stochsim

### Compiler
CXX = g++
OPTS = -D_STD_CPP -D_UNIX -D_DYNAMIC_UPDATE
CXXFLAGS = -O3 -Wno-deprecated -Wall -fPIC 
#CXXFLAGS = -g -Wno-deprecated -Wall -fPIC 


### Running the Test
all: $(TEST_NAME)
	-mkdir $(DATADIR)/cell1/Output $(DATADIR)/cell2/Output $(DATADIR)/cell3/Output $(DATADIR)/cell4/Output
	export LD_LIBRARY_PATH=$(LIBDIR); $(ROOTDIR)/$(TEST_NAME) $(DATADIR)/cell1/Input $(DATADIR)/cell2/Input; \
	cd $(DATADIR)/cell3/Input; $(BINDIR)/stochsim; \
	cd $(DATADIR)/cell4/Input; $(BINDIR)/stochsim; \
	echo " " > $(TEST_LOG); \
	echo "TEST RESULTS" >> $(TEST_LOG); \
	echo " " >> $(TEST_LOG); \
	echo "We are comparing results from stochsim standalone runs" >> $(TEST_LOG); \
	echo "with runs done using the Wrapper. The diff between the two" >> $(TEST_LOG); \
	echo "outputs are listed between the two lines of stars below." >> $(TEST_LOG); \
	echo "The test is passed if there are no differences." >> $(TEST_LOG); \
	echo " " >> $(TEST_LOG); \
	echo "************************************************" >> $(TEST_LOG); \
	diff -r --exclude="LOG.OUT" --exclude="CVS" $(DATADIR)/cell1/Output $(DATADIR)/cell3/Output >> $(TEST_LOG); \
	echo " " >> $(TEST_LOG); \
	diff -r --exclude="LOG.OUT" --exclude="CVS" $(DATADIR)/cell2/Output $(DATADIR)/cell4/Output >> $(TEST_LOG); \
	echo "************************************************" >> $(TEST_LOG); \
	cat $(TEST_LOG)


### Compiling the test
$(TEST_NAME):  
	$(CXX) $(CXXFLAGS) $(OPTS) -I$(SRCDIR) -o $(TEST_NAME) $(TEST_NAME).cpp -L$(LIBDIR) -l$(STOCHSIM_LIB)


### Cleaning 
.PHONY: clean
clean:
	rm -f $(DATADIR)/cell?/Output/*.OUT *.o $(TEST_NAME) $(TEST_LOG)
