//
// Created by 蔡润青 on 2024/4/15.
//

#include "Ram.h"
#include <iostream>
#include <utility>

int Ram::numBlock = 0;

Ram::Ram() {
    data.resize(numBlock);
    for(int i=0; i<numBlock; ++i){
        data[i] = std::make_shared<DataBlock>();
    }
}

std::shared_ptr<DataBlock> Ram::getBlock(Address address){
    return data[address.getRamIndex()];
}

void Ram::setBlock(Address address, DataBlock& dataBlock){
    *data[address.getRamIndex()] = std::move(dataBlock);
}

// used in initializing arrays and matrix
void Ram::setDouble(const Address& address, const double& value){
    data[address.getRamIndex()]->data[address.getOffset()] = value;
}

double Ram::getDouble(const Address& address){
    return data[address.getRamIndex()]->data[address.getOffset()];
}