//
//  boost_compile_test.cpp
//  
//
//  Created by Lennart Hilbert on 03.08.19.
//

#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

int main()
{
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;
    
    std::for_each(
                  in(std::cin), in(), std::cout << (_1 * 3) << " " );
}
