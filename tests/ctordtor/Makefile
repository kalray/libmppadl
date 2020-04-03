include ../Makefile.common

ctor_mdo-srcs := ctor.mdo.c
ctor_mdo-bin := ctor.mdo
dtor_mdo-srcs := dtor.mdo.c
dtor_mdo-bin := dtor.mdo

main-srcs := main.c 
main-deps := $(GENERATED)/ctor_hex.h $(GENERATED)/dtor_hex.h
main-cflags := $(MAIN_CFLAGS)
main-lflags := $(MAIN_LDFLAGS)

cluster-or-io-bin := main
cluster-or-io-dynbin := ctor_mdo dtor_mdo

include ../Makefile.footer

include $(KALRAY_TOOLCHAIN_DIR)/share/make/Makefile.kalray

$(eval $(call mdo_rules,ctor))
$(eval $(call mdo_rules,dtor))
$(eval $(call test_rule))
