include ../Makefile.common

global_pointer_mdo-srcs := global_pointer.mdo.c
global_pointer_mdo-bin := global_pointer.mdo

main-srcs := main.c
main-deps := $(GENERATED)/global_pointer_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := global_pointer_mdo

include ../Makefile.footer

include $(KALRAY_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,global_pointer))
$(eval $(call test_rule))
