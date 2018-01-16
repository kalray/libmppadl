include ../Makefile.common

module_A_mdo-srcs := module_A.mdo.c $(GENERATED)/module_B_hex.c
module_A_mdo-bin := module_A_mdo

module_B_mdo-srcs := module_B.mdo.c
module_B_mdo-bin := module_B_mdo

main-srcs := main.c $(GENERATED)/module_A_hex.c
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := module_A_mdo module_B_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,module_A))
$(eval $(call mdo_rules,module_B))
$(eval $(call test_rule))

