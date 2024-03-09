//
// Created by take_ on 2024/3/9.
//

#ifndef HUNGARIAN_HPP
#define HUNGARIAN_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>


class Hungarian
{
private:
    int num_rows;
    int num_columns;
    std::vector<std::vector<double>> cost_matrix;

public:
    explicit Hungarian(const std::string& input_filename);

};

#endif // HUNGARIAN_HPP
