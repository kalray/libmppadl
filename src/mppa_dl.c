/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl.h"
#include "priv/mppa_dl_internal.h"

mppa_dl_handle_t *mppa_dl_head_handles = NULL;


void *mppa_dl_load(const char *image, int flag)
{
	MPPA_DL_LOG(1, "> mppa_dl_load(%s, %d)\n", image, flag);

	size_t i;
	size_t memsz = 0, malign = 0;
	size_t sht_symtab_nb_syms = 0;
	int nb_tps = 0;
	const char *sht_strtab = NULL;
	ElfKVX_Sym *sht_symtab_syms = NULL;

	mppa_dl_handle_t *hdl = NULL;
	void *addr = NULL;

	/* ELF header */
	ElfKVX_Ehdr *ehdr = (ElfKVX_Ehdr *)image;
	/* Section header list */
	ElfKVX_Shdr *shdr = (ElfKVX_Shdr *)(image + ehdr->e_shoff);
	/* Program header list */
	ElfKVX_Phdr *phdr = (ElfKVX_Phdr *)(image + ehdr->e_phoff);
	/* String table (section names) */
	char *shstrtab = (char *)(image + shdr[ehdr->e_shstrndx].sh_offset);

	if (flag <= MPPA_DL_FLAG_MIN || flag >= MPPA_DL_FLAG_MAX) {
		mppa_dl_errno(E_WRONG_FLAG);
		return NULL;
	}

	/* handle list is empty and _DYNAMIC symbol has been declared by the
	   main program: add an handle for it */
	if (mppa_dl_head_handles == NULL && _DYNAMIC != 0) {
		MPPA_DL_LOG(2, ">> main program have a .dynamic section\n");

		mppa_dl_head_handles = (mppa_dl_handle_t *)
			mppa_dl_malloc(sizeof(mppa_dl_handle_t));
		enum MPPA_DL_ERRNO err = mppa_dl_init_handle(
			mppa_dl_head_handles, _DYNAMIC, NULL, NULL,
			MPPA_DL_GLOBAL);
		if (err != E_NONE) {
			mppa_dl_errno(err);
			return NULL;
		}
	}

	MPPA_DL_LOG(2, ">> ELF image contains %u program headers, "
		    "starting at offset 0x%lx\n",
		    ehdr->e_phnum, ehdr->e_phoff);
	MPPA_DL_LOG(2, ">> ELF image contains %u section headers, "
		    "starting at offset 0x%lx\n",
		    ehdr->e_shnum, ehdr->e_shoff);

	/* allocate memory to load needed ELF segments */

	/* determine memory size and alignement constraints */
	for (i = 0; i < ehdr->e_phnum; i++) {
		if (phdr[i].p_type == PT_LOAD) {
			memsz = MAX(memsz, phdr[i].p_vaddr + phdr[i].p_memsz);
			malign = MAX(malign, phdr[i].p_align);
		}

		/* Bailout if TLS data is found. We do not support any TLS at the moment. */
		if (phdr[i].p_type == PT_TLS) {
			MPPA_DL_LOG(2, ">> ELF image contains TLS data, this is not supported\n");
			mppa_dl_errno(E_UNSUP_TLS);
			return NULL;
		}
	}

	addr = mppa_dl_memalign(malign, memsz);

	MPPA_DL_LOG(2, ">> allocate %ld bytes of memory at 0x%lx, "
		    "with alignement: %ld\n",
		    (long int)memsz, (ElfKVX_Addr)addr, (long int)malign);

	if (addr == NULL) {
		mppa_dl_errno(E_MEM_ALIGN);
		return NULL;
	}

	/* iterate program headers to load PT_LOAD segments */
	for (i = 0; i < ehdr->e_phnum; i++) {
		switch (phdr[i].p_type) {
		case PT_LOAD: /* load segment to memory */
			memcpy(addr + phdr[i].p_vaddr,
			       image + phdr[i].p_offset, phdr[i].p_memsz);

			MPPA_DL_LOG(2, ">> load segment: %lu bytes at 0x%lx\n",
				    phdr[i].p_memsz,
				    (ElfKVX_Addr)addr + phdr[i].p_vaddr);
			break;
		default: /* do not load other segments */
			break;
		}
	}

	/* iterate section headers to init handle for loaded image */
	for (i = 0; i < ehdr->e_shnum; i++) {
		switch (shdr[i].sh_type) {
		case SHT_DYNAMIC:
			hdl = (mppa_dl_handle_t *)
				mppa_dl_malloc(sizeof(mppa_dl_handle_t));
			enum MPPA_DL_ERRNO err = mppa_dl_init_handle(
				hdl,
				(ElfKVX_Dyn *)((ElfKVX_Addr)shdr[i].sh_addr),
				addr, mppa_dl_head_handles, flag);
			if (err == E_NONE) {
				mppa_dl_head_handles = hdl;
			} else {
				mppa_dl_errno(err);
				return NULL;
			}
			break;
		case SHT_STRTAB:
			/* there can be several STRTAB sections but we are only
			* interested by .strtab */
			if (strcmp(".strtab", &shstrtab[shdr[i].sh_name]) == 0)
				sht_strtab = image + shdr[i].sh_offset;
			break;
		case SHT_SYMTAB:
			sht_symtab_nb_syms =
				shdr[i].sh_size / shdr[i].sh_entsize;
			sht_symtab_syms = (ElfKVX_Sym *)
				(shdr[i].sh_offset + image);
			break;
		/* fill .bss section with zeroes */
		case SHT_NOBITS:
			if (strcmp(".bss", &shstrtab[shdr[i].sh_name]) == 0) {
				memset(addr + shdr[i].sh_addr, 0,
				       shdr[i].sh_size);
			}
			break;
		case SHT_NOTE:
			if (strcmp("kvx_tp_id",
				&shstrtab[shdr[i].sh_name]) == 0) {
				nb_tps = (int) shdr[i].sh_size;
			}
			break;
		/* Rejects .ctors/.dtors sections based
		 * constructors/destructors. init_array/fini_array and
		 * DT_INIT/DT_FINI are handled in
		 * mppa_dl_init_handle */
		case SHT_PROGBITS:
			if (strcmp(".ctors", &shstrtab[shdr[i].sh_name]) == 0
			    || strcmp(".dtors", &shstrtab[shdr[i].sh_name]) == 0) {
				mppa_dl_errno(E_UNSUP_CTOR_DTOR);
				return NULL;
			}
			break;
		default:
			break;
		}
	}

	mppa_dl_head_handles->sht_strtab = sht_strtab;
	mppa_dl_head_handles->sht_symtab_syms = sht_symtab_syms;
	mppa_dl_head_handles->sht_symtab_nb_syms = sht_symtab_nb_syms;

	/* process relocations */

	MPPA_DL_LOG(2, ">> %ld RELPLT relocations and %ld RELA relocations\n",
		    (long int)mppa_dl_head_handles->pltreln,
		    (long int)mppa_dl_head_handles->relan);

	for (i = 0; i < mppa_dl_head_handles->relan; i++) { /* DT_RELA relocations */
		if (mppa_dl_apply_rela(mppa_dl_head_handles,
		    mppa_dl_head_handles->rela[i]) != 0) {
			mppa_dl_errno(E_RELOC);
			return NULL;
		}
	}

	for (i = 0; i < mppa_dl_head_handles->pltreln; i++) {
		/* DT_PLTREL relocations */
		if (mppa_dl_head_handles->pltrel == DT_RELA) {
			if (mppa_dl_apply_rela(mppa_dl_head_handles,
				((ElfKVX_Rela*)mppa_dl_head_handles->jmprel)[i])
				!= 0) {
				mppa_dl_errno(E_RELOC);
				return NULL;
			}
		} else {
			mppa_dl_errno(E_PLT_RELOC);
			return NULL;
		}
	}

	__builtin_kvx_fence();
	__builtin_kvx_iinval();
	__builtin_kvx_barrier();

	mppa_dl_trace_load(mppa_dl_head_handles, nb_tps);

	mppa_dl_debug_update(mppa_dl_head_handles);

	MPPA_DL_LOG(1, "< mppa_dl_load(%s, %d)\n", image, flag);

	return (void *)mppa_dl_head_handles;
}


