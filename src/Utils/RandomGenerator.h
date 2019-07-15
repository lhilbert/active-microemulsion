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
// Testing PCG generator: more info at http://www.pcg-random.org/
// #include <pcg_random.hpp>

// static pcg32 fooRNG; // This is just a hack to get icpc to not complain about pcg* types being incomplete...
// static pcg64 fooRNG64;

class RandomGenerator
{
private:
    static std::mt19937 rng;
//    static pcg32 rng;
    #pragma omp threadprivate(rng)
    static std::mt19937_64 rng64;
//    static pcg64 rng64;
    #pragma omp threadprivate(rng64)

public:
    static RandomGenerator& getInstance();
    
    std::mt19937& getGenerator();
    
    std::mt19937_64& getGenerator64();

private:
    RandomGenerator();

    static void seedEngines();
public:
    RandomGenerator(RandomGenerator const&) = delete;
    void operator=(RandomGenerator const&) = delete;
};

#endif //ACTIVE_MICROEMULSION_RANDOMGENERATOR_H
