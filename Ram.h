//
// Created by 蔡润青 on 2024/4/15.
//

#ifndef PROJECT_DRAFT_RAM_H
#define PROJECT_DRAFT_RAM_H

#include <vector>
#include <memory>
#include "Address.h"
#include "DataBlock.h"

class Ram {
public:
    static int numBlock;
    std::vector<std::shared_ptr<DataBlock>> data;
    Ram();
    std::shared_ptr<DataBlock> getBlock(Address address);
    void setBlock(Address address, DataBlock& dataBlock);
    void setDouble(const Address& address, const double& value);
    double getDouble(const Address& address);
};




#endif //PROJECT_DRAFT_RAM_H
