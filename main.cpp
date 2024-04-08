//
// Created by take_ on 2024/3/9.
//

#include "hungarian.hpp"

#include <boost/program_options.hpp>


using namespace std;


int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    // Declare the supported options.
    po::options_description desc("Allowed options");

    desc.add_options()
            ("help", "produce help message")
            ("instance,i", po::value<std::string>()->required(), "input instance file")
            ("output,o", po::value<std::string>()->default_value("../"), "output folder name");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;

        return 1;
    }

    po::notify(vm);

    Hungarian test(vm["instance"].as<std::string>());
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

// Test on windows platform:
// Compile
// build by clion.
// Run
// ./hungarian_assign -i ../instance/100x100_seed_42.txt