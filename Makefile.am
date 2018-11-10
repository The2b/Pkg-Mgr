AUTOMAKE_OPTIONS = foreign
SUBDIRS = src

check:
	$(MAKE) -C tests/executable-tests check

unit-tests:
	$(MAKE) -C tests/unit-tests check

.PHONY: all unit-tests
