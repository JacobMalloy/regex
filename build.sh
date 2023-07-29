#!/usr/bin/env bash
CC="clang"
C_FLAGS="-O0 -g -P"
INCLUDE_FLAGS="-Iinclude"

FLAGS="${INCLUDE_FLAGS} ${C_FLAGS}"


rm -rf objs
mkdir objs

rm -rf libs
mkdir libs


rm -rf compiled_tests
mkdir compiled_tests

for i in src/*.c; do
    $CC $i -o "objs/$(basename $i .c).o" -c $FLAGS &
done

wait


$CC objs/*.o -o libs/regex.so -shared $FLAGS &
ar -crs libs/regex.a objs/*.o

for i in tests/*.c; do
    $CC $i objs/*.o -o compiled_tests/$(basename $i .c) $FLAGS &
done

wait

#./compiled_tests/main
./compiled_tests/test
