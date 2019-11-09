/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl.h"

mppa_dl_handle_t *head = NULL;


void *mppa_dl_load(const char *image, int flag)
{
	MPPA_DL_LOG(1, "> mppa_dl_load(%s, %d)\n", image, flag);

	size_t i;
	size_t memsz = 0, malign = 0;

	mppa_dl_handle_t *hdl = NULL;
	void *addr = NULL;

	/* ELF header */
	ElfK1_Ehdr *ehdr = (ElfK1_Ehdr *)image;
	/* Section header list */
	ElfK1_Shdr *shdr = (ElfK1_Shdr *)(image + ehdr->e_shoff);
	/* Program header list */
	ElfK1_Phdr *phdr = (ElfK1_Phdr *)(image + ehdr->e_phoff);
	/* String table (section names) */
	char *shstrtab = (char *)(image + shdr[ehdr->e_shstrndx].sh_offset);

	if (flag <= MPPA_DL_FLAG_MIN || flag >= MPPA_DL_FLAG_MAX) {
		mppa_dl_errno(E_WRONG_FLAG);
		return NULL;
	}

	/* handle list is empty and _DYNAMIC symbol has been declared by the
	   main program: add an handle for it */
	if (head == NULL && _DYNAMIC != 0) {
		MPPA_DL_LOG(2, ">> main program have a .dynamic section\n");

		head = (mppa_dl_handle_t *)
			mppa_dl_malloc(sizeof(mppa_dl_handle_t));
		if (mppa_dl_init_handle(head, _DYNAMIC, NULL, NULL,
					MPPA_DL_GLOBAL) != 0) {
			mppa_dl_errno(E_INIT_HDL);
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
		    (long int)memsz, (ElfK1_Addr)addr, (long int)malign);

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
				    (ElfK1_Addr)addr + phdr[i].p_vaddr);
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
			if (mppa_dl_init_handle(
				    hdl,
				    (ElfK1_Dyn *)((ElfK1_Addr)shdr[i].sh_addr),
				    addr, head, flag) == 0) {
				head = hdl;
			} else {
				mppa_dl_errno(E_INIT_HDL);
				return NULL;
			}
			break;
		/* fill .bss section with zeroes */
		case SHT_NOBITS:
			if (strcmp(".bss", &shstrtab[shdr[i].sh_name]) == 0) {
				memset(addr + shdr[i].sh_addr, 0,
				       shdr[i].sh_size);
			}
			break;
		default:
			break;
		}
	}


	/* process relocations */

	MPPA_DL_LOG(2, ">> %ld RELPLT relocations and %ld RELA relocations\n",
		    (long int)head->pltreln, (long int)head->relan);

	for (i = 0; i < head->relan; i++) { /* DT_RELA relocations */
		if (mppa_dl_apply_rela(head, head->rela[i]) != 0) {
			mppa_dl_errno(E_RELOC);
			return NULL;
		}
	}

	for (i = 0; i < head->pltreln; i++) { /* DT_PLTREL relocations */
		if (head->pltrel == DT_RELA) {
			if (mppa_dl_apply_rela(
				    head,
				    ((ElfK1_Rela*)head->jmprel)[i]) != 0) {
				mppa_dl_errno(E_RELOC);
				return NULL;
			}
		} else {
			mppa_dl_errno(E_PLT_RELOC);
			return NULL;
		}
	}

	__builtin_k1_fence();
	__builtin_k1_iinval();

	mppa_dl_debug_update(head);

	MPPA_DL_LOG(1, "< mppa_dl_load(%s, %d)\n", image, flag);

	return (void *)head;
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

	mppa_dl_free(hdl->addr); /* free allocated ELF image memory */

	/* remove the handle from the list */

	if (hdl == head) /* removing the head of the list,
			    moving the head pointer */
		head = hdl->parent;

	/* update parent's child pointer and child's parent pointer
	   of the item to remove */
	if (hdl->parent != NULL)
		hdl->parent->child = hdl->child;
	if (hdl->child != NULL)
		hdl->child->parent = hdl->parent;

	/* if after removing the item, the list size is equal to one,
	   and _DYNAMIC symbol has been declared, unload it too */
	if (head != NULL && _DYNAMIC != 0) {
		MPPA_DL_LOG(2, ">> unload also handle for main program\n");

		if (head->parent == NULL) {
			mppa_dl_free(head->addr);
			mppa_dl_free(head);
			head = NULL;
		}
	}

	if (hdl->name)
		mppa_dl_free(hdl->name);

	mppa_dl_free(handle);

	mppa_dl_debug_update(head);

	MPPA_DL_LOG(1, "< mppa_dl_unload(%p)\n", handle);

	return ret;
}
