//
// Created by take_ on 2024/3/9.
//

#include <cfloat>
#include <cmath>
#include <tuple>
#include "hungarian.hpp"

using namespace std;


Hungarian::Hungarian(const std::string &input_filename):
    num_rows(0),
    num_columns(0),
    square_matrix_size(0),
    epsilon(1e-9)
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


double Hungarian::hungarian_solve()
{
    std::vector<std::vector<double>> transformed_matrix = cost_matrix;

    // Step 1: Subtract row mins from each row.
    for(auto& row : transformed_matrix)
    {  // 遍历每一行
        double row_min_value = *std::min_element(row.begin(), row.end());  // 找到当前行的最小值
        std::transform(row.begin(), row.end(), row.begin(),
                       [row_min_value](double x) { return x - row_min_value; });  // 从每一行中减去最小值
    }

    // check after step 1.
    // print_double_matrix(transformed_matrix);

    // Step 2: Subtract column mins from each column.
    for (size_t column_index = 0; column_index < square_matrix_size; column_index++)
    {
        double column_min_value = transformed_matrix[0][column_index];
        for (size_t row_index = 1; row_index < square_matrix_size; row_index++)
        {
            column_min_value = std::min(column_min_value, transformed_matrix[row_index][column_index]);
        }

        for (auto &each_row : transformed_matrix)
        {
            each_row[column_index] -= column_min_value;
        }
    }

    // check after step 2.
    // print_double_matrix(transformed_matrix);

    // Step 3: Use minimum number of lines to cover all zeros in the matrix.
    // 覆盖线有可能是竖线也有可能是横线
    int iter_step3 = 0;
    size_t total_covered = 0;

    while(total_covered < square_matrix_size)
    // while(total_covered < square_matrix_size && iter_step3 < 3)
    {
        cout << "iter_step3: " << iter_step3 << endl;

        CoverZeros cover_zeros(transformed_matrix);
        auto covered_rows = cover_zeros.get_covered_rows();  // 这是一个list
        CoverZeros::print_int_vector("covered_rows", covered_rows);
        auto covered_columns = cover_zeros.get_covered_columns();  // 这也是一个list
        CoverZeros::print_int_vector("covered columns", covered_columns);
        total_covered = covered_rows.size() + covered_columns.size();
        cout << "total covered: " << total_covered << endl;

        if(total_covered < square_matrix_size)
        {
            adjust_matrix_by_min_uncovered_num(transformed_matrix, covered_rows, covered_columns);
        }

        print_double_matrix("transformed_matrix", transformed_matrix);

        iter_step3++;
    }

    // Step 4: Starting with the top row, work your way downwards as you make assignments.
    // Find single zeros in rows or columns.
    // Add them to final result and remove them and their associated row/column from the matrix.
    auto expected_results = std::min(num_columns, num_rows);
    std::vector<std::vector<bool>> zero_locations;
    zero_locations.resize(transformed_matrix.size());
    for (size_t i = 0; i < transformed_matrix.size(); ++i)
    {
        // Resize the inner vector to match size of the inner transformed_matrix vector
        zero_locations[i].resize(transformed_matrix[i].size());

        for (size_t j = 0; j < transformed_matrix[i].size(); ++j)
        {
            // Check if the value is close enough to zero to be considered zero
            zero_locations[i][j] = std::fabs(transformed_matrix[i][j]) < epsilon;
        }
    }
    print_bool_matrix("zero_locations", zero_locations);

    int iter_step4 = 0;
    while(work_assignment.size() != expected_results)
    // while(work_assignment.size() != expected_results && iter_step4 < 2)
    {
        cout << "iter_step4: " << iter_step4 << endl;
        bool exist_zero = false;
        for (const auto& row : zero_locations)
        {
            for (bool zero : row)
            {
                if (zero)
                {
                    exist_zero = true; // Found a zero, no need to check further.
                    break;
                }
            }
        }

        if(!exist_zero)
        {
            throw std::runtime_error("Unable to find results. Algorithm has failed.");
        }

        auto temp = find_matches(zero_locations);
        auto matched_rows = temp.first;
        auto matched_columns = temp.second;

        CoverZeros::print_int_vector("matched_rows", matched_rows);
        CoverZeros::print_int_vector("matched_columns", matched_columns);

        // Make arbitrary selection
        auto total_matched = matched_rows.size() + matched_columns.size();
        if(total_matched == 0)
        {
            auto temp = select_arbitrary_match(zero_locations);
            matched_rows = vector<int>{temp.first};
            matched_columns = vector<int>{temp.second};
        }

        // 测试select_arbitrary_match的正确性
        // auto result = select_arbitrary_match(zero_locations);
        // cout << "result:" << result.first << " " << result.second << endl;

        // Delete rows
        for (int row : matched_rows)
        {
            // Assuming all rows have the same size, which should be the case in any proper matrix
            std::fill(zero_locations[row].begin(), zero_locations[row].end(), false);
        }

        // Delete columns
        for (int column : matched_columns)
        {
            for (std::vector<bool>& row : zero_locations)
            {
                row[column] = false;
            }
        }

        // print_bool_matrix("zero_locations after delete matched:\n", zero_locations);

        std::vector<std::pair<int, int>> zipped_matched;
        // Ensure that both vectors have the same size
        if(matched_rows.size() != matched_columns.size())
        {
            // Handle the error, perhaps throw an exception
            throw std::invalid_argument("matched_rows and matched_columns must be the same size");
        }

        for (size_t i = 0; i < matched_rows.size(); ++i)
        {
            zipped_matched.emplace_back(matched_rows[i], matched_columns[i]);
        }

        set_results(zipped_matched);

        iter_step4++;
    }

    // Calculate total potential
    double total_cost = 0;
    for(auto element : work_assignment)
    {
        total_cost += cost_matrix[element.first][element.second];
    }

    cout << "total cost: " << total_cost << endl;

    return total_cost;
}


