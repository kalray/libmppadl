include ../Makefile.common

weak_sym_mdo-srcs := weak_sym.mdo.c
weak_sym_mdo-bin := weak_sym.mdo

main-srcs := main.c
main-deps := $(GENERATED)/weak_sym_hex.h

main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS) -Wl,--defsym=this_is_weak_bound_to_main_from_cli=0,--defsym=this_is_weak_bound_to_main_from_cli_nonull=0xcafebebe

cluster-or-io-bin := main
cluster-or-io-dynbin := weak_sym_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,weak_sym))
$(eval $(call test_rule))
