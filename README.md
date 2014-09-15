#Windows-PE-Research


##Introduction

This is the research project for Windoes PE files which tries to discover the potential 
similarity among a file and its obfuscated instances using different types of packers.

Specifically, it transforms a givien file into the n-gram distribution model with a set 
of selected binary blocks. Such frequency model will be the core material for similarity 
comparison.

Currently, the project focuses on the preciseness of model generation. For the different
kinds of applications like file clustering and classification, one can fork the project
as the core utility for system integration.

##Usage


##Reference

The project is inspired by the research paper from G.Jacob et. al.  
* [A Static, Packer-Agnostic Filter to Detect Similar Malware Samples]
(https://www.cs.ucsb.edu/~vigna/publications/2012_DIMVA_packedmalware.pdf)