void Hungarian::adjust_matrix_by_min_uncovered_num(std::vector<std::vector<double>>& matrix,
                           std::vector<int> covered_rows, std::vector<int> covered_columns)
{
    // Calculate minimum uncovered number (min_uncovered_num)
    auto min_uncovered_num = DBL_MAX;
    for (size_t row_index = 0; row_index < matrix.size(); ++row_index)
    {
        for (size_t column_index = 0; column_index < matrix[row_index].size(); ++column_index)
        {
            bool is_covered_row = std::find(covered_rows.begin(), covered_rows.end(), row_index) != covered_rows.end();
            bool is_covered_column = std::find(covered_columns.begin(), covered_columns.end(), column_index) != covered_columns.end();

            if (!is_covered_row && !is_covered_column)
            {
                min_uncovered_num = std::min(min_uncovered_num, matrix[row_index][column_index]);
            }
        }
    }

    cout << "min_uncovered_num: " << min_uncovered_num << endl;

    // Add min_uncovered_num to every element in covered rows
    for (int row : covered_rows)
    {
        for (size_t column_index = 0; column_index < matrix[row].size(); ++column_index)
        {
            // Only add to elements not in covered columns
            matrix[row][column_index] += min_uncovered_num;
        }
    }

    // Add min_uncovered_num to every element in covered columns
    for (int column : covered_columns)
    {
        for (size_t row_index = 0; row_index < matrix.size(); ++row_index)
        {
            // Only add to elements not in covered rows
            matrix[row_index][column] += min_uncovered_num;
        }
    }

    // Subtract min_uncovered_num from element
    for (size_t row_index = 0; row_index < matrix.size(); ++row_index)
    {
        for (size_t column_index = 0; column_index < matrix[row_index].size(); ++column_index)
        {
            matrix[row_index][column_index] -= min_uncovered_num;
        }
    }
}


