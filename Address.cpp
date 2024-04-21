//
// Created by 蔡润青 on 2024/4/15.
//

#include "Address.h"


int Address::indexSize = 0;
int Address::tagSize = 0;
int Address::offsetSize = 0;

Address::Address():address(0){}

Address::Address(const int& address):address(address>>3) {
}

int Address::getIndex() const {
    return address >> (tagSize + offsetSize);
}

int Address::getTag() const {
    return (address >> offsetSize) & ((1<< tagSize)-1);
}

int Address::getOffset() const {
    return address & ((1<<offsetSize)-1);
}

int Address::getRamIndex() const{
    return address >> offsetSize;
}

int Address::getAll() const {
    return address;
}
