path_to_bin="build/bin"
executable="./$path_to_bin/riscv_32-vm"
test="$path_to_bin/$1"
if [ -e $test ]; then
    $executable $test
else
    echo "FILE DOES NOT EXIST"
fi