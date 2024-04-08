//
// Created by take_ on 2024/3/9.
//

#include "hungarian.hpp"

using namespace std;


int main()
{
    // Hungarian test("../example3.txt");
    // Hungarian test("../example4_10x10.txt");
    // Hungarian test("../instance/example5_20x20.txt");
    // Hungarian test("../instance/20x20.txt");
    // Hungarian test("../instance/50x50.txt");
    Hungarian test("../instance/100x100.txt");
    // std::cout << "hello" << std::endl;

    int start_time = clock();
    // cout << "hhhhh" << endl;

    test.hungarian_solve();
    Hungarian::print_pair_vector("work assignment", test.get_work_assignment());

    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    cout << " elapsed time(s): " << elapsed_time;

    // test.brute_force();

    return 0;
}
