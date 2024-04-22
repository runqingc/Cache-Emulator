#include <iostream>
#include <cmath>
#include <memory>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cassert>

#include "Address.h"
#include "DataBlock.h"
#include "Cache.h"
#include "Ram.h"
#include "Cpu.h"

using namespace std;

enum class ReplacementPolicy {
    Random,
    FIFO,
    LRU
};

enum class AlgorithmPolicy{
    daxpy,
    mxm,
    mxm_block
};

const int sz = 8;
const int addressSize = 32;

int cacheSize;
int dataBlockSize;
int associativity;
ReplacementPolicy replacement;
AlgorithmPolicy algorithm;
int dimension;
bool printEnabled;
int blockingFactor;

shared_ptr<Ram> ram;
shared_ptr<Cache> cache;
shared_ptr<Cpu> cpu;

void parseInput(int argc, char** argv){


    // default settings
    cacheSize = 524288;
    dataBlockSize = 64;   // first /8 then*8? keep it as a multiple of 8
    associativity = 2;
    replacement = ReplacementPolicy::LRU;
    algorithm = AlgorithmPolicy::mxm_block;  // [TODO] change back to mxm_block back !
    printEnabled = true;
    dimension = 480;
    blockingFactor = 32;

    // read in input arguments
    for(int i=1; i<argc; ++i){
        std::string arg = argv[i];
        if(arg == "-c" && i+1<argc){
            cacheSize = std::stoi(argv[++i]);
        }else if(arg == "-b" && i+1<argc){
            dataBlockSize = std::stoi(argv[++i]);
        }else if(arg == "-n" && i+1<argc){
            associativity = std::stoi(argv[++i]);
        }else if(arg == "-r" && i+1<argc){
            std::string r = argv[++i];
            if(r=="FIFO"){
                replacement = ReplacementPolicy::FIFO;
            }else if(r=="random"){
                replacement = ReplacementPolicy::Random;
            }
        }else if(arg == "-d" && i+1<argc){
            dimension = std::stoi(argv[++i]);
        }else if(arg == "-a" && i+1<argc){
            std::string a = argv[++i];
            if(a=="daxpy"){
                algorithm = AlgorithmPolicy::daxpy;
            }else if(a=="mxm"){
                algorithm = AlgorithmPolicy::mxm;
            }
        }else if(arg == "-p"){
            printEnabled = true;
        }else if(arg == "-f" && i+1<argc){
            blockingFactor = std::stoi(argv[++i]);
        }
    }

}


void initializeEmulator(){

    // initialize Data Block
    DataBlock::size = dataBlockSize / sz;   // suppose divisible!

    // initialize Cache
    Cache::numBlocks = cacheSize / dataBlockSize;  // suppose divisible!
    Cache::numSets = Cache::numBlocks / associativity;  // suppose divisible!

    // initialize Address
    Address::indexSize = std::ceil(std::log(Cache::numSets) / std::log(2));
    Address::offsetSize = std::ceil(std::log(DataBlock::size) / std::log(2));
    Address::tagSize = addressSize - Address::indexSize - Address::offsetSize;



    // initialize Ram
    if(algorithm==AlgorithmPolicy::daxpy){
        Ram::numBlock = ceil(3.0 * dimension / DataBlock::size);
    }else{
        Ram::numBlock = ceil(3.0 * dimension * dimension / DataBlock::size);
    }

    ram = make_shared<Ram>();


    switch (replacement) {
        case ReplacementPolicy::Random:
            cache = make_shared<RandomCache>(ram); break;
        case ReplacementPolicy::LRU:
            cache = make_shared<LRUCache>(ram); break;
        case ReplacementPolicy::FIFO:
            cache = make_shared<FIFOCache>(ram); break;
    }

    cpu = make_shared<Cpu>(cache);

}

