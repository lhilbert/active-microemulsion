//
// Created by tommaso on 21/11/18.
//

#include "RandomGenerator.h"

//thread_local std::mt19937 RandomGenerator::rng(std::random_device{}());
std::mt19937 RandomGenerator::rng(std::random_device{}());
std::mt19937_64 RandomGenerator::rng64(std::random_device{}());

RandomGenerator &RandomGenerator::getInstance()
{
    static RandomGenerator instance;
    return instance;
}

std::mt19937 &RandomGenerator::getGenerator()
{
    return rng;
}

std::mt19937_64 &RandomGenerator::getGenerator64()
{
    return rng64;
}

RandomGenerator::RandomGenerator()
{
    seedEngines();
}

void RandomGenerator::seedEngines()
{
    //rng.seed(boost::random::random_device{}()); // This should be a true PRNG
    rng.seed(std::random_device{}()); // This is not a true PRNG
    rng64.seed(std::random_device{}());
}
