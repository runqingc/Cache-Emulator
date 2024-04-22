//
// Created by 蔡润青 on 2024/4/15.
//

#ifndef PROJECT_DRAFT_CACHE_H
#define PROJECT_DRAFT_CACHE_H

#include <cstdint>
#include <utility>
#include <vector>
#include <list>
#include <memory>
#include <random>
#include "Address.h"
#include "DataBlock.h"
#include "Ram.h"

class Cache;

struct CacheEntry{
    bool valid;
    int tag;
    std::shared_ptr<DataBlock> data;
    CacheEntry():valid(false), tag(0){}
    CacheEntry(const bool valid, const int tag, const std::shared_ptr<DataBlock>& data):
    valid(valid), tag(tag), data(data){}
};


class Cache {
public:
    static int numSets;
    static int numBlocks;
    static int readHit;
    static int readMiss;
    static int writeHit;
    static int writeMiss;
    std::shared_ptr<Ram> ram;
    explicit Cache(std::shared_ptr<Ram> ram);
    virtual std::shared_ptr<DataBlock> getBlock(Address address) = 0;
    double getDouble(Address address);
    virtual void setDouble(Address address, double value) = 0;
    virtual ~Cache() = default;
};


class RandomCache: public Cache{
private:
    std::mt19937 gen;
    std::uniform_int_distribution<> distrib;
    std::vector<std::vector<std::shared_ptr<CacheEntry>>> blocks;
public:
    explicit RandomCache(std::shared_ptr<Ram> ram);
    std::shared_ptr<DataBlock> getBlock(Address address) override;
    void setDouble(Address address, double value) override;
};


class LRUCache: public Cache{
private:
    std::vector<std::list<std::shared_ptr<CacheEntry>>> blocks;
public:
    explicit LRUCache(std::shared_ptr<Ram> ram);
    std::shared_ptr<DataBlock> getBlock(Address address) override;
    void setDouble(Address address, double value) override;
    template<class T>
    static void moveTop(std::list<T>& list, typename std::list<T>::iterator node);
};

// helper function, move the list node to the front of the list
template<class T>
void LRUCache::moveTop(std::list<T>& list, typename std::list<T>::iterator node){
    T nodeValue = *node;
    list.push_front(nodeValue);
    list.erase(node);
}


class FIFOCache:public Cache{
private:
    std::vector<std::vector<std::shared_ptr<CacheEntry>>> blocks;
    std::vector<int> nextFree;
public:
    static int setSize;
    explicit FIFOCache(const std::shared_ptr<Ram>& ram);
    std::shared_ptr<DataBlock> getBlock(Address address) override;
    void setDouble(Address address, double value) override;
};



#endif //PROJECT_DRAFT_CACHE_H
