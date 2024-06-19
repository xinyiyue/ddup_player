install library:
```
sudo apt install cmake
sudo apt install g++
sudo apt install libsdl2-dev
sudo apt-get install libsdl2-image-dev
sudo apt-get install libsdl2-ttf-dev
```

make cmd：
```
cmake -G "Unix Makefiles" -B build  -DCMAKE_INSTALL_PREFIX=build  -DCMAKE_BUILD_TYPE=Debug
cd build
make
make install
```

