conan remove *cpr/1.2.0*  --force 
cd ..
conan export local/testing
cd .conan
conan install --build missing
rm -rf build
mkdir build
cd build
cmake ../
cmake --build .
./bin/cpr_demo
