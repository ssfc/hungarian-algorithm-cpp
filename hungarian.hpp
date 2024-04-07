//
// Created by take_ on 2024/3/9.
//

#ifndef HUNGARIAN_HPP
#define HUNGARIAN_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
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
    std::vector<std::vector<bool>> zero_locations;

    size_t square_matrix_size;
    std::vector<int> marked_rows;
    std::vector<int> marked_columns;

    std::vector<std::vector<bool>> choices;

public:
    explicit CoverZeros(const std::vector<std::vector<double>>& input_matrix);

    bool compute_min_lines_to_cover_zeros();

    int mark_new_columns_with_zeros_in_marked_rows();

    int mark_new_rows_with_choices_in_marked_columns();

    bool choice_in_all_marked_columns();

    int find_row_without_choice(int choice_column_index);

    int find_marked_column_without_choice();

    // debug func
    static bool is_double_equal(double x, double y, double epsilon = 1e-9)
    {
        return std::abs(x - y) < epsilon;
    }

    static void print_int_vector(const std::vector<int>& input_vector);
};

#endif // HUNGARIAN_HPP
