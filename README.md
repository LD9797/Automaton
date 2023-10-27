# Automaton
 Deterministic Finite State Automaton Interface

A release binary can be found in the "Release" directory.

How to compile (using CMake 3.22 or higher):

Requirements:
```
CMake 3.22 or higher
libgtk-3-dev
Make
```

1. Navigate to the project directory. 

2. Install CMake, Make, and libgtk-3-dev: 

```
sudo apt-get update
sudo apt-get install cmake make libgtk-3-dev -y
```
3. Compilation steps:
```
mkdir build
cd build
cmake ..
make
```
4. Execute
```
./Automaton
```

If the project is opened using CLion, it will automatically read the CMakeLists.txt and set up everything to compile 
by just pressing Shift+F10.