void printInput(){
    std::cout << "INPUTS====================================" << std::endl;
    std::cout << "Ram Size =                   " << Ram::numBlock * DataBlock::size * sz << " bytes" << std::endl;
    std::cout << "Cache Size =                 " << cacheSize << " bytes" << std::endl;
    std::cout << "Block Size =                 " << dataBlockSize << std::endl;
    std::cout << "Total Blocks in Cache =      " << Cache::numBlocks << std::endl;
    std::cout << "Associativity =              " << associativity << std::endl;
    std::cout << "Number of Sets =             " << Cache::numSets << std::endl;
    switch (replacement) {
        case ReplacementPolicy::Random:
            std::cout << "Replacement Policy =         Random" << std::endl; break;
        case ReplacementPolicy::LRU:
            std::cout << "Replacement Policy =         LRU" << std::endl; break;
        case ReplacementPolicy::FIFO:
            std::cout << "Replacement Policy =         FIFO" << std::endl; break;
    }
    switch (algorithm) {
        case AlgorithmPolicy::daxpy:
            std::cout << "Algorithm =                  daxpy" << std::endl; break;
        case AlgorithmPolicy::mxm:
            std::cout << "Algorithm =                  mxm" << std::endl; break;
        case AlgorithmPolicy::mxm_block:
            std::cout << "Algorithm =                  mxm_block" << std::endl; break;
    }
    if(algorithm==AlgorithmPolicy::mxm_block)
        std::cout << "MXM Blocking Factor =        " << blockingFactor << std::endl;
    std::cout << "Matrix or Vector dimension = " << dimension << std::endl;
}

void printResult(){
    cout << "RESULTS====================================" << endl;
    cout << "Address: index/tag/offset: " << Address::indexSize << "/" << Address::tagSize << "/" << Address::offsetSize << endl;
    cout << "Instruction count: " << Cpu::instructionCount << endl;
    cout << "Read hits:         " << Cache::readHit << endl;
    cout << "Read misses:       " << Cache::readMiss << endl;
    cout << "Read miss rate:    " << std::fixed << std::setprecision(2)
         << 100.0*Cache::readMiss / (Cache::readHit + Cache::readMiss) << "%" << endl;
    cout << "Write hits:        " << Cache::writeHit << endl;
    cout << "Write misses:      " << Cache::writeMiss << endl;
    cout << "Write miss rate:   " << std::fixed << std::setprecision(2)
         << 100.0*Cache::writeMiss / (Cache::writeHit + Cache::writeMiss) << "%" << endl;
}


void emulateDaxpy(){
    // emulate c = a*D + b
    // construct array of address
    std::vector<Address> a(dimension), b(dimension), c(dimension);
    int address = 0;
    for(int i=0; i<dimension; ++i){
        a[i] = Address(address);
        b[i] = Address(sz*dimension+address);
        c[i] = Address(2*sz*dimension+address);
        address += sz;
    }

    // insert some value in ram
    int value = 1;
    for(int i=0; i<dimension; ++i){
        ram->setDouble(a[i], value);
        ram->setDouble(b[i], 2*value);
        ram->setDouble(c[i], 0);
    }
    cout << "Value initialized. " << endl;

    // put a random D value in register
    double register0 = 3, register1, register2, register3, register4;

    // Run the daxpy
    for(int i=0; i<dimension; ++i){
        register1 = cpu->loadDouble(Address(a[i]));
        register2 = cpu->Cpu::multDouble(register0, register1);
        register3 = cpu->loadDouble(Address(b[i]));
        register4 = cpu->Cpu::addDouble(register2, register3);
        cpu ->storeDouble(Address(c[i]), register4);
    }

    // verify result:
    for(int i=0; i<dimension; ++i){
        assert(ram->getDouble(c[i]) == register0*value + 2*value);
    }
}



