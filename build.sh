# -Build Variable
CC="gcc"
ARCHIVE="ar"
FLAGS="-Wall -Wextra -std=c11"

# -Build Dir
mkdir --parents ".build/examples"

# -Library
LIBSTOTEM=".build/libstotem.a"
$CC $FLAGS -I "include" -c -o ".build/allocator.o" "src/allocator.c"
$CC $FLAGS -I "include" -c -o ".build/allocator.fba.o" "src/allocator.fba.c"
$ARCHIVE crs $LIBSTOTEM ".build/allocator.o" ".build/allocator.fba.o"

# Examples
$CC $FLAGS -I "include" -o ".build/examples/allocator.fba" "examples/fba.c" $LIBSTOTEM