void *mppa_dl_sym(void *handle, const char* symbol)
{
	MPPA_DL_LOG(1, "> mppa_dl_sym(%p, %s)\n",
		    handle, symbol);

	void *sym = mppa_dl_sym_lookup((mppa_dl_handle_t *)handle, symbol, 1);

	MPPA_DL_LOG(1, "< mppa_dl_sym(%p, %s)\n",
		    handle, symbol);
	return sym;
}


int mppa_dl_unload(void *handle)
{
	MPPA_DL_LOG(1, "> mppa_dl_unload(%p)\n", handle);

	int ret = 0;
	mppa_dl_handle_t *hdl = (mppa_dl_handle_t*)handle;

	mppa_dl_debug_set_valid(0);

	 mppa_dl_trace_unload(hdl);

	mppa_dl_free(hdl->addr); /* free allocated ELF image memory */

	/* remove the handle from the list */

	if (hdl == mppa_dl_head_handles) /* removing the head of the list,
			    moving the head pointer */
		mppa_dl_head_handles = hdl->parent;

	/* update parent's child pointer and child's parent pointer
	   of the item to remove */
	if (hdl->parent != NULL)
		hdl->parent->child = hdl->child;
	if (hdl->child != NULL)
		hdl->child->parent = hdl->parent;

	/* if after removing the item, the list size is equal to one,
	   and _DYNAMIC symbol has been declared, unload it too */
	if (mppa_dl_head_handles != NULL && _DYNAMIC != 0) {
		MPPA_DL_LOG(2, ">> unload also handle for main program\n");

		if (mppa_dl_head_handles->parent == NULL) {
			mppa_dl_free(mppa_dl_head_handles);
			mppa_dl_head_handles = NULL;
		}
	}

	if (hdl->name)
		mppa_dl_free(hdl->name);

	mppa_dl_free(handle);

	mppa_dl_debug_update(mppa_dl_head_handles);

	MPPA_DL_LOG(1, "< mppa_dl_unload(%p)\n", handle);

	return ret;
}
