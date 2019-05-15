system-name := mbr

# build.rb defines the following variables: O, CFLAGS, INSTALL_LIBDIR, INSTALL_INCLUDEDIR, DOXYGEN_DIR, DOC_PREFIX

MPPADL_SRCS = 				\
	src/mppa_dl.c 			\
	src/mppa_dl_load.c 		\
	src/mppa_dl_sym.c 		\
	src/mppa_dl_error.c 		\
	src/mppa_dl_debug.c		\
	src/mppa_dl_mem.c

## Use target dependent implementation
MPPADL_SRCS += src/arch/$(ARCH).c

use-module := strict-flags

mppadl-srcs := $(MPPADL_SRCS)
mppadl-cflags := -O2 -Iinclude $(CFLAGS)
mppadl-lflags += $(LDFLAGS)
mppadl-name := mppadl

somppadl-srcs := src/empty.c
somppadl-cflags := -O2 -fPIC -shared $(CFLAGS)
somppadl-lflags := -shared -nostdlib -nostartfiles $(LDFLAGS)
somppadl-name := libsomppadl.so

cluster-lib := mppadl
cluster-bin := somppadl # trick: use -bin rule to build a dynamic, shared library for io/cluster targets
# this will create a .so in bin/, a post-build-hooks rule will move it to lib/ 

post-build-hooks := mv-so-from-bin-to-lib

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

mv-so-from-bin-to-lib:
	mv $(O)/bin/libsomppadl.so $(O)/lib/cluster

install:
	cp $(O)/lib/cluster/lib*mppadl.{a,so} $(INSTALL_LIBDIR)
	cp include/*.h $(INSTALL_INCLUDEDIR)
	cp include/arch/*.h $(INSTALL_INCLUDEDIR)/arch

doc-install:
	mkdir -p $(DOC_PREFIX)/share/doc/libmppadl
	cp -r $(O)/doc/html $(DOC_PREFIX)/share/doc/libmppadl/
	install $(O)/doc/latex/libmppadl.pdf $(DOC_PREFIX)/share/doc/libmppadl/

doc:
	$(DOXYGEN_DIR)/bin/doxygen Doxyfile
	[ ! -s $(O)/doxygen_errors.log ] || echo "Missing documentation, see $(O)/doxygen_errors.log"
	[ ! -s $(O)/doxygen_errors.log ] || exit 1
	make -f Makefile.doc

.PHONY: doc
