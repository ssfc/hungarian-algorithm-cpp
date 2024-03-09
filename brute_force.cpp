//
// Created by take_ on 2024/3/9.
//

#include <iostream>
#include <algorithm>
#include <climits>
#include <vector>

using namespace std;


int main()
{
    int costMatrix[3][3] = {{4, 2, 8}, {4, 3, 7}, {3, 1, 6}};
    vector<int> job_assignment = {0, 1, 2};

    int min_cost = INT_MAX;
    vector<int> best_job_assignment;

    do
    {
        int current_cost = 0;
        for(int i = 0; i < 3; i++)
        {
            current_cost += costMatrix[i][job_assignment[i]];
        }

        if(current_cost < min_cost)
        {
            min_cost = current_cost;
            best_job_assignment = job_assignment;
        }
    } while(std::next_permutation(job_assignment.begin(), job_assignment.end()));

    std::cout << "min cost: " << min_cost << std::endl;
    std::cout << "assign: " << std::endl;
    for(int i = 0; i < 3; i++)
    {
        std::cout << "worker " << i << " assignment " << best_job_assignment[i] << std::endl;
    }


    return 0;
}


