./build-debug.sh
cd build
ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=../asan-suppressions.txt ./PathTracer
