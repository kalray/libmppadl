include ../Makefile.common

create_destroy_mdo-srcs := create_destroy.mdo.c
create_destroy_mdo-bin := create_destroy.mdo

main-srcs := main.c $(GENERATED)/create_destroy_hex.c
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := create_destroy_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,create_destroy))
$(eval $(call test_rule))
