AUTOMAKE_OPTIONS = foreign
SUBDIRS = src
man3_MANS = docs/man/man3/*

.PHONY: all check unit-tests help html-docs man-docs latex-docs docs clean

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
	@echo make html-docs - Create HTML documentation
	@echo make latex-docs - Create LaTeX documentation
	@echo make man-docs - Create man pages
	@echo make docs - Create html-docs, latex-docs, man-docs
	@echo make clean-docs - Remove built documentation
	
html-docs:
	@doxygen docs/doxy-config-html

latex-docs:
	@doxygen docs/doxy-config-latex

man-docs:
	@doxygen docs/doxy-config-man

docs: html-docs latex-docs man-docs

clean-docs:
	@rm -rfv docs/{html,man,latex}
