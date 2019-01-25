AUTOMAKE_OPTIONS = foreign
SUBDIRS = src

check:
	$(MAKE) -C tests/executable-tests check

unit-tests:
	$(MAKE) -C tests/unit-tests check

help:
	@echo
	@echo make - Build the executable pkg-mgr
	@echo make install - Install pkg-mgr
	@echo make uninstall - Uninstall pkg-mgr
	@echo make clean - Removes files created during compilation. Retains configuration status.
	@echo make dist-clean - Remvoes files created during configuration and compilation.
	@echo make check - Run integration tests on a built executable
	@echo make unit-tests - Build and run unit tests
	@echo


.PHONY: all unit-tests
