//
// Created by 蔡润青 on 2024/4/15.
//

#include "Cache.h"
#include <random>
#include <utility>
#include <iostream>

int Cache::numSets = 0;
int Cache::numBlocks = 0;
int Cache::readHit = 0;
int Cache::readMiss = 0;
int Cache::writeHit = 0;
int Cache::writeMiss = 0;

int FIFOCache::setSize = 0;

Cache::Cache(std::shared_ptr<Ram> ram):ram(std::move(ram)){}

double Cache::getDouble(Address address){
    return getBlock(address)->data[address.getOffset()];
}



RandomCache::RandomCache(std::shared_ptr<Ram> ram):
Cache(std::move(ram)),
gen(std::random_device{}()),
distrib(0, Cache::numBlocks / Cache::numSets - 1) {
    blocks.resize(numSets);
    int blocksPerSet = numBlocks / numSets;
    // construct the cache data to empty matrix of numSet*blocksPerSet
    for(int i=0; i<numSets; ++i){
        blocks[i].resize(blocksPerSet);
        for(int j=0; j<blocksPerSet; ++j){
            blocks[i][j] = std::make_shared<CacheEntry>();
        }
    }
}



// strategy: traverse all data block within a given set
// if found, return the Data Block;
// if not, let ram find it, and evict a random Data Block
std::shared_ptr<DataBlock> RandomCache::getBlock(Address address){
    int setIndex = address.getRamIndex() % Cache::numSets;
    int tag = address.getTag();
    int firstEmptyIndex = -1;
    // Case 1: find in cache
    for(int i=0; i<blocks[setIndex].size(); ++i){
        if(firstEmptyIndex == -1 && !blocks[setIndex][i]->valid){
            firstEmptyIndex = i;
        }else if(blocks[setIndex][i]->valid && blocks[setIndex][i]->tag == tag){
            ++readHit;
            return blocks[setIndex][i]->data;
        }
    }
    ++readMiss;
    // Case 2: not found in cache but cache has empty slot
    std::shared_ptr<DataBlock> res = ram->getBlock(address);
    if(firstEmptyIndex != -1){
        blocks[setIndex][firstEmptyIndex] -> valid = true;
        blocks[setIndex][firstEmptyIndex] -> tag = tag;
        blocks[setIndex][firstEmptyIndex] -> data = res;
    }
    // Case 3: evict some random block
    else{
        int evictIndex = distrib(gen);
        blocks[setIndex][evictIndex] -> tag = tag;
        blocks[setIndex][evictIndex] -> data = res;
    }
    return res;
}

void RandomCache::setDouble(Address address, double value){

    int setIndex = address.getRamIndex() % Cache::numSets;
    int tag = address.getTag();
    int offset = address.getOffset();
    int firstEmptyIndex = -1;
    // Case 1: find in cache
    for(int i=0; i<blocks[setIndex].size(); ++i){
        if(firstEmptyIndex == -1 && !blocks[setIndex][i]->valid){
            firstEmptyIndex = i;
        }else if(blocks[setIndex][i]->valid && blocks[setIndex][i]->tag == tag){
            ++writeHit;
            blocks[setIndex][i] -> data->data[offset] = value;
            return;
        }
    }
    ++writeMiss;

    // Case 2: not found in cache but cache has empty slot
    if(firstEmptyIndex != -1){
        blocks[setIndex][firstEmptyIndex] -> valid = true;
        blocks[setIndex][firstEmptyIndex] -> tag = tag;
        blocks[setIndex][firstEmptyIndex] -> data = ram->getBlock(address);
        blocks[setIndex][firstEmptyIndex] -> data->data[offset] = value;
    }
    // Case 3: evict some random block
    else{
        int evictIndex = distrib(gen);
        blocks[setIndex][evictIndex] -> tag = tag;
        blocks[setIndex][evictIndex] -> data = ram->getBlock(address);
        blocks[setIndex][evictIndex] -> data->data[offset] = value;
    }
}


LRUCache::LRUCache(std::shared_ptr<Ram> ram): Cache(std::move(ram)) {
    blocks.resize(numSets);
}

