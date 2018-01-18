include ../Makefile.common

empty_module_mdo-srcs := empty_module.mdo.c
empty_module_mdo-bin := empty_module.mdo

main-srcs := main.c
main-deps := $(GENERATED)/empty_module_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := empty_module_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,empty_module))
$(eval $(call test_rule))