std::pair<std::vector<int>, std::vector<int>> Hungarian::find_matches(const std::vector<std::vector<bool>>& zero_locations)
{
    vector<int> marked_rows;
    vector<int> marked_columns;

    // Mark rows and columns with matches
    // Iterate over rows
    // We'll iterate over the rows of zero_locations
    for (size_t index = 0; index < zero_locations.size(); ++index)
    {
        const auto& row = zero_locations[index];

        // Count the number of `true` values in the current row which represent zeros in original context
        auto sum_of_row = std::count(row.begin(), row.end(), true);

        if (sum_of_row == 1)
        {
            // Find the index of the only `true` value in the row which stands for the column index
            auto it = std::find(row.begin(), row.end(), true);
            if (it != row.end())
            {
                int column_index = std::distance(row.begin(), it);

                // mark_rows_and_columns may look a bit different in C++ compared to Python since we would
                // modify vectors by reference and not return new ones most likely, so let's use that approach here.
                std::tie(marked_rows, marked_columns) = mark_rows_and_columns(
                        marked_rows,
                        marked_columns,
                        static_cast<int>(index), // row_index
                        column_index);
            }
        }
    }

    // Iterate over columns
    for (size_t columnIndex = 0; columnIndex < zero_locations[0].size(); ++columnIndex)
    {
        int sum_of_column = 0;
        int rowIndexOfSingleZero = -1; // This will store the index of the single 'True' if found

        // Sum up the 'True' values in the column and find the row index if there is only one 'True'
        for (size_t rowIndex = 0; rowIndex < zero_locations.size(); ++rowIndex)
        {
            if (zero_locations[rowIndex][columnIndex])
            {
                ++sum_of_column;
                rowIndexOfSingleZero = rowIndex;
            }
        }

        // If there is exactly one 'True' in this column
        if (sum_of_column == 1)
        {
            std::tie(marked_rows, marked_columns) = mark_rows_and_columns(
                    marked_rows,
                    marked_columns,
                    rowIndexOfSingleZero, // Row index of the single 'True'
                    static_cast<int>(columnIndex)); // Column index
        }
    }

    return std::make_pair(marked_rows, marked_columns);
}


std::pair<std::vector<int>, std::vector<int>> Hungarian::mark_rows_and_columns(
        const std::vector<int>& marked_rows,
        const std::vector<int>& marked_columns,
        int row_index,
        int column_index)
{
    // Create new vectors from the existing ones
    std::vector<int> new_marked_rows = marked_rows;
    std::vector<int> new_marked_columns = marked_columns;

    // If row_index is not in marked_rows and column_index is not in marked_columns, mark them
    if (std::find(marked_rows.begin(), marked_rows.end(), row_index) == marked_rows.end() &&
        std::find(marked_columns.begin(), marked_columns.end(), column_index) == marked_columns.end()) {
        new_marked_rows.push_back(row_index);
        new_marked_columns.push_back(column_index);
    }

    // Return the updated lists of marked rows and columns as a pair
    return std::make_pair(new_marked_rows, new_marked_columns);
}


std::pair<int, int> Hungarian::select_arbitrary_match(
        const std::vector<std::vector<bool>>& zero_locations)
{
    // Set up a vector to keep track of the counts
    std::vector<int> zero_count;
    std::vector<std::pair<int, int>> coordinates;

    // Iterate over every element and count zeros in the corresponding rows and columns
    for (size_t i = 0; i < zero_locations.size(); ++i)
    {
        for (size_t j = 0; j < zero_locations[i].size(); ++j)
        {
            if (zero_locations[i][j])
            {
                int row_count = 0;
                int column_count = 0;

                // Count zeros in the row
                for (size_t k = 0; k < zero_locations[i].size(); ++k)
                {
                    if (zero_locations[i][k]) row_count++;
                }

                // Count zeros in the column
                for (size_t k = 0; k < zero_locations.size(); ++k)
                {
                    if (zero_locations[k][j]) column_count++;
                }

                // Add the sum of row and column zeros to the counts
                zero_count.push_back(row_count + column_count);
                coordinates.push_back({i, j}); // Store the coordinates as well
            }
        }
    }

    // Find the row-column combination with the fewest zeros
    auto it = std::min_element(zero_count.begin(), zero_count.end());

    // If there are no zeros, return an error state or handle appropriately
    if (it == zero_count.end())
    {
        // Throw an error or handle the situation where no matching zero is found
        throw std::runtime_error("No zero found to select.");
    }

    // The coordinates corresponding to the smallest count
    size_t index = std::distance(zero_count.begin(), it);
    int row = coordinates[index].first;
    int column = coordinates[index].second;

    return {row, column};
}


