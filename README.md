# mesh-compiler
3D file converter to a customizable binary file format

I am making this one for myself - for game i am developing: https://github.com/pwalig/3D-FPS-OpenGL, but maybe somebody will find this useful.

### why...?

The idea is that you can perform all precalculations, that you would in your game when loading models, beforehand and save results into binary files to minimize reading time at the cost of filesize (or maybe at no cost depending on what file formats are you using).

## Usage
[running the program](https://github.com/pwalig/mesh-compiler/wiki/usage)  
[format file syntax](https://github.com/pwalig/mesh-compiler/wiki/format-file-syntax)  
[mesh reader library](https://github.com/pwalig/mesh-compiler/wiki/mesh-reader-library)

For pre-built binaries checkout releases: https://github.com/pwalig/mesh-compiler/releases

## Using
Mesh-compiler is using [Open Asset Import Library (assimp)](https://github.com/assimp/assimp) for data import. No need to download additional dependencies, all necessery assimp files are included in this repository.
