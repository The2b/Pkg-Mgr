# This makefile has two explicit targets
# The first, and probably more important of them is the test harness run on the executable which was built
# These are stored in the folder "executable-tests"
#
# The latter is the unit-tests, which are the ones in their own folder. This will likely change eventually, however
# The unit tests are not automated

TESTS = tstInstall.py tstUninstall.py tstFollow.py tstUnfollow.py

TEST_EXTENSIONS = .py
AM_TESTS_ENVIRONMENT = PKG_MGR_PATH='$(top_srcdir)/src/pkg-mgr' ; export PKG_MGR_PATH;
PY_LOG_COMPILER = $(PYTHON)
AM_PY_LOG_COMPILER_FLAGS = 
