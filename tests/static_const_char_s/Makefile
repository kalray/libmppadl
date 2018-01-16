include ../Makefile.common

static_const_char_s_mdo-srcs := static_const_char_s.mdo.c
static_const_char_s_mdo-bin := static_const_char_s.mdo

main-srcs := main.c $(GENERATED)/static_const_char_s_hex.c
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := static_const_char_s_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,static_const_char_s))
$(eval $(call test_rule))
