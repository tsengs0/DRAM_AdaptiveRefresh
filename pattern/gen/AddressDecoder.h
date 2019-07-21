/*
 * Copyright (c) 2014 TU Eindhoven and TU Czech.
 * All rights reserved.
 *
 * Licensed under BSD 3-Clause License
 *
 * Authors: Yonghui Li, Benny Akesson and Kees Goossens
 *
 */

#ifndef ADDRESSDECODER_H
#define	ADDRESSDECODER_H

#include "Trans.h"

namespace Data{
  //translate the logical address of a transaction
  //into the physical address.
  class AddressDecoder{
  public:
    AddressDecoder(const Data::MemorySpecification& _memSpec):
    memSpec(_memSpec){
    }
    ~AddressDecoder(){
    }
    //provides the physical address in terms of bank row and column.
    Trans::PhysicalAddr MemoryMap(const Trans& trans);
  private:
    const Data::MemorySpecification& memSpec;
  };
}


#endif	/* ADDRESSDECODER_H */

