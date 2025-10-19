mkdir build-release
cd build-release

cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS_DEBUG="-g -O3" \
      -DCMAKE_CXX_FLAGS_DEBUG="-g -O3" \
      ..
cmake --build .