std::shared_ptr<DataBlock> LRUCache::getBlock(Address address){
    int setIndex = address.getRamIndex() % Cache::numSets;
    int tag = address.getTag();

    // case 1: found in cache (cache Hit)
    for(auto it = blocks[setIndex].begin(); it!=blocks[setIndex].end(); ++it){
        if((*it)->tag == tag){
            ++readHit;
            moveTop(blocks[setIndex], it);
            return blocks[setIndex].front()->data;
        }
    }

    ++readMiss;
    std::shared_ptr<DataBlock> res = ram->getBlock(address);
    // case 2: not found in cache, yet cache has empty block
    if(blocks[setIndex].size()< numBlocks/numSets){
        blocks[setIndex].emplace_front(std::make_shared<CacheEntry>(true, tag, std::move(res)));
        return blocks[setIndex].front()->data;
    }

    // case 3: not found in cache, and cache does not have empty block
    blocks[setIndex].pop_back();
    blocks[setIndex].emplace_front(std::make_shared<CacheEntry>(true, tag, std::move(res)));
    return blocks[setIndex].front()->data;
}



void LRUCache::setDouble(Address address, double value){

    int setIndex = address.getRamIndex() % Cache::numSets;
    int offset = address.getOffset();
    int tag = address.getTag();

    // case 1: found in cache (cache Hit)
    for(auto it = blocks[setIndex].begin(); it!=blocks[setIndex].end(); ++it){
        if((*it)->tag == tag){
            ++writeHit;
            moveTop(blocks[setIndex], it);
            blocks[setIndex].front()->data->data[offset] = value;
            return;
        }
    }

    ++writeMiss;
    std::shared_ptr<DataBlock> res = ram->getBlock(address);
    // case 2: not found in cache, yet cache has empty block
    if(blocks[setIndex].size()< numBlocks/numSets){
        blocks[setIndex].emplace_front(std::make_shared<CacheEntry>(true, tag, std::move(res)));
        blocks[setIndex].front()->data->data[offset] = value;
        return;
    }

    // case 3: not found in cache, and cache does not have empty block
    blocks[setIndex].pop_back();
    blocks[setIndex].emplace_front(std::make_shared<CacheEntry>(true, tag, std::move(res)));
    blocks[setIndex].front()->data->data[offset] = value;

}


FIFOCache::FIFOCache(const std::shared_ptr<Ram>& ram): Cache(ram){
    setSize = numBlocks/numSets;
    nextFree.resize(numSets);
    std::fill(nextFree.begin(), nextFree.end(), 0);
    blocks.resize(numSets);
    // construct the cache data to empty matrix of numSet*blocksPerSet
    for(int i=0; i<numSets; ++i){
        blocks[i].resize(setSize);
        for(int j=0; j<setSize; ++j){
            blocks[i][j] = std::make_shared<CacheEntry>();
        }
    }
}

std::shared_ptr<DataBlock> FIFOCache::getBlock(Address address){

    int setIndex = address.getRamIndex() % Cache::numSets;
    int tag = address.getTag();
    // Case 1: find in cache
    for(const auto & i : blocks[setIndex]){
        if(i->valid && i->tag == tag){
            ++readHit;
            return i->data;
        }
    }
    ++readMiss;
    // case 2/3: not found in cache, evict cache line in nextFree
    std::shared_ptr<DataBlock> res = ram->getBlock(address);
    int evictIndex = nextFree[setIndex];
    nextFree[setIndex] = (nextFree[setIndex]+1)%setSize;
    blocks[setIndex][evictIndex] -> valid = true;
    blocks[setIndex][evictIndex] -> tag = tag;
    blocks[setIndex][evictIndex] -> data = res;
    return res;
}

void FIFOCache::setDouble(Address address, double value){

    int setIndex = address.getRamIndex() % Cache::numSets;
    int tag = address.getTag();
    int offset = address.getOffset();

    // Case 1: find in cache
    for(const auto & i : blocks[setIndex]){
        if(i->valid && i->tag == tag){
            ++writeHit;
            i->data->data[offset] = value;
            return;
        }
    }
    ++writeMiss;
    // case 2/3: not found in cache, evict cache line in nextFree
    std::shared_ptr<DataBlock> res = ram->getBlock(address);
    int evictIndex = nextFree[setIndex];
    nextFree[setIndex] = (nextFree[setIndex]+1)%setSize;
    blocks[setIndex][evictIndex] -> valid = true;
    blocks[setIndex][evictIndex] -> tag = tag;
    blocks[setIndex][evictIndex] -> data = res;
    blocks[setIndex][evictIndex] -> data->data[offset] = value;
}