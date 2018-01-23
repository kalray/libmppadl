include ../Makefile.common

plt_test_mdo-srcs := plt_test.mdo.c
plt_test_mdo-bin := plt_test.mdo

main-srcs := main.c
main-deps := $(GENERATED)/plt_test_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := plt_test_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,plt_test))
$(eval $(call test_rule))
