/*
 * Copyright (c) 2014 TU Eindhoven and TU Czech.
 * All rights reserved.
 *
 * Licensed under BSD 3-Clause License
 *
 * Authors: Yonghui Li, Benny Akesson and Kees Goossens
 *
 */

#ifndef MEMORYSPECIFICATION_H
#define	MEMORYSPECIFICATION_H

#include <string.h>
#include <vector>
#include <sstream>
#include <iostream>
#include "MemTimingSpec.h"
#include "MemArchitectureSpec.h"
#include "Parameter.h"
#include "Trans.h"

using namespace std;

namespace Data {
class MemorySpecification: public MemTimingSpec, public MemArchitectureSpec{
  public:
    MemorySpecification(char* memSpec);
    MemorySpecification(){
    }
    ~MemorySpecification(){
    }
    
    void readParaValue(char* memSpec);

    MemTimingSpec memTimingSpec;
    MemArchitectureSpec memArchSpec;
    Parametrisable parametrisable;

    enum MemoryType {
      SRAM,
      SRAM_ML605,
      DDR2,
      DDR3,
      DDR4,
      LPDDR,
      LPDDR2,
      LPDDR3,
      WIDEIO_SDR,
      WIDEIO_DDR,
      MEMORY_TYPE_INVALID
    };    

    MemoryType memoryType;

    class ParaSchedule{
    public:
      ParaSchedule():
      tRRD_init(0),
      tSwitch_init(0),
      tRWTP_init(0){
      }
      ~ParaSchedule(){
      }

      int tRRD_init;
      int tSwitch_init;
      int tRWTP_init;
    };

    void getTimingConstraints (bool BGSwitch, Trans::TransType PreType, Trans::TransType CurrentType);
    int getRWTP(Trans::TransType transType);

    void setRRD(int _tRRD) {tRRD = _tRRD;}
    void setRWTP(int _tRWTP) {tRWTP = _tRWTP;}
    void setSwitch(int _tSwitch) {tSwitch = _tSwitch;}

    int getRRD() {return tRRD;}
    int getRWTP() {return tRWTP;}
    int getSwitch() {return tSwitch;}

private:
    int tRRD;
    int tRWTP;
    int tSwitch;    
  };
}

#endif	/* MEMORYSPECIFICATION_H */

