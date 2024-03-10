//
// Created by take_ on 2024/3/9.
//

#ifndef HUNGARIAN_HPP
#define HUNGARIAN_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>


class Hungarian
{
private:
    int square_matrix_size;
    std::vector<std::vector<double>> cost_matrix;

public:
    explicit Hungarian(const std::string& input_filename);

    void hungarian_solve();

    void brute_force();

    // debug func
    static void print_double_matrix(const std::vector<std::vector<double>>& input_matrix);

};


class CoverZeros
{
private:
    size_t square_matrix_size;
    std::vector<int> marked_rows;
    std::vector<int> marked_columns;

    std::vector<std::vector<bool>> choices;

public:
    explicit CoverZeros(const std::vector<std::vector<double>>& input_matrix);

    bool compute_min_lines_to_cover_zeros();

    // debug func
    bool is_double_equal(double x, double y, double epsilon = 1e-9)
    {
        return std::abs(x - y) < epsilon;
    }

    void print_int_vector(const std::vector<int>& input_vector);
};

#endif // HUNGARIAN_HPP
