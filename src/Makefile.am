bin_PROGRAMS = pkg-mgr
globalconf_DATA = pkg-mgr.conf

# We also need to add our package cache dir and installed pkg dir
install-data-local:
systemrootdir =	$(DESTDIR)/$(defaultSystemRoot)
tarlibdir = $(DESTDIR)/$(defaultTarLibPath)
installedpkgdir = $(DESTDIR)/$(defaultInstalledPkgPath)
globalconfdir = $(defaultGlobalConfigPath)

# This is to counteract the fact that I disabled the global defeault CXXFLAGS so I could compile tests without optimization
AM_CXXFLAGS = -O2 -g

pkg_mgr_SOURCES = backend/Config.cpp backend/Options.cpp backend/Pkg.cpp cli/Starter.cpp

pkg_mgr_CPPFLAGS = -I$(top_srcdir)/src/include -DDEFAULT_VERBOSITY='$(defaultVerbosity)' -DDEFAULT_SMART_OP='$(defaultSmartOp)' -DDEFAULT_TAR_LIBRARY_PATH='"$(defaultTarLibPath)"' -DDEFAULT_INSTALLED_PKG_PATH='"$(defaultInstalledPkgPath)"' -DDEFAULT_GLOBAL_CONFIG_PATH='"$(defaultGlobalConfigPath)/pkg-mgr.conf"' -DDEFAULT_USER_CONFIG_PATH='"$(defaultUserConfigPath)/pkg-mgr.conf"' -DDEFAULT_SYSTEM_ROOT='"$(defaultSystemRoot)"' -DDEFAULT_EXCLUDED_FILES='$(defaultExcludedFiles)'

pkg_mgr_LDADD =

if WITH_ARCHIVE
pkg_mgr_LDADD += -larchive
endif # WITH_ARCHIVE

if WITH_STDCXXFS
pkg_mgr_LDADD += -lstdc++fs
endif # WITH_STDCXXFS
