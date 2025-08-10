SUBDIRS = lib test
ACLOCAL_AMFLAGS = -I m4
pkgconfig_DATA = libhomfly.pc
dist-hook:
	cp .travis.yml $(distdir)
	cp LICENSE $(distdir)
	cp README.md $(distdir)
