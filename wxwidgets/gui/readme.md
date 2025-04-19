# Build Instructions for Example Application Using wxWidgets


## Linux

### Install wxWidgets

### Build Application

cd ~/github/cpp_experiments/wxwidgets/gui
make -f linux_Makefile
./wx_example

## Windows 11

### Install MinGW

https://github.com/msys2/msys2-installer/releases/download/2025-02-21/msys2-x86_64-20250221.exe

Run MSYS2. /c is the Windows C: drive.

pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S git
pacman -S vim
pacman -S gmake
...

### Install wxWidgets

Inside the MSYS2 shell

git clone https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets
mkdir build-static 
cd build-static
../configure --disable-shared
make
make install

### Build Application

make -f windows_Makefile
./wx_example.exe

