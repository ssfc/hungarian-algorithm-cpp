//
// Created by take_ on 2024/3/9.
//

#include "hungarian.hpp"



int main()
{
    Hungarian test("../example2.txt");
    // std::cout << "hello" << std::endl;

    test.hungarian_solve();
    // test.brute_force();

    return 0;
}
