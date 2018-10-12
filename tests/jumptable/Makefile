include ../Makefile.common

jumptable_mdo-srcs := jumptable.mdo.c
jumptable_mdo-bin := jumptable.mdo

main-srcs := main.c
main-deps := $(GENERATED)/jumptable_hex.h

main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := jumptable_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,jumptable))
$(eval $(call test_rule))
