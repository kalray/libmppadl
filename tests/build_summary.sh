#!/bin/bash
set -e
set -u

READELF=k1-readelf

find -path '*output/bin/*_mdo' | while read f;
do
    echo ''
    echo $f
    $READELF --relocs $f
done | ( grep  -e '\(^.*_mdo$\)\|\(^[0-9a-f]*[[:space:]][0-9a-f]*[[:space:]].*$\)' || true ) > used_relocs_in_mdo.1
cp used_relocs_in_mdo.1 used_relocs_in_mdo
echo -e "\nSummary:\n" >> used_relocs_in_mdo
cat used_relocs_in_mdo.1 | grep -e '^[0-9a-f]\+[[:space:]][0-9a-f]*' | awk '{print $3}' | sort -u >> used_relocs_in_mdo

find -path '*mdo.c.s'| while read f; do
    echo ''
    echo $f
    echo '------------------'
    grep -4 -e '@[a-z0-9_]*(.*)' $f || true
done >  used_pseudo_func_in_mdo.1
cp used_pseudo_func_in_mdo.1 used_pseudo_func_in_mdo
echo -e "\nSummary:\n" >> used_pseudo_func_in_mdo

cat used_pseudo_func_in_mdo.1 | grep '@[a-z0-9_]*'| grep -v '@function' | sed -e 's/.*\(@[a-z0-9_]*\).*$/\1/g'| sort -u  >> used_pseudo_func_in_mdo

echo 'USED PSEUDO FUNCS IN MDO'
cat used_pseudo_func_in_mdo
echo ''
echo ''
echo 'USED RELOCS IN MDO'
cat used_relocs_in_mdo
