TESTS_NAMES =				\
	empty_module			\
	empty_module_unload		\
	create_destroy_loop		\
	jumptable			\
	plt_test			\
	char_s				\
	three_load_modules		\
	static_const_char_s		\
	const_char_s			\
	simple_printf_reloc		\
	global_pointer			\
	sym_reloc			\
	mem_func_override		\
	modules_with_same_symbol_name	\
	zero_in_bss 			\
	weak_sym			\
	tls

ROOT_DIR=$(abspath $(CURDIR))

export

all: $(patsubst %,%.dir, $(TESTS_NAMES))

all-tests: $(patsubst %,%.test, $(TESTS_NAMES))

%.dir:
	mkdir -p $(DESTDIR)/$*
	echo -e "export\n%:\n\t" $(MAKE) DESTDIR=$(DESTDIR)/$* ROOT_DIR=$(ROOTDIR) K1_TOOLCHAIN_DIR=$(K1_TOOLCHAIN_DIR) SPEC_CFLAGS=\"$(SPEC_CFLAGS)\" SPEC_LDFLAGS=\"$(SPEC_LDFLAGS)\" OS=\"$(OS)\" CLUSTER_TYPE=$(CLUSTER_TYPE) ARCH=$(ARCH) MCORE=$(MCORE) BOARD=$(BOARD) EXECUTION_PLATFORM=$(EXECUTION_PLATFORM) -C $(CURDIR)/$* \$$* > $(DESTDIR)/$*/Makefile
	$(MAKE) DESTDIR=$(DESTDIR)/$* -C $* all

%.test: %.dir
	mkdir -p $(DESTDIR)
	$(MAKE) DESTDIR=$(DESTDIR)/$* -C $* test
