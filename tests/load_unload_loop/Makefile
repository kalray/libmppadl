include ../Makefile.common

load_unload_mdo-srcs := load_unload.mdo.c
load_unload_mdo-bin := load_unload.mdo

main-srcs := main.c $(GENERATED)/load_unload_hex.c
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := load_unload_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,load_unload))
$(eval $(call test_rule))
