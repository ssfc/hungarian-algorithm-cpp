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
    int num_rows;
    int num_columns;
    int square_matrix_size;
    std::vector<std::vector<double>> cost_matrix;

public:
    explicit Hungarian(const std::string& input_filename);

    void hungarian_solve();

    void adjust_matrix_by_min_uncovered_num(std::vector<std::vector<double>>& matrix,
            std::vector<int> covered_rows, std::vector<int> covered_columns);

    // debug func
    void brute_force();
    static void print_double_matrix(const std::vector<std::vector<double>>& input_matrix);
    static void print_bool_matrix(const std::vector<std::vector<bool>>& input_matrix);
};


class CoverZeros
{
private:
    std::vector<std::vector<bool>> zero_locations;

    size_t square_matrix_size;
    std::vector<int> marked_rows;
    std::vector<int> marked_columns;

    std::vector<std::vector<bool>> choices;
    std::vector<int> covered_rows;
    std::vector<int> covered_columns;

public:
    explicit CoverZeros(const std::vector<std::vector<double>>& input_matrix);

    bool compute_min_lines_to_cover_zeros();

    int mark_new_columns_with_zeros_in_marked_rows();

    int mark_new_rows_with_choices_in_marked_columns();

    bool choice_in_all_marked_columns();

    int find_row_without_choice(int choice_column_index);

    std::pair<int, int> find_best_choice_row_and_new_column(int choice_column_index);

    int find_marked_column_without_choice();

    std::vector<int> get_covered_rows();

    std::vector<int> get_covered_columns();

    // debug func
    static bool is_double_equal(double x, double y, double epsilon = 1e-9)
    {
        return std::abs(x - y) < epsilon;
    }

    static void print_int_vector(const std::string& vector_name, const std::vector<int>& input_vector);
};

#endif // HUNGARIAN_HPP
