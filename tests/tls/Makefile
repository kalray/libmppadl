include ../Makefile.common

tls_mdo-srcs := tls.mdo.c
tls_mdo-bin := tls.mdo

main-srcs := main.c 
main-deps := $(GENERATED)/tls_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := tls_mdo

include ../Makefile.footer

include $(KALRAY_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,tls))
$(eval $(call test_rule))
