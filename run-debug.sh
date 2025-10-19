./build-debug.sh
cd build

if [[ "$OSTYPE" == "darwin"* ]]; then
  ./PathTracer
else 
  ASAN_OPTIONS=detect_leaks=1 LSAN_OPTIONS=suppressions=../asan-suppressions.txt ./PathTracer
fi
