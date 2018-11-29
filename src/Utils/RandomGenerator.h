//
// Created by tommaso on 21/11/18.
//

#ifndef ACTIVE_MICROEMULSION_RANDOMGENERATOR_H
#define ACTIVE_MICROEMULSION_RANDOMGENERATOR_H

#include <random>
#include <algorithm>
#include <functional>
#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>

class RandomGenerator
{
private:
//    thread_local static std::mt19937 rng;
    static std::mt19937 rng;
    #pragma omp threadprivate(rng)

public:
    static RandomGenerator& getInstance()
    {
        static RandomGenerator instance;
        return instance;
    }
    
    std::mt19937& getGenerator()
    {
        return rng;
    }

private:
    RandomGenerator()
    {
        seedEngine();
    }

    static void seedEngine()
    {
//        rng.seed(boost::random::random_device{}()); // This should be a true PRNG
        rng.seed(std::random_device{}()); // This is not a true PRNG
    }

public:
    RandomGenerator(RandomGenerator const&) = delete;
    void operator=(RandomGenerator const&) = delete;
};

//thread_local std::mt19937 RandomGenerator::rng(std::random_device{}());
//std::mt19937 RandomGenerator::rng(std::random_device{}());

#endif //ACTIVE_MICROEMULSION_RANDOMGENERATOR_H