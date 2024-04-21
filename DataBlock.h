//
// Created by 蔡润青 on 2024/4/15.
//

#ifndef PROJECT_DRAFT_DATABLOCK_H
#define PROJECT_DRAFT_DATABLOCK_H
#include <vector>

class DataBlock {
public:
    static int size;
    std::vector<double> data;
    DataBlock();
    DataBlock(std::vector<double>& dataPayload);
};


#endif //PROJECT_DRAFT_DATABLOCK_H
