//
// Created by take_ on 2024/3/9.
//

#include "hungarian.hpp"



int main()
{
    Hungarian test("../example3.txt");
    // Hungarian test("../example4_10x10.txt");
    // Hungarian test("../example4_10x10.txt");
    // std::cout << "hello" << std::endl;

    test.hungarian_solve();
    Hungarian::print_pair_vector("work assignment", test.get_work_assignment());
    // test.brute_force();

    return 0;
}
