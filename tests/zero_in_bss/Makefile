include ../Makefile.common

zero_in_bss_mdo-srcs := zero_in_bss.mdo.c
zero_in_bss_mdo-bin := zero_in_bss.mdo

main-srcs := main.c
main-deps := $(GENERATED)/zero_in_bss_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := zero_in_bss_mdo

include ../Makefile.footer

include $(KALRAY_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,zero_in_bss))
$(eval $(call test_rule))
