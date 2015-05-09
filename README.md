PE-NGram-Analysis
=================

Integration Test (Travis CI)
+ [![Build Status](https://travis-ci.org/ZSShen/PE-NGram-Analysis.svg?branch=master)](https://travis-ci.org/ZSShen/PE-NGram-Analysis)  

##Introduction
A research project to discover the similarity between a Windows PE file and its obfuscated instances using different types of file protectors. Specifically, it transforms a givien PE file into the n-gram distribution model which  calculates the frequency of each kinds of byte token combinations. Such frequency model can be used to analysis the characteristics and distance between a pair of file binaries.  

##Installation
####***Basic***
First of all, we need to prepare the following utilities:
- [CMake] - A cross platform build system.
- [Valgrind] - An instrumentation framework help for memory debug.
- [gnuplot] - A portable command line driven graphing utility.

For Ubuntu 12.04 and above, it should be easy:
``` sh
$ sudo apt-get install -qq cmake
$ sudo apt-get install -qq valgrind
$ sudo apt-get install -qq gnuplot
```
Now we can build the entire source tree under the project root folder:
``` sh
$ ./clean.py --rebuild
$ cd build
$ cmake ..
$ make
```
Then the main engine should be under:  
- `./bin/engine/release/pe_ngram`  

And the assistent plugins should be under:
- `./bin/plugin/release/libRegion_*.so`
- `./bin/plugin/release/libModel_*.so`

####***Advanced***
If we patch the main engine or the assistent plugins, we can apply CMake to configure the building procedures resprectively with the following parameters:  
- `BUILD_TARGET`  
  + Engine - Set the building procedure for main engine only. Root output folder is `./bin/engine`.
  + Plugin - Set the buliding procedure for assistent plugins only. Root output folder is `./bin/plugin`.
- `CMAKE_BUILD_TYPE`
  + Release - For performance optimized binary. Root output folder is `./bin/.*/release`.
  + Debug - For debug information added binary. Root output folder is `./bin/.*/debug`.

For examples:
- For debug version of engine  
``` sh
$ cmake .. --DBUILD_TARGET=Engine  --DCMAKE_BUILD_TYPE=Debug
$ make
```
- For release version of plugins
``` sh
$ cmake .. --DBUILD_TARGET=Plugin  --DCMAKE_BUILD_TYPE=Release
$ make
```
Note that if we do not specifiy the `BUILD_TARGET` parameter, both the building procedures for engine and plugins are prepared. And if we do not specify the `CMAKE_BUILD_TYPE` parameter, the release version for optimized binaries will be generated.

##Usage


##Reference
+ The project is inspired by the research paper from G.Jacob et. al.
  [A Static, Packer-Agnostic Filter to Detect Similar Malware Samples]
  (https://www.cs.ucsb.edu/~vigna/publications/2012_DIMVA_packedmalware.pdf)

[CMake]:http://www.cmake.org/
[Valgrind]:http://valgrind.org/
[gnuplot]:http://www.gnuplot.info/
