include ../Makefile.common

mem_func_override_mdo-srcs := mem_func_override.mdo.c
mem_func_override_mdo-bin := mem_func_override.mdo

main-srcs := main.c
main-deps := $(GENERATED)/mem_func_override_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := mem_func_override_mdo

include ../Makefile.footer

include $(KALRAY_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,mem_func_override))
$(eval $(call test_rule))
