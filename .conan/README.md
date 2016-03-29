    pip install conan
    git clone https://github.com/DEGoodmanWilson/cpr.git cpr-conan
    cd cpr-conan
    conan export cpr/testing
    cd ..
    git clone https://github.com/DEGoodmanWilson/cpr-conan-demo.git
    cd cpr-conan-demo
    conan install --build cpr
    cd ..
    mkdir cpr-conan-demo-build && cd cpr-conan-demo-build
    cmake ../cpr-conan-demo
    cmake --build .
    ./bin/cpr_demo
