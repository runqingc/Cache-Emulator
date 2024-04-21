//
// Created by 蔡润青 on 2024/4/15.
//

#ifndef PROJECT_DRAFT_ADDRESS_H
#define PROJECT_DRAFT_ADDRESS_H

#include <cstdint>

class Address {
    int address;
public:
    static int indexSize;
    static int tagSize;
    static int offsetSize;
    Address();
    explicit Address(const int& address);
    int getIndex() const;
    int getTag() const;
    int getOffset() const;
    int getRamIndex() const;
    int getAll() const;
};


#endif //PROJECT_DRAFT_ADDRESS_H
