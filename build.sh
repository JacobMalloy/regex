#!/usr/bin/env bash
CC="clang"
C_FLAGS="-O0 -g -P"
INCLUDE_FLAGS="-Iinclude"

FLAGS="${INCLUDE_FLAGS} ${C_FLAGS}"


rm -rf objs
mkdir objs

for i in src/*.c; do
    $CC $i -o "objs/$(basename $i .c).o" -c $FLAGS &
done

wait

$CC objs/*.o -o regex $FLAGS

./regex
