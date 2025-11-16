# PowerShell script to build the entire CPR library with all modules

# Create build directory if it doesn't exist
if (-not (Test-Path -Path "build" -PathType Container)) {
    New-Item -Path "build" -ItemType Directory | Out-Null
}

# Change to build directory
Set-Location -Path "build"

# Run CMake configuration
cmake .. -DCMAKE_BUILD_TYPE=Release -DCPR_BUILD_TESTS=OFF

# Build the project
cmake --build . --config Release

# Change back to original directory
Set-Location -Path ..
