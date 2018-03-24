# CLArray
Abstraction of memory access for arrays in GPUs for optimal kernel performance.

CLArray uses the Google Test and CLTune libraries.

### Google Test Setup on Mac
1. Download the gtest library from https://github.com/google/googletest/archive/release-1.8.0.zip
2. Run the following commands to build the library:
```
cd googletest-release-1.8.0/googletest/
mkdir build
cd build
cmake ..
make
cp -r ../include/gtest /usr/local/include
cp -r lib*.a /usr/local/lib
```
3. Delete the folder googletest-release-1.8.0 and release-1.8.0.zip

### CLTune Setup on Mac
1. Install the repo https://github.com/CNugteren/CLTune
2. Go into the root of the CLTune folder and run:
```
mkdir build
cd build
cmake ..
make
sudo make install
```