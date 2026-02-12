# build program @host 
mkdir x86_64-build && cd x86_64-build

- host default: use system openssl
cmake ../cpp/server \
    -DUSE_GSOAP=ON -DCMAKE_PREFIX_PATH="$HOME/work/aarch64_env/01_dist/x86_64-gsoap-2.8.140"

# cpp/server build with feature test
 - ruby (defaut, 3.3.8)
 - cucumber (7.1.0)
 - cucumber-wire (6.2.1)
cmake .. -DUSE_GSOAP=ON \
    -DCMAKE_PREFIX_PATH="$HOME/work/aarch64_env/01_dist/x86_64-gsoap-2.8.140;$HOME/work/aarch64_env/01_dist/x86_64-ccpp/lib/cmake" \
    -DCMAKE_BUILD_TYPE=Debug -DENABLE_CUCUMBER_TEST=1
