# Installation Instructions

To build and run the CPR Distributed Library, you need to install the following dependencies:

## 1. Install CMake

CMake is a build system generator that is used to build the CPR library. You can download it from the official website:

[https://cmake.org/download/](https://cmake.org/download/)

After downloading, run the installer and follow the on-screen instructions. Make sure to add CMake to your system PATH.

## 2. Install GCC Compiler

For Windows users, you can install GCC via MinGW-w64:

[https://www.mingw-w64.org/downloads/](https://www.mingw-w64.org/downloads/)

Download the installer and follow the on-screen instructions. Make sure to select the "x86_64-posix-seh" variant for 64-bit Windows.

For Linux users, you can install GCC via your distribution's package manager:

```bash
# Debian/Ubuntu
sudo apt-get update
sudo apt-get install gcc g++

# Fedora/CentOS
sudo dnf install gcc g++
```

## 3. Install OpenSSL

The CPR library requires OpenSSL for encryption. You can download it from the official website:

[https://www.openssl.org/source/](https://www.openssl.org/source/)

For Windows users, you can also install OpenSSL via Chocolatey:

```bash
choco install openssl
```

For Linux users, you can install OpenSSL via your distribution's package manager:

```bash
# Debian/Ubuntu
sudo apt-get install libssl-dev

# Fedora/CentOS
sudo dnf install openssl-devel
```

## 4. Build the CPR Library

Once you have installed all the dependencies, you can build the CPR library using the following commands:

```bash
# Create a build directory
mkdir build
cd build

# Generate the build files
cmake .. -DCMAKE_BUILD_TYPE=Release -DCPR_BUILD_TESTS=OFF

# Build the library
cmake --build . --config Release
```

## 5. Build the Distributed Example

To build the distributed example, run the following commands:

```bash
cd build
g++ -std=c++17 -I ../include ../example/distributed_example.cpp -o distributed_example -lcpr -lssl -lcrypto -lpthread
```

## 6. Run the Distributed Example

After building the example, you can run it using the following command:

```bash
./distributed_example
```