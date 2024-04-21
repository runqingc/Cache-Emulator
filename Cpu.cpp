//
// Created by 蔡润青 on 2024/4/16.
//

#include "Cpu.h"

#include <utility>

long Cpu::instructionCount = 0;

Cpu::Cpu(std::shared_ptr<Cache> cache):cache(std::move(cache)){}


double Cpu::loadDouble(Address address) const{
    ++instructionCount;
    return cache->getDouble(address);
}
void Cpu::storeDouble(Address address, double value) const{
    ++instructionCount;
    cache->setDouble(address, value);
}

