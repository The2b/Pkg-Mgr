bin_PROGRAMS = pkg-mgr
sysconf_DATA = pkg-mgr.conf

.PHONY: pkg-dirs

# This is to counteract the fact that I disabled the global defeault CXXFLAGS so I could compile tests without optimization
AM_CXXFLAGS = -O2 -g

pkg_mgr_SOURCES = backend/Config.cpp backend/Options.cpp backend/Pkg.cpp cli/Starter.cpp

pkg_mgr_CPPFLAGS = -I$(top_srcdir)/src/include

pkg_mgr_LDADD =

if WITH_ARCHIVE
pkg_mgr_LDADD += -larchive
endif # WITH_ARCHIVE

if WITH_STDCXXFS
pkg_mgr_LDADD += -lstdc++fs
endif

# We also need to add our package cache dir and installed pkg dir
pkg_dirs:
	$(MKDIR_P) $(DEFAULT_SYSTEM_ROOT)
	$(MKDIR_P) $(DEFAULT_SYSTEM_ROOT)/$(DEFAULT_TAR_LIBRARY_PATH)
	$(MKDIR_P) $(DEFAULT_SYSTEM_ROOT)/$(DEFAULT_INSTALLED_PKG_PATH)
