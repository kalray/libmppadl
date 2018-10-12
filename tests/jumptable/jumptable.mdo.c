/* This is copied from gcc testsuite:
   gcc/testsuite/gcc.dg/pr28574.c
   and modified to test jumptables.

   This file is part of GCC.
   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.
   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

typedef enum yasm_module_type {
    YASM_MODULE_ARCH = 0,
    YASM_MODULE_DBGFMT,
    YASM_MODULE_OBJFMT,
    YASM_MODULE_LISTFMT,
    YASM_MODULE_OPTIMIZER
} yasm_module_type;

struct yasm_module {
    const char *name;
};

typedef struct yasm_module yasm_arch_module;
typedef struct yasm_module yasm_dbgfmt_module;
typedef struct yasm_module yasm_objfmt_module;
typedef struct yasm_module yasm_listfmt_module;
typedef struct yasm_module yasm_optimizer_module;

typedef struct module {
    void *data;
} module;

int
main_dl_jumptable(yasm_module_type type,
	     void (*printfunc) (const char *name, ...))
{
    int i;
    int ret = -1;

    for (i=0; i<2; i++) {
        switch (type) {
            case YASM_MODULE_ARCH:
                printfunc("%s %d %d/2\n", "first case", __LINE__, i+1);
		ret = (type+1)<<2;
                break;
            case YASM_MODULE_DBGFMT:
                printfunc("%s %d %d/2\n", "second case", __LINE__, i+1);
		ret = (type+1)<<3;
                break;
            case YASM_MODULE_OBJFMT:
                printfunc("%s %d %d/2\n", "third case", __LINE__, i+1);
		ret = (type+1)<<4;
                break;
            case YASM_MODULE_LISTFMT:
                printfunc("%s %d %d/2\n", "fourth case", __LINE__, i+1);
		ret = (type+1)<<5;
                break;
            case YASM_MODULE_OPTIMIZER:
                printfunc("%s %d %d/2\n", "fifth case", __LINE__, i+1);
		ret = (type+1)<<6;
		break;
        }
    }
    return ret;
}
