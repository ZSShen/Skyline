#Windows-PE-Research


##Introduction

A research project for Windoes PE which tries to discover the 
similarity among a file and its obfuscated instances using 
different types of packers.

Specifically, it transforms a givien file into the distribution
model of n-gram tokens using a set of binary blocks selected by
heuristics. Such frequency model will be the core material for 
similarity comparison.

Currently, this project focuses on the n-gram model generation. 
For the applications like clustering and classification for
similar PE files, one can fork the project as the core utility 
for system integration.

##Usage
###1. Source Building
    The project can now be utilized only under the Linux-related platform.
    
    It can be divided into two parts:
        The engine which controls the entire workflow.
        The plugins for binary block selection and model generation.
        
    For the two aspects, the default selected plugins are:
        Retrieve the section with maximum average entropy.
        Generate the frequency model with descending order.
    
    Besides, the engine can provide the graphcial report
    to visualize the tend of distribution. For this, please
    make sure that the "gnuplot" utility is already installed.
    
    On the other hand, the engine can be built for normal or debug version.
    For debug version, please make sure that the "valgrind" utility is
    already installed to turn on the memory usage inspection.

####1.1 Source Tree
    |-- Makefile
    |-- src
        |-- main.c
        |-- util.c
        |-- except.c
        |-- pe_info.c
        |-- region.c
        |-- ngram.c
        |-- report.c
    |-- include
        |-- util.h
        |-- except.h
        |-- pe_info.h
        |-- region.h
        |-- ngram.h
        |-- report.h
    |-- plugin
        |-- Makefile
        |-- src
            |--Region_Template.c (The template for plugin.)
            |--Region_MaxEntropySection.c (The default plugin.)
            |--Model_Template.c (The template for plugin.)
            |--Model_DescendingFrequency.c (The default plugin.)
        |-- obj (after first build.)
        |-- lib (after first build.)
    |-- obj (after first build.)
    |-- release (after first build.)
    |-- debug (after first build.)
    
    
####1.2 Building Entire Source
    There is a major Makefile at the top of source tree:
        For the normal release build, execute `make`.
        For the debug build, execute `make DEBUG=true`.
    
    Note that both kinds of builds will also compile the default plugins.

####1.3 Builing Plugin
    There is a second-level Makefile in the plugin folder:
        For the plugin of binary block selection:
            Execute `make dynamic_region REGION=[NameOfSourceFile]`.
            e.g.: `make dynamic_region REGION=Region_MaxEntropySection`
    
        For the plugin fo model generation:
            Execute `make dynamic_model MODEL=[NameOfSourceFile]`.
            e.g.: `make dynamic_model MODEL=Model_DescendingFrequency`

###2. Binary Execution
    The built binary is named "pe_ngram". Actually, one can execute
    `pe_ngram -h` or `pe_ngram --help` for detail usage.

    There are four arguments to be specified:
    --input     | -i [PathInputFile]
    --output    | -o [PathOutputFolder]
    --dimension | -d [NgramDimension]
    --report    | -t [TypesOfReports]
    
    PathInputFile   : The path to the input file.
                      (Only accept absolute path.)
    PathOutputFolder: The path to the output report folder.
                      (Only accept absolute path.)
    NgramDimension  : The dimension used to generate ngram tokens. 
                      (Only accept range from 1 to 4.)
    TypeOfReports   : The set of control flags for report types.
                      (Flag 'e': For text dump of entropy information.)
                      (Flag 't': For text dump of n-gram distribution.)
                      (Flag 'i': For visualized image of n-gram model.)
                      (Note that the image is dependent on the text dump.)
                      
    Usage Example:
        `pe_ngram --input /path/a.exe --output /path/a.data/ --dimension 2 --report eti`
        `pe_ngram -i      /path/a.exe -o       /path/a.data/ -d          2 -t       eti`

##Reference

The project is inspired by the research paper from G.Jacob et. al.  
* [A Static, Packer-Agnostic Filter to Detect Similar Malware Samples]
(https://www.cs.ucsb.edu/~vigna/publications/2012_DIMVA_packedmalware.pdf)
