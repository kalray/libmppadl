include ../Makefile.common

char_s_mdo-srcs := char_s.mdo.c
char_s_mdo-bin := char_s.mdo

main-srcs := main.c
main-deps := $(GENERATED)/char_s_hex.h

main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := char_s_mdo

include ../Makefile.footer

include $(KALRAY_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,char_s))
$(eval $(call test_rule))
