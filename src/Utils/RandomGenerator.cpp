//
// Created by tommaso on 21/11/18.
//

#include "RandomGenerator.h"

//thread_local std::mt19937 RandomGenerator::rng(std::random_device{}());
//std::mt19937 RandomGenerator::rng(std::random_device{}());
//std::mt19937_64 RandomGenerator::rng64(std::random_device{}());
pcg32 RandomGenerator::rng(std::random_device{}());
pcg64 RandomGenerator::rng64(std::random_device{}());

RandomGenerator &RandomGenerator::getInstance()
{
    static RandomGenerator instance;
    return instance;
}

pcg32 &RandomGenerator::getGenerator()
{
    return rng;
}

pcg64 &RandomGenerator::getGenerator64()
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
//    rng.seed(std::random_device{}()); // This is not a true PRNG
//    rng64.seed(std::random_device{}());
    pcg_extras::seed_seq_from<std::random_device> seed_source;
    rng.seed(seed_source);
    pcg_extras::seed_seq_from<std::random_device> seed_source64;
    rng64.seed(seed_source64);
}
