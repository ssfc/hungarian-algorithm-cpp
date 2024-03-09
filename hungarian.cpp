//
// Created by take_ on 2024/3/9.
//

#include "hungarian.hpp"

using namespace std;


Hungarian::Hungarian(const std::string &input_filename):
    num_rows(0),
    num_columns(0)
{
    std::ifstream map_file(input_filename);
    if (map_file.is_open())
    {
        map_file >> num_rows >> num_columns;

        cost_matrix.resize(num_rows);
        for(int i=0;i<num_rows;i++)
        {
            cost_matrix[i].resize(num_columns);
        }

        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_columns; j++)
            {
                map_file >> cost_matrix[i][j];
            }
        }

        // test cost input
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_columns; j++)
            {
                std::cout << cost_matrix[i][j] << " ";
            }
            std::cout << std::endl;
        }

        // (0, 3), (2, 4), (3, 0), (5, 2), (1, 5), (4, 1): 第0个工人分配了第3个任务, 第2个工人分配了第4个任务, 以此类推。
        // cout << cost_matrix[0][3] + cost_matrix[2][4] + cost_matrix[3][0] + cost_matrix[5][2] + cost_matrix[1][5]
        // + cost_matrix[4][1] << endl;
        // cout << cost_matrix[3][0] + cost_matrix[4][2] + cost_matrix[0][3] + cost_matrix[2][5] + cost_matrix[5][1]
        //         + cost_matrix[1][4] << endl;
    }
    else
    {
        std::cerr << "Error opening file." << std::endl;
    }
}


void Hungarian::brute_force()
{
    std::vector<int> job_assignment(num_columns);
    for (int i = 0; i < num_columns; ++i)
    {
        job_assignment[i] = i;
    }

    int min_cost = INT_MAX;
    vector<int> best_job_assignment;

    do
    {
        int current_cost = 0;
        for(int i = 0; i < 3; i++)
        {
            current_cost += cost_matrix[i][job_assignment[i]];
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
}

