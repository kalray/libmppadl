include ../Makefile.common

module_A_mdo-srcs := module_A.mdo.c
module_A_mdo-deps := $(GENERATED)/module_B_hex.h
module_A_mdo-bin := module_A_mdo
module_A_mdo-cflags := $(MAIN_CFLAGS)

module_B_mdo-srcs := module_B.mdo.c
module_B_mdo-bin := module_B_mdo

main-srcs := main.c
main-deps := $(GENERATED)/module_A_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := module_A_mdo module_B_mdo

include ../Makefile.footer

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,module_A))
$(eval $(call mdo_rules,module_B))
$(eval $(call test_rule))

