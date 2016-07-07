[![Build Status](https://travis-ci.org/ZSShen/PE-NGram-Analysis.svg?branch=master)](https://travis-ci.org/ZSShen/PE-NGram-Analysis) 

# **Skyline**  

Skyline is a research project to discover the similarity between a Windows PE file and its obfuscated instances using different types of file protectors. Specifically, it transforms the file into the ***n-gram distribution model*** which calculates the frequency of n-gram tokens. Such frequency model can be used to inspect the characteristics and the potential similarity of PE files.  

## **Installation**  
#### **Basic**  
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

And the assistant plugins should be under:
- `./bin/plugin/release/libRegion_*.so`
- `./bin/plugin/release/libModel_*.so`

#### **Advanced**  
If we modify the main engine or the assistant plugins, we can apply CMake to configure the building procedures respectively with the following arguments:  
- `BUILD_TARGET`  
  + Engine - To build main engine only. Root output folder is `./bin/engine`.
  + Plugin - To build assistant plugins only. Root output folder is `./bin/plugin`.
- `CMAKE_BUILD_TYPE`
  + Release - For optimized build. Root output folder is `./bin/.*/release`.
  + Debug - For debug build. Root output folder is `./bin/.*/debug`.

For example:
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
Note that if we do not specify the `BUILD_TARGET` argument, the building procedures for the engine and plugins are both added. And if we do not specify the `CMAKE_BUILD_TYPE` argument, the optimized build will be generated.

## **Usage**
To run the engine, we should specify some arguments:  

| Argument | Description |
| ------------- | ------------- |
| `--input` or `-i` | The pathname of the input sample |
| `--output` or `-o` | The pathname of the output report folder |
| `--dimension` or `-d` | The n-gram dimension |
| `--report` or `-t` | The control flags for report types |

- For `--dimension` - The minimum value is 1 and the maximum value is 3.
- For `--report` - There are 3 kinds of control flags
  + `e` - For text dump of entropy distribution.
  + `t` - For text dump of n-gram model.
  + `i` - For visualized image of n-gram model.
  + Note that the `t` flag should be specified before `i` flag. (e.g. `e`, `t`, `i`, `et`, `eti`)

The example command:
```sh
$ ./pe_ngram --input ~/mybin/a.exe --output /myreport/a --dimension 2 --report eti
```
or
```sh
$ ./pe_ngram -i ~/mybin/a.exe -o ~/mybin/a -d 2 --t eti
```

## **Demo**
| PE Binary Description | N-Gram Distribution Model |
| ------------- | ------------- |
| **32bit notepad.exe** | <img src="https://raw.githubusercontent.com/ZSShen/PE-NGram-Analysis/master/res/picture/Fsg_Notepad_ngram_model.png" width="460px" height="350px"/> |
| **UPX-protected notepad.exe** | <img src="https://raw.githubusercontent.com/ZSShen/PE-NGram-Analysis/master/res/picture/UPX_Notepad_ngram_model.png" width="440px" height="350px"/> |
| **RLPack-protected notepad.exe** | <img src="https://raw.githubusercontent.com/ZSShen/PE-NGram-Analysis/master/res/picture/RLPack_Notepad_ngram_model.png" width="440px" height="350px"/> |
| **Fsg-protected notepad.exe** | <img src="https://raw.githubusercontent.com/ZSShen/PE-NGram-Analysis/master/res/picture/Fsg_Notepad_ngram_model.png" width="440px" height="350px"/> |

## **Reference**
+ The project is inspired by the research paper from G.Jacob et. al.
  [A Static, Packer-Agnostic Filter to Detect Similar Malware Samples]
  (https://www.cs.ucsb.edu/~vigna/publications/2012_DIMVA_packedmalware.pdf)

## **Contact**
Please contact me via the mail ***andy.zsshen@gmail.com***.  

[CMake]:http://www.cmake.org/
[Valgrind]:http://valgrind.org/
[gnuplot]:http://www.gnuplot.info/