void constructMatrix(std::vector<std::vector<Address>>& a, std::vector<std::vector<Address>>& b,
                     std::vector<std::vector<Address>>& c){

    int address = 0;
    for(int i=0; i<dimension; ++i){
        for(int j=0; j<dimension; ++j){
            a[i][j] = Address(address);
            b[i][j] = Address(sz*dimension*dimension + address);
            c[i][j] = Address(2*sz*dimension*dimension + address);
            address += sz;
        }
    }

    // insert some value in ram
    int value = 1;
    for(int i=0; i<dimension; ++i){
        for(int j=0; j<dimension; ++j){
            ram->setDouble(a[i][j], value);
            ram->setDouble(b[i][j], value);
            ram->setDouble(c[i][j], 0);
            // increase value if necessary
        }
    }
    cout << "Value initialized. " << endl;
}


void emulateMxm(){

    // emulate C = A * B
    // construct matrix of address
    std::vector<std::vector<Address>>
    a(dimension, std::vector<Address>(dimension)),
    b(dimension, std::vector<Address>(dimension)),
    c(dimension, std::vector<Address>(dimension));

    constructMatrix(a, b, c);

    // run naive mxm
    double register1, register2, register3, register4;
    for(int i=0; i<dimension; ++i){
        for(int j=0; j<dimension; ++j){
            register1 = 0.0;
            for(int k=0; k<dimension; ++k){
                register2 = cpu->loadDouble(a[i][k]);
                register3 = cpu->loadDouble(b[k][j]);
                register4 = cpu->multDouble(register2, register3);
                register1 = cpu->addDouble(register1, register4);
            }
            cpu->storeDouble(c[i][j], register1);
        }
    }

    for(int i=0; i<dimension; ++i){
        for(int j=0; j<dimension; ++j){
            assert(ram->getDouble(c[i][j]) == dimension);
        }
    }

    cout << "Mxm finished. " << endl;

}

void emulateMxmBlock(){



    // emulate C = A * B
    // construct matrix of address
    std::vector<std::vector<Address>>
            a(dimension, std::vector<Address>(dimension)),
            b(dimension, std::vector<Address>(dimension)),
            c(dimension, std::vector<Address>(dimension));

    constructMatrix(a, b, c);

    // run block mxm
    double register1, register2, register3, register4, tmp;
    int i, j, k, jj, kk;
    for(jj=0; jj<dimension; jj+=blockingFactor){
        for(kk=0; kk<dimension; kk+=blockingFactor){
            for(i=0; i<dimension; ++i){
                for(j=jj; j<min(jj+blockingFactor, dimension); ++j){
                    register1 = cpu->loadDouble(c[i][j]);  // Load directly to register1
//                    if((int)register1%32!=0 ){
//                        int v1 = cpu->loadDouble(c[i][j]);
//                        int v2 = ram->getDouble(c[i][j]);
//                        cout << v1 << " " << v2 << endl;
//                    }
                    for(k=kk; k<min(kk+blockingFactor, dimension); ++k){
                        register2 = cpu->loadDouble(a[i][k]);
                        register3 = cpu->loadDouble(b[k][j]);
                        register4 = cpu->multDouble(register2, register3);
                        register1 = cpu->addDouble(register1, register4);
                    }
                    cpu->storeDouble(c[i][j], register1);  // Store from register1
//                    if((int)register1%32!=0  || (int)(ram->getDouble(c[i][j]))%32!=0 ){
//                        cout << register1 << endl;
//                    }
                }
            }
        }
    }


    for(i=0; i<dimension; ++i){
        for(j=0; j<dimension; ++j){
            assert(ram->getDouble(c[i][j]) == dimension);
//            if(ram->getDouble(c[i][j])!=dimension){
//                cout << ram->getDouble(c[i][j]) << endl;
//            }
        }
    }

    cout << "Mxm_block finished. " << endl;
}


int main(int argc, char** argv) {

    parseInput(argc, argv);

    initializeEmulator();

    printInput();

    switch (algorithm) {
        case AlgorithmPolicy::daxpy:
            emulateDaxpy(); break;
        case AlgorithmPolicy::mxm:
            emulateMxm(); break;
        case AlgorithmPolicy::mxm_block:
            emulateMxmBlock(); break;
    }

    if(printEnabled){
        printResult();
    }

    return 0;
}



