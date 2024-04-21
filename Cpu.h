//
// Created by 蔡润青 on 2024/4/16.
//

#ifndef PROJECT_DRAFT_CPU_H
#define PROJECT_DRAFT_CPU_H

#include "Address.h"
#include "Cache.h"
#include <memory>

class Cpu {
public:
    static long instructionCount;
    explicit Cpu(std::shared_ptr<Cache> cache);
    std::shared_ptr<Cache> cache;
    [[nodiscard]] double loadDouble(Address address) const;
    void storeDouble(Address address, double value) const;
    inline static double addDouble(double value1, double value2){
        ++instructionCount;
        return value1 + value2;
    };
    inline static double multDouble(double value1, double value2){
        ++instructionCount;
        return value1 * value2;
    };

};


#endif //PROJECT_DRAFT_CPU_H
