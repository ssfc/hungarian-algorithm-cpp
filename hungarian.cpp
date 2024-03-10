//
// Created by take_ on 2024/3/9.
//

#include "hungarian.hpp"

using namespace std;


Hungarian::Hungarian(const std::string &input_filename):
    square_matrix_size(0)
{
    std::ifstream map_file(input_filename);
    if (map_file.is_open())
    {
        int num_rows;
        int num_columns;
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

        if (num_rows != num_columns)
        {
            // 计算需要添加的行数或列数
            int size_diff = std::abs(num_rows - num_columns);
            if (num_rows < num_columns)
            {
                square_matrix_size = num_columns;
                // 如果行数少于列数，添加新的行
                for (int i = 0; i < size_diff; ++i)
                {
                    cost_matrix.emplace_back(num_columns, 0);
                }
            }
            else
            {
                square_matrix_size = num_rows;
                // 如果列数少于行数，为每行添加新的列
                for(auto& each_row : cost_matrix)
                {
                    each_row.insert(each_row.end(), size_diff, 0);
                }
            }
        }
        else
        {
            square_matrix_size = num_rows;
        }

        // test cost input
        /*
        for (int i = 0; i < square_matrix_size; i++)
        {
            for (int j = 0; j < square_matrix_size; j++)
            {
                std::cout << cost_matrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
        //*/

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


void Hungarian::hungarian_solve()
{
    std::vector<std::vector<double>> transformed_matrix = cost_matrix;

    // Step 1: Subtract row mins from each row.
    for(auto& row : transformed_matrix)
    {  // 遍历每一行
        double min_val = *std::min_element(row.begin(), row.end());  // 找到当前行的最小值
        std::transform(row.begin(), row.end(), row.begin(),
                       [min_val](double x) { return x - min_val; });  // 从每一行中减去最小值
    }

    // check after step 1.
    // print_double_matrix(transformed_matrix);
}


void Hungarian::brute_force()
{
    std::vector<int> work_assignment(square_matrix_size);
    for (int i = 0; i < square_matrix_size; ++i)
    {
        work_assignment[i] = i;
    }

    double min_cost = std::numeric_limits<double>::max();
    vector<int> best_work_assignment;

    do
    {
        double current_cost = 0;
        for(int i = 0; i < square_matrix_size; i++)
        {
            current_cost += cost_matrix[i][work_assignment[i]];
        }

        if(current_cost < min_cost)
        {
            min_cost = current_cost;
            best_work_assignment = work_assignment;
        }
    } while(std::next_permutation(work_assignment.begin(), work_assignment.end()));

    std::cout << "min cost: " << min_cost << std::endl;
    std::cout << "work assign: " << std::endl;

    for(int i = 0; i < square_matrix_size; i++)
    {
        std::cout << "worker " << i << " assignment " << best_work_assignment[i] << std::endl;
    }
}


void Hungarian::print_double_matrix(const std::vector<std::vector<double>>& input_matrix)
{
    for (auto & each_row : input_matrix)
    {
        for (double each_element : each_row)
        {
            std::cout << each_element << " ";
        }

        std::cout << std::endl;
    }
}


