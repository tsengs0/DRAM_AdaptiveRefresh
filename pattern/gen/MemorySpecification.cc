/*
 * Copyright (c) 2014 TU Eindhoven and TU Czech.
 * All rights reserved.
 *
 * Licensed under BSD 3-Clause License
 *
 * Authors: Yonghui Li, Benny Akesson and Kees Goossens
 *
 */

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include "MemorySpecification.h"

using namespace std;
using namespace Data;

MemorySpecification::MemorySpecification(char* memSpec):
tRRD(0),
tRWTP(0),
tSwitch(0)
{
  //read the parameter value from the specified 
  //memory specification file.
  readParaValue(memSpec);

  //Only DDR3 SDRAM memories are considered, though
  //the value of memoryType can be obtained from the file.
  memoryType = MemorySpecification::DDR3;

  //set the values of the memory timing constraints and
  //the memory architecture parameters, respectively.
  memTimingSpec.processParameters(parametrisable);
  memArchSpec.processParameters(parametrisable);
}

void MemorySpecification::readParaValue(char* memSpec){

  ifstream memorySpec;
  memorySpec.open(memSpec);
  string line;
  string id;
  string value;
  string type("string");

  if(memorySpec.fail()){
    cout <<" Memory specification file not found!\n";
    exit(0);
  }

  parametrisable.parameters.clear();

  while(getline(memorySpec, line)){
    remove(line.begin(), line.end(), ' ');
    istringstream linestream(line);
    string item;
    int itemnum = 0;

    while (getline(linestream, item, ',')){

      if (itemnum == 0) {
        //the name of the parameter
        id = item;
      }
      else if (itemnum == 1) {
        //the value of the parameter
        value = item;
      }

      itemnum++;
    }
    Parameter parameter(id, type, value);

    //store the parameters in the vector "parameters".
    parametrisable.parameters.push_back(parameter);
  }

  memorySpec.close();
}

// get the timings for command scheduling according to different memories.
// In particular, tSwitch_init is generally used to provide the timings for
// scheduling a read/write command after a read/write command which have been
// scheduled to any possible banks within any possible bank groups (DDR4).
void MemorySpecification::getTimingConstraints(bool BGSwitch,
    Trans::TransType PreType, Trans::TransType CurrentType)
{
  int tRRD_init;
  int tCCD_init;
  int tWTR_init;
  int tSwitch_init;
  
  if (memoryType == MemorySpecification::DDR4) {
    if (BGSwitch) {
      tCCD_init = memTimingSpec.CCD_S;
      tRRD_init = memTimingSpec.RRD_S;
      tWTR_init = memTimingSpec.WTR_S;
    } else {
      tCCD_init = memTimingSpec.CCD_L;
      tRRD_init = memTimingSpec.RRD_L;
      tWTR_init = memTimingSpec.WTR_S;
    }
    if (PreType == CurrentType) {
      tSwitch_init = tCCD_init;
    } else if ((PreType == Trans::CMD_WRITE) && (CurrentType == Trans::CMD_READ)) {
      tSwitch_init = memTimingSpec.WL + memArchSpec.burstLength /
                     memArchSpec.dataRate + tWTR_init;
    }
  } else if (memoryType == MemorySpecification::WIDEIO_SDR) {
    if (PreType == CurrentType) {
      tSwitch_init = memTimingSpec.CCD;
    } else if ((PreType == Trans::CMD_WRITE) && (CurrentType == Trans::CMD_READ)) {
      tSwitch_init = memTimingSpec.WL + memArchSpec.burstLength /
                     memArchSpec.dataRate - 1 + memTimingSpec.WTR;
    }

    tRRD_init = memTimingSpec.RRD;
  } else {
    tRRD_init = memTimingSpec.RRD;
    if (PreType == CurrentType) {
      tSwitch_init = memTimingSpec.CCD;
    } else if ((PreType == Trans::CMD_WRITE) && (CurrentType == Trans::CMD_READ)) {
      tSwitch_init = memTimingSpec.WL + memArchSpec.burstLength /
                     memArchSpec.dataRate + memTimingSpec.WTR;
    }
  }

  if ((PreType == Trans::CMD_READ) && (CurrentType == Trans::CMD_WRITE)) {
    tSwitch_init = memTimingSpec.RL + memArchSpec.burstLength /
                   memArchSpec.dataRate + 2 - memTimingSpec.WL;
  }

  setRRD(tRRD_init);
  setRWTP(getRWTP(CurrentType));
  setSwitch(tSwitch_init);
}

//////////////////////////////////////////////////////////////////////////////
// get the time when a precharge occurs after a read/write command is scheduled.
// In addition, it copes with different kind of memories.
int MemorySpecification::getRWTP(Trans::TransType transType)
{
  long int tRWTP_init = 0;

  if (transType == Trans::CMD_READ) {
    switch (memoryType) {
    case MemorySpecification::LPDDR:
    case MemorySpecification::WIDEIO_SDR:
      tRWTP_init = memArchSpec.burstLength / memArchSpec.dataRate;
      break;
    case MemorySpecification::LPDDR3:
      tRWTP_init = memArchSpec.burstLength / memArchSpec.dataRate +
                   max(0, memTimingSpec.RTP - 2);
      break;
    case MemorySpecification::DDR2:
      tRWTP_init = memTimingSpec.AL + memArchSpec.burstLength /
                   memArchSpec.dataRate +
                   max(memTimingSpec.RTP, 2) - 2;
      break;
    case MemorySpecification::DDR3:
    case MemorySpecification::DDR4:
      tRWTP_init = memTimingSpec.RTP;
      break;
    default:
      break;
    }
  } else if (transType == Trans::CMD_WRITE) {
    if (memoryType == MemorySpecification::WIDEIO_SDR) {
      tRWTP_init = memTimingSpec.WL + memArchSpec.burstLength /
                   memArchSpec.dataRate - 1 + memTimingSpec.WR;
    } else {
      tRWTP_init = memTimingSpec.WL + memArchSpec.burstLength /
                   memArchSpec.dataRate + memTimingSpec.WR;
    }
  }
  return tRWTP_init;
}