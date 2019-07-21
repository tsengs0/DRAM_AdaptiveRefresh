/*
 * Copyright (c) 2014 TU Eindhoven and TU Czech.
 * All rights reserved.
 *
 * Licensed under BSD 3-Clause License
 *
 * Authors: Yonghui Li, Benny Akesson and Kees Goossens
 *
 */

#ifndef UTILS_H
#define	UTILS_H

#include <string>
#include <sstream>
#include <typeinfo>
#include <stdexcept>

#define MILLION 1000000
//transaction sizes: 16 Bytes, 32 Bytes, 64 Bytes,
//128 Bytes, 256 Bytes, 512 Bytes and 1024 Bytes
#define NUM_TRANSACTION_SIZES 7

template<typename T>
T fromString(const std::string& s,
        std::ios_base& (*f)(std::ios_base&) = std::dec)
throw (std::runtime_error) {
    std::istringstream is(s);
    T t;
    if (!(is >> f >> t)) {
        throw std::runtime_error("Cannot convert '" + s + "' to " +
                typeid (t).name() + " using fromString");

    }

    return t;
}

#endif	/* UTILS_H */