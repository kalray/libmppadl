include ../Makefile.common

create_destroy_mdo-srcs := create_destroy.mdo.c
create_destroy_mdo-bin := create_destroy.mdo

main-srcs := main.c
main-deps := $(GENERATED)/create_destroy_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := create_destroy_mdo

include ../Makefile.footer

include $(KALRAY_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,create_destroy))
$(eval $(call test_rule))
