//
// Created by take_ on 2024/3/9.
//

#include "hungarian.hpp"


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
    }
    else
    {
        std::cerr << "Error opening file." << std::endl;
    }
}

