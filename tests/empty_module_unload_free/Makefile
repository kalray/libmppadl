include ../Makefile.common

empty_module_unload_free_mdo-srcs := empty_module_unload_free.mdo.c
empty_module_unload_free_mdo-bin := empty_module_unload_free.mdo

main-srcs := main.c $(GENERATED)/empty_module_unload_free_hex.c
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := empty_module_unload_free_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,empty_module_unload_free))
$(eval $(call test_rule))
