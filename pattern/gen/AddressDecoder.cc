/*
 * Copyright (c) 2014 TU Eindhoven and TU Czech.
 * All rights reserved.
 *
 * Licensed under BSD 3-Clause License
 *
 * Authors: Yonghui Li, Benny Akesson and Kees Goossens
 *
 */

#include <assert.h>
#include <math.h>
#include "MemorySpecification.h"
#include "AddressDecoder.h"

using namespace Data;

//////////////////////////////////////////////////////////////////////////////
// The logical address of each transaction is translated into a physical address
// which consists of bank group (DDR4), bank, row and column addresses.
Trans::PhysicalAddr AddressDecoder::MemoryMap(const Trans& trans)
{

  int DecLogic;
  Trans::PhysicalAddr physicalAddr;
  DecLogic = trans.logicalAddress;
  physicalAddr.logicalAddr = trans.logicalAddress;
  MemCtrlConfig memCtrlConfig = trans.memCtrlConfig;

  unsigned nBanks          = memSpec.memArchSpec.nbrOfBanks;
  unsigned nColumns        = memSpec.memArchSpec.nbrOfColumns;
  unsigned nRows           = memSpec.memArchSpec.nbrOfRows;
  unsigned burstLength     = memSpec.memArchSpec.burstLength;
  unsigned nbrOfBankGroups = 1;

  int BGI = 1;    // No support for BGI
  int BI  = memCtrlConfig.BI;
  int BC  = memCtrlConfig.BC;

  assert(nBanks > 0 && nColumns > 0 && nRows > 0 && burstLength > 0);
  assert(BGI >= 0 && BI > 0 && BC > 0);

  // row-bank-column-BI-BC-BGI-BL
  if (BGI >= 1 && memSpec.memoryType == MemorySpecification::DDR4) {
    unsigned colBits = static_cast<unsigned> (log2(nColumns));
    unsigned bankShift = static_cast<unsigned> (colBits + ((BI > 1) ? log2(BI) : 0)
                         + ((BGI > 1) ? log2(BGI) : 0));
    unsigned bankMask = static_cast<unsigned> (nBanks / (BI * nbrOfBankGroups) - 1)
                        << bankShift;
    unsigned bankAddr = (DecLogic & bankMask) >>
                        static_cast<unsigned>(colBits + ((BGI > 1) ? log2(BGI) : 0));
    physicalAddr.bankAddr = bankAddr;

    unsigned bankGroupShift = static_cast<unsigned> (log2(burstLength));
    unsigned bankGroupMask = (nbrOfBankGroups / BGI - 1) << bankGroupShift;
    unsigned bankGroupAddr = (DecLogic & bankGroupMask) >> bankGroupShift;
    physicalAddr.bankGroupAddr = bankGroupAddr;

    unsigned colShift = static_cast<unsigned> (log2(BC * burstLength) +
                        ((BI > 1) ? log2(BI) : 0) + ((BGI > 1) ? log2(BGI) : 0));
    unsigned colMask = static_cast<unsigned> (nColumns / (BC * burstLength) - 1)
                       << colShift;
    unsigned colAddr = (DecLogic & colMask) >>
                       static_cast<unsigned>((colShift - log2(static_cast<unsigned>(BC) * burstLength)));
    physicalAddr.colAddr = colAddr;
  } else {
    unsigned colBits = static_cast<unsigned> (log2(nColumns));
    unsigned bankShift = static_cast<unsigned> (colBits + ((BI > 1) ? log2(BI) : 0));
    unsigned bankMask = static_cast<unsigned> (nBanks / BI - 1) << bankShift;
    unsigned bankAddr = (DecLogic & bankMask) >> colBits;
    physicalAddr.bankAddr = bankAddr;

    unsigned colShift = static_cast<unsigned> (log2(BC * burstLength) +
                        ((BI > 1) ? log2(BI) : 0));
    unsigned colMask = static_cast<unsigned> (nColumns / (BC * burstLength) - 1)
                       << colShift;
    unsigned colAddr = (DecLogic & colMask) >>
                       static_cast<unsigned>((colShift - log2(static_cast<unsigned>(BC) * burstLength)));
    physicalAddr.colAddr = colAddr;

    physicalAddr.bankGroupAddr = 0;
  }

  unsigned rowShift = static_cast<unsigned> (log2(nColumns * nBanks));
  unsigned rowMask = static_cast<unsigned> (nRows - 1) << rowShift;
  unsigned rowAddr = (DecLogic & rowMask) >> rowShift;
  physicalAddr.rowAddr = rowAddr;

  return physicalAddr;
}

