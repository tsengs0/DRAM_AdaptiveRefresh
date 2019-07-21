/*
 * Copyright (c) 2014 TU Eindhoven and TU Czech.
 * All rights reserved.
 *
 * Licensed under BSD 3-Clause License
 *
 * Authors: Yonghui Li, Benny Akesson and Kees Goossens
 *
 */

#ifndef TRANS_H
#define	TRANS_H

#include "MemCtrlConfig.h"

namespace Data{
class Trans {
   public:
    enum TransType {CMD_READ, CMD_WRITE, CMD_WRITE_ACK, CMD_INVALID};
    enum CmdType {ACT = 0, READ = 1, WRITE = 2, PRE = 3, REF = 4, NOP = 5, PREA = 6};
    Trans():
      id(0),
      type(CMD_INVALID),
      arrivalTimeCc(0),
      logicalAddress(0),
      size(0)
    {
    }
    ~Trans()
    {
    }

    class PhysicalAddr {
    public:
      PhysicalAddr():
        logicalAddr(0),
        rowAddr(0),
        bankAddr(0),
        bankGroupAddr(0),
        colAddr(0)
      {
      }
      ~PhysicalAddr()
      {
      }

      unsigned logicalAddr;
      unsigned rowAddr;
      unsigned bankAddr;
      unsigned bankGroupAddr;
      unsigned colAddr;
    };    
    
    int id;
    TransType type;
    long int arrivalTimeCc;
    unsigned logicalAddress;
    unsigned int size;
    PhysicalAddr physicalAddr;
    MemCtrlConfig memCtrlConfig;
  };
}
#endif	/* TRANS_H */

