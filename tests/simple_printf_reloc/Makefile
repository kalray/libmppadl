include ../Makefile.common

simple_printf_reloc_mdo-srcs := simple_printf_reloc.mdo.c
simple_printf_reloc_mdo-bin := simple_printf_reloc.mdo

main-srcs := main.c
main-deps := $(GENERATED)/simple_printf_reloc_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := simple_printf_reloc_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,simple_printf_reloc))
$(eval $(call test_rule))
