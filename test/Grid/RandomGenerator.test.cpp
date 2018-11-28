#include "catch.hpp"
#include "fakeit.hpp"
#include "../../src/Utils/RandomGenerator.h"
#include <omp.h>

#define REPEATS 1000000
#define NUM_THREADS 2

using namespace fakeit;

// function that returns correlation coefficient.
double correlationCoefficient(std::vector<int> X, std::vector<int> Y)
{
    
    double sum_X = 0, sum_Y = 0, sum_XY = 0;
    double squareSum_X = 0, squareSum_Y = 0;
    
    int n = static_cast<int>(X.size());
    
    for (int i = 0; i < n; i++)
    {
        // sum of elements of array X.
        sum_X = sum_X + X[i];
        
        // sum of elements of array Y.
        sum_Y = sum_Y + Y[i];
        
        // sum of X[i] * Y[i].
        sum_XY = sum_XY + X[i] * Y[i];
        
        // sum of square of array elements.
        squareSum_X = squareSum_X + X[i] * X[i];
        squareSum_Y = squareSum_Y + Y[i] * Y[i];
    }
    
    // use formula for calculating correlation coefficient.
    double corr = (n * sum_XY - sum_X * sum_Y)
                 / sqrt((n * squareSum_X - sum_X * sum_X)
                        * (n * squareSum_Y - sum_Y * sum_Y));
    
    return corr;
}

TEST_CASE( "RandomGenerator thread locality test", "[RandomGenerator]" )
{
    omp_set_num_threads(NUM_THREADS);
    std::vector<int> results[NUM_THREADS];
    void* genRef[NUM_THREADS];
    
    // Fill data in parallel
    #pragma omp parallel for
    for (int i=0; i<NUM_THREADS; ++i)
    {
        auto rng = RandomGenerator::getInstance().getGenerator();
        genRef[i] = &rng;
    
        int threadId = omp_get_thread_num();
        std::uniform_int_distribution<std::mt19937::result_type> dist(0,32003);
        for (int repeat = 0; repeat < REPEATS; ++repeat)
        {
            int value = static_cast<int>(dist(rng));
            results[threadId].emplace_back(value);
        }
    }
    
    // Now print a sample and then check
    for (int i=0; i<NUM_THREADS; ++i)
    {
        std::cout << "RNG@" << genRef[i] << std::endl;
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            std::cout << "T=" << i << ", repeat=" << repeat << ", value=" << results[i][repeat] << std::endl;
        }
    }
    
    int t = 0;
    int v = results[t][0];
    do {
        ++t;
    } while (results[t][0] == v && t<NUM_THREADS);
    bool isFirstEqual = (t==NUM_THREADS);
    
    int numEquals = 0;
    for (int repeat = 0; repeat < REPEATS; ++repeat)
    {
        int i = 0;
        int v = results[i][repeat];
        do {
            ++i;
        } while (i<NUM_THREADS && results[i][repeat] == v);
        numEquals += (i==NUM_THREADS);
    }
    double correlation = correlationCoefficient(results[0], results[1]);
    
    std::cout << "isFirstEqual=" << isFirstEqual << std::endl;
    std::cout << "numEquals=" << numEquals << std::endl;
    std::cout << "correlation=" << correlation << std::endl;
    
    REQUIRE(!isFirstEqual);
    REQUIRE(numEquals < REPEATS);
    REQUIRE(correlation < 1e-2);
}
