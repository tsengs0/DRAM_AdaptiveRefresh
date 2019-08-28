#!/bin/bash
rm pattern_analysis ;
g++-5.4 -lm -std=c++11 -o pattern_analysis pattern_analysis.cpp ../src/mem_map.cpp ../inc/mem_map.h
