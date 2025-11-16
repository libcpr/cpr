@echo off
mkdir build
cd build
cmake .. -DCPR_BUILD_TESTS=OFF
cmake --build . --target distributed_example
cd ..