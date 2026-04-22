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
$CC $FLAGS -I "include" -c -o ".build/allocator.page.o" "src/allocator.page.c"
$CC $FLAGS -I "include" -c -o ".build/allocator.linear.o" "src/allocator.linear.c"
$ARCHIVE crs $LIBSTOTEM ".build/allocator.o" ".build/allocator.fixed_buffer.o" ".build/allocator.page.o" ".build/allocator.linear.o"

# Examples
$CC $FLAGS -I "include" -o ".build/examples/allocator.fixed_buffer" "examples/fixed_buffer.c" $LIBSTOTEM
$CC $FLAGS -I "include" -o ".build/examples/allocator.page" "examples/page.c" $LIBSTOTEM
$CC $FLAGS -I "include" -o ".build/examples/allocator.linear" "examples/linear.c" $LIBSTOTEM
