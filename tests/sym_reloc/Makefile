include ../Makefile.common

sym_reloc_mdo-srcs := sym_reloc.mdo.c
sym_reloc_mdo-bin := sym_reloc.mdo

main-srcs := main.c $(GENERATED)/sym_reloc_hex.c
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := sym_reloc_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,sym_reloc))
$(eval $(call test_rule))
