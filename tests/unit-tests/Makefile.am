# This makefile has two explicit targets
# The first, and probably more important of them is the test harness run on the executable which was built
# These are stored in the folder "executable-tests"
#
# The latter is the unit-tests, which are the ones in their own folder. This will likely change eventually, however
# The unit tests are not automated

check_PROGRAMS = testConfig/tstConfig testPkg/tstPkg testOptions/tstOptions
TESTS = $(check_PROGRAMS)

include $(top_srcdir)/defaultDefinitions.am

LOG_COMPILER = $(top_srcdir)/tests/unit-tests/binary-wrapper.sh 

AM_CPPFLAGS = -g -O0 -I$(top_srcdir)/src/include -I$(top_srcdir)/src/include/tests $(DEFAULT_DEFINITIONS)
AM_CXXFLAGS = 

testConfig_tstConfig_SOURCES = testConfig/tstConfig.cpp $(top_srcdir)/src/backend/Config.cpp
testConfig_tstConfig_CPPFLAGS = $(AM_CPPFLAGS) -lcppunit 
testConfig_tstConfig_CXXFLAGS =
testConfig_tstConfig_LDADD = -lcppunit

testPkg_tstPkg_SOURCES = testPkg/tstPkg.cpp $(top_srcdir)/src/backend/Pkg.cpp
testPkg_tstPkg_CPPFLAGS = $(AM_CPPFLAGS) -lcppunit -larchive -lstdc++fs
testPkg_tstPkg_CXXFLAGS =
testPkg_tstPkg_LDADD = -lcppunit -larchive -lstdc++fs

testOptions_tstOptions_SOURCES = testOptions/tstOptions.cpp $(top_srcdir)/src/backend/Options.cpp
testOptions_tstOptions_CPPFLAGS = $(AM_CPPFLAGS) -lcppunit -lstdc++fs
testOptions_tstOptions_CXXFLAGS =
testOptions_tstOptions_LDADD = -lcppunit -lstdc++fs
