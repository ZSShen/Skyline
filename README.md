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
####1. Source Building

####2. Binary Execution

##Reference

The project is inspired by the research paper from G.Jacob et. al.  
* [A Static, Packer-Agnostic Filter to Detect Similar Malware Samples]
(https://www.cs.ucsb.edu/~vigna/publications/2012_DIMVA_packedmalware.pdf)
