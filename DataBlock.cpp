//
// Created by 蔡润青 on 2024/4/15.
//

#include "DataBlock.h"
int DataBlock::size = 0;

DataBlock::DataBlock(){
    data.resize(size);
}


DataBlock::DataBlock(std::vector<double>& dataPayload){
    this->data = std::move(dataPayload);
}