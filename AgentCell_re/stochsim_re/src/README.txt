TO BUILD STOCHSIM:

  ON LINUX:

    cp makefile.linux makefile
    make
    make install

  ON CYGWIN

    cp makefile.cygwin makefile
    make
    make install

TO TEST STOCHSIM FUNCTIONALITY:

  ON LINUX:
    cd ../tests/TestStochsimWrapper/src
    cp makefile.linux makefile
    make
    make clean

    cd ../../TestGetters/src
    cp makefile.linux makefile
    make
    make clean

  ON CYGWIN:

    cd ../tests/TestStochsimWrapper/src
    cp makefile.cygwin makefile
    make
    make clean

    cd ../../TestGetters/src
    cp makefile.cygwin makefile
    make
    make clean


