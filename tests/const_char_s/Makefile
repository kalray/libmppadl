include ../Makefile.common

const_char_s_mdo-srcs := const_char_s.mdo.c
const_char_s_mdo-bin := const_char_s.mdo

main-srcs := main.c 
main-deps := $(GENERATED)/const_char_s_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := const_char_s_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,const_char_s))
$(eval $(call test_rule))
