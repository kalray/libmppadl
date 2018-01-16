include ../Makefile.common

inter_module_call_noplt_mdo-srcs := inter_module_call_noplt.mdo.c
inter_module_call_noplt_mdo-bin := inter_module_call_noplt.mdo

other_module_mdo-srcs := other_module.mdo.c
other_module_mdo-bin := other_module.mdo

main-srcs := main.c $(GENERATED)/inter_module_call_noplt_hex.c $(GENERATED)/other_module_hex.c
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := inter_module_call_noplt_mdo other_module_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,inter_module_call_noplt))
$(eval $(call mdo_rules,other_module))
$(eval $(call test_rule))
