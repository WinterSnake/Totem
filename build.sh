# -Build Variable
CC="gcc"
ARCHIVE="ar"
FLAGS="-Wall -Wextra -std=c11"

# -Build Dir
mkdir --parents ".build/examples"

# -Library
LIBSTOTEM=".build/libstotem.a"
$CC $FLAGS -I "include" -c -o ".build/allocator.o" "src/allocator.c"
$CC $FLAGS -I "include" -c -o ".build/allocator.fixed_buffer.o" "src/allocator.fixed_buffer.c"
$ARCHIVE crs $LIBSTOTEM ".build/allocator.o" ".build/allocator.fixed_buffer.o"

# Examples
$CC $FLAGS -I "include" -o ".build/examples/allocator.fixed_buffer" "examples/fixed_buffer.c" $LIBSTOTEM
