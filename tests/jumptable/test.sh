#!/bin/bash
set -eu

build_dir=$(realpath $(dirname $1)/../build/)
echo Build dir is $build_dir

mdo_assembly=$(find $build_dir -name jumptable.mdo.c.s)
echo MDO $mdo_assembly

## we must match something !
if grep -e '[[:space:]]*pcrel \$r[0-9]* = @pcrel(\.L[0-9]*)$' $mdo_assembly; then
    echo "Found some pcrel insn that should correspond to PIC jump tables: GOOD"
    exit 0
else
    echo "Could NOT find some pcrel insn for PIC jump tables: BAD"
    exit 255
fi
