# Create build directory if it doesn't exist
mkdir -p build
cd build

# Run CMake to configure and build
cmake ..
cmake --build .

./slinger
