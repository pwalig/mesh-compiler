# mesh-compiler
3D file converter to a customizable binary file format

I am making this one for myself - for game i am developing: https://github.com/pwalig/3D-FPS-OpenGL, but maybe somebody will find this useful.

## why...?

The idea is that you can perform all precalculations, that you would in your game when loading models, beforehand and save results into binary files to minimize reading time at the cost of filesize (or maybe at no cost depending on what file formats are you using).

# Installation

Get your copy from [release section](https://github.com/pwalig/mesh-compiler/releases), or build from source.

## Install Dependency

Mesh-compiler is using [Open Asset Import Library (assimp)](https://github.com/assimp/assimp) for data import. For non Visual Studio 2022 you will have to either build or install assimp library yourself. Follow instructions on [their page](https://github.com/assimp/assimp/blob/master/Build.md)

## Building with CMake

```
mkdir build
cd build
cmake ..
make
```

You might need to adjust `CMakeLists.txt` file to conform to location where you have installed the assimp library.

### Building with Visual Studio 2022

For Visual Studio 2022 builds all assimp files: include headders and pre-built libraries are included in this repository. Just open `src/mesh-compiler.sln` with Visual Studio 2022 and run.

## Usage

[running the program](https://github.com/pwalig/mesh-compiler/wiki/running-the-program)  
[format file syntax](https://github.com/pwalig/mesh-compiler/wiki/format-file-syntax)  
[mesh reader library](https://github.com/pwalig/mesh-compiler/wiki/mesh-reader-library)
