#!/bin/bash
rm pattern_gen ;
g++-5.4 -lm -std=c++11 -o pattern_gen pattern_gen.cpp ../src/mem_map.cpp ../inc/mem_map.h
