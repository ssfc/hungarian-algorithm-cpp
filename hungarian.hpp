//
// Created by take_ on 2024/3/9.
//

#ifndef HUNGARIAN_HPP
#define HUNGARIAN_HPP

#include <fstream>
#include <string>


class Hungarian
{
private:

public:

    explicit Hungarian(const std::string& input_filename)
    {
        std::ifstream map_file(input_filename);
    }
};

#endif // HUNGARIAN_HPP
