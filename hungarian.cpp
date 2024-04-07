//
// Created by take_ on 2024/3/9.
//

#include <cfloat>
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

    while(total_covered < square_matrix_size && iter_step3 < 1)
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

        cout << "transformed_matrix: " << endl;
        for(int i=0;i<transformed_matrix.size();i++)
        {
            for(int j=0;j<transformed_matrix[i].size();j++)
            {
                cout << transformed_matrix[i][j] << " ";
            }
            cout << endl;
        }

        iter_step3++;
    }

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


CoverZeros::CoverZeros(const std::vector<std::vector<double>> &input_matrix)
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