void Hungarian::set_results(const std::vector<std::pair<int, int>>& result_pairs)
{
    // Iterate through result pairs and add them to the assignment list
    for (const auto& result : result_pairs)
    {
        int row = result.first;
        int column = result.second;

        // Check if the row and column indices are within the bounds of the original matrix
        if (row < num_rows && column < num_columns)
        {
            work_assignment.push_back(std::make_pair(row, column));
        }
    }
}


std::vector<std::pair<int, int>> Hungarian::get_work_assignment()
{
    return work_assignment;
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


void Hungarian::print_double_matrix(const std::string& matrix_name, const std::vector<std::vector<double>>& input_matrix)
{
    cout << matrix_name << ":\n";
    for (auto & each_row : input_matrix)
    {
        for (double each_element : each_row)
        {
            std::cout << each_element << " ";
        }

        std::cout << std::endl;
    }
}


void Hungarian::print_bool_matrix(const string& matrix_name, const std::vector<std::vector<bool>>& input_matrix)
{
    cout << matrix_name <<":\n";
    for (auto & each_row : input_matrix)
    {
        for (double each_element : each_row)
        {
            std::cout << each_element << " ";
        }

        std::cout << std::endl;
    }
}


CoverZeros::CoverZeros(const std::vector<std::vector<double>> &input_matrix):
epsilon(1e-9)
{
    // 找到输入矩阵中0的位置
    zero_locations.resize(input_matrix.size());
    for (size_t i = 0; i < input_matrix.size(); i++)
    {
        zero_locations[i].resize(input_matrix[i].size());
        for (size_t j = 0; j < input_matrix[i].size(); j++)
        {
            if (is_double_equal(input_matrix[i][j], 0))
            {
                zero_locations[i][j] = true;
            }
            else
            {
                zero_locations[i][j] = false;
            }
        }
    }

    square_matrix_size = input_matrix.size();

    // Choices starts without any choices made.
    choices.resize(input_matrix.size());
    for (size_t i = 0; i < input_matrix.size(); i++)
    {
        choices[i].resize(input_matrix[i].size(), false);
    }

    compute_min_lines_to_cover_zeros();

    // Draw lines through all unmarked rows and all marked columns.
    // 创建一个包含所有行索引的向量
    std::vector<int> all_rows(square_matrix_size);
    for (int i = 0; i < square_matrix_size; ++i)
    {
        all_rows[i] = i;
    }

    // 将_marked_rows排序，std::set_difference要求有序输入
    std::sort(marked_rows.begin(), marked_rows.end());

    // 计算差集
    std::vector<int> covered_rows_temp;

    std::set_difference(all_rows.begin(), all_rows.end(),
                        marked_rows.begin(), marked_rows.end(),
                        std::back_inserter(covered_rows_temp));

    // 移动到成员变量
    covered_rows = std::move(covered_rows_temp);
    // print_int_vector("covered_rows", covered_rows);

    covered_columns = marked_columns;
}


bool CoverZeros::compute_min_lines_to_cover_zeros()
{
    int iter_line = 0;
    // cout << "compute_min_lines" << endl;
    while(true)
    {
        cout << endl << "iter_line: " << iter_line << endl;

        // Erase all marks.
        marked_rows.clear();
        marked_columns.clear();

        // Mark all rows in which no choice has been made.
        // 如果一行中没有任何非零元素（即该行所有元素都为0或假），则将该行的索引添加到self._marked_rows这个列表中。
        for(size_t i = 0; i < choices.size(); i++)
        {
            if(std::none_of(choices[i].begin(), choices[i].end(), [](bool x){return x;}))
            {
                marked_rows.push_back(i);
            }
        }

        print_int_vector("marked_rows", marked_rows);

        // If no marked rows then finish.
        if (marked_rows.empty())
        {
            return true;
        }

        // Mark all columns not already marked which have zeros in marked rows.
        int num_marked_columns = mark_new_columns_with_zeros_in_marked_rows();
        cout << "num_marked_columns:" << num_marked_columns << endl;
        print_int_vector("marked_columns", marked_columns);

        // If no new marked columns then finish.
        if(num_marked_columns == 0)
        {
            cout << "no new marked columns" << endl;

            return true;
        }

        // While there is some choice in every marked column.
        int iter_choice = 0;
        while(choice_in_all_marked_columns())
        // while(choice_in_all_marked_columns() && iter_choice < 1)
        {
            cout << "iter_choice: " << iter_choice << endl;

            // Some Choice in every marked column.
            // Mark all rows not already marked which have choices in marked columns.
            int num_marked_rows = mark_new_rows_with_choices_in_marked_columns();

            // If no new marks then Finish.
            if(num_marked_rows == 0)
            {
                cout << "no new marked rows" << endl;

                return true;
            }

            // Mark all columns not already marked which have zeros in marked rows.
            num_marked_columns = mark_new_columns_with_zeros_in_marked_rows();

            // If no new marked columns then finish.
            if(num_marked_columns == 0)
            {
                cout << "no new marked columns" << endl;

                return true;
            }

            iter_choice++;
        }
        cout << "iter_choice stop at: " << iter_choice << endl;

        // No choice in one or more marked columns.
        // Find a marked column that does not have a choice.
        int choice_column_index = find_marked_column_without_choice();

        int iter_choice_column_index = 0;
        while(choice_column_index != -1)
        // while(choice_column_index != -1 && iter_choice_column_index < 2)
        {
            cout << "iter_choice_column_index: " << iter_choice_column_index << endl;

            // Find a zero in the column indexed that does not have a row with a choice.
            int choice_row_index = find_row_without_choice(choice_column_index);
            cout << "choice_row_index: " << choice_row_index << endl;

            // Check if an available row was found.
            int new_choice_column_index = -1;
            if(choice_row_index == -1)
            {
                cout << "choice_row_index is None" << endl;
                // Find a good row to accommodate swap. Find its column pair.
                auto temp = find_best_choice_row_and_new_column(choice_column_index);
                choice_row_index = temp.first;
                new_choice_column_index = temp.second;

                // Delete old choice.
                choices[choice_row_index][new_choice_column_index] = false;
            }

            // Set zero to choice.
            choices[choice_row_index][choice_column_index] = true;

            // # Loop again if choice is added to a row with a choice already in it.
            choice_column_index = new_choice_column_index;
            cout << "choice_column_index: " << choice_column_index << endl;
            iter_choice_column_index += 1;
        }
        cout << "iter_choice_column_index stop at:" << iter_choice_column_index << endl;

        iter_line++;
    }

    cout << "iter_line stop at: " << iter_line << endl;
}


int CoverZeros::mark_new_columns_with_zeros_in_marked_rows()
{
    int num_marked_columns = 0;
    std::set<int> marked_rows_set(marked_rows.begin(), marked_rows.end()); // 初始把所有的已标记行放入一个set，方便查询

    for(size_t column_index = 0; column_index < zero_locations[0].size(); ++column_index) // 遍历每一列
    {
        // 判断该列是否已经被标记
        if(std::find(marked_columns.begin(), marked_columns.end(), column_index) != marked_columns.end())
        {
            continue;
        }

        // 检测这一列是否在已标记的行中存在0
        std::set<int> rows_with_zero;
        for(size_t i = 0; i < zero_locations.size(); ++i)
        {
            if(zero_locations[i][column_index])
            {
                rows_with_zero.insert(i);
            }
        }

        // 如果这一列在已标记行中存在0，就把这一列标记起来
        std::set<int> intersect_rows;
        std::set_intersection(
                rows_with_zero.begin(), rows_with_zero.end(),
                marked_rows_set.begin(), marked_rows_set.end(),
                std::inserter(intersect_rows, intersect_rows.begin())
        );

        if(!intersect_rows.empty())
        {
            marked_columns.push_back(column_index);
            ++num_marked_columns;
        }
    }

    return num_marked_columns;
}


int CoverZeros::mark_new_rows_with_choices_in_marked_columns()
{
    int num_marked_rows = 0;
    for(size_t i = 0; i < choices.size(); ++i)
    { // 遍历每一行
        // 判断该行是否已经被标记
        if(std::find(marked_rows.begin(), marked_rows.end(), i) != marked_rows.end())
        {
            continue;
        }

        // 检测这一行是否在已标记的列中存在选择
        for(size_t j = 0; j < choices[i].size(); ++j)
        {
            if(choices[i][j] && (std::find(marked_columns.begin(), marked_columns.end(), j) != marked_columns.end()))
            {
                marked_rows.push_back(i);
                ++num_marked_rows;

                break;
            }
        }
    }

    return num_marked_rows;
}


bool CoverZeros::choice_in_all_marked_columns()
{
    bool choice_in_all_marked_columns = true;

    for (auto column_index: marked_columns)
    {
        if (std::none_of(choices.begin(), choices.end(),
                         [column_index](const std::vector<bool>& row){ return row[column_index]; }))
        {
            choice_in_all_marked_columns = false;

            break;
        }
    }

    return choice_in_all_marked_columns;
}


int CoverZeros::find_row_without_choice(int choice_column_index)
{
    int row_without_choice_index = -1;
    for(size_t i = 0; i < zero_locations.size(); ++i){ // 遍历每一行

        // 判断在指定列是否存在0，并且这行里没有选择
        if(zero_locations[i][choice_column_index] && std::none_of(choices[i].begin(), choices[i].end(), [](bool b){ return b; }))
        {
            row_without_choice_index = i;

            break;
        }
    }

    return row_without_choice_index;
}


std::pair<int, int> CoverZeros::find_best_choice_row_and_new_column(int choice_column_index)
{
    std::vector<size_t> row_indices;
    // Find row indices where zero_locations is true for choice_column_index
    for (size_t i = 0; i < square_matrix_size; i++)
    {
        if (zero_locations[i][choice_column_index])
            row_indices.push_back(i);
    }

    for (auto row_index : row_indices)
    {
        for (size_t column_index = 0; column_index < square_matrix_size; column_index++)
        {
            if (choices[row_index][column_index] && find_row_without_choice(column_index) != -1)
            {
                return {row_index, column_index};
            }
        }
    }

    // Cannot find optimal row and column. Return a random row and column.
    size_t row = row_indices[0];

    std::vector<size_t> column_indices;
    for (size_t column_index = 0; column_index < square_matrix_size; column_index++)
    {
        if (choices[row][column_index])
            column_indices.push_back(column_index);
    }

    return {row, column_indices[0]};
}


int CoverZeros::find_marked_column_without_choice()
{
    int marked_column_without_choice_index = -1;
    for (auto column_index: marked_columns)
    {
        if (std::all_of(choices.begin(), choices.end(),
                        [column_index](const std::vector<bool>& row){ return !row[column_index]; }))
        {
            marked_column_without_choice_index = column_index;

            break;
        }
    }

    if(marked_column_without_choice_index == -1)
    {
        throw std::runtime_error("Could not find a column without a choice. Failed to cover matrix zeros. Algorithm has failed.");
    }

    return marked_column_without_choice_index;
}


std::vector<int> CoverZeros::get_covered_rows()
{
    return covered_rows;
}


std::vector<int> CoverZeros::get_covered_columns()
{
    return covered_columns;
}


void CoverZeros::print_int_vector(const std::string& vector_name, const std::vector<int>& input_vector)
{
    cout << vector_name << ": ";
    for(auto element : input_vector)
    {
        cout << element << " ";
    }
    cout << endl;
}
