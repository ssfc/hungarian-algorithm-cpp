//
// Created by take_ on 2024/3/9.
//

#ifndef HUNGARIAN_HPP
#define HUNGARIAN_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


class Hungarian
{
private:
    int square_matrix_size;
    std::vector<std::vector<int>> cost_matrix;

public:
    explicit Hungarian(const std::string& input_filename);

    void brute_force();

};

#endif // HUNGARIAN_HPP
