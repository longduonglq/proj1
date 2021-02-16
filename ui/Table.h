//
// Created by dop on 2/13/21.
//

#ifndef PROJ1_TABLE_H
#define PROJ1_TABLE_H

#include <iostream>
#include <numeric>
#include <algorithm>
#include <vector>
#include <type_traits>
#include "Column.h"
#include "configuration.h"

void tableDemo();

class Table {
public:

    Table(const std::vector<AbstractColumn*>& _columns, std::wstring _title)
    :
    columns {_columns},
    title {_title}
    {
        // Center table
        int tableWidth = std::transform_reduce(
        columns.cbegin(), columns.cend(),
        0,
        std::plus<int> (),
        [](const auto& a) { return a->getColumnWidth(); }
        );
        leftPadding = (config::CONSOLE_WIDTH - tableWidth) / 2;
        rightPadding = leftPadding;
    }

    std::wstring nSpace(int n)
    {
        return std::wstring(n, ' ');
    }

    std::wstring doubleLine(int n)
    {
        return std::wstring(n, L'=');
    }

    std::wstring singleLine(int n)
    {
        return std::wstring(n, '_');
    }

    void dumpTableTo(std::wostream& os)
    {
        os << nSpace(leftPadding)
           << title
           << std::endl;

        os << nSpace(leftPadding)
           << doubleLine(config::CONSOLE_WIDTH - leftPadding - rightPadding)
           << std::endl;

        for (std::size_t i = 0; i <= columns.at(0)->getSize(); i++)
        {
            os << nSpace(leftPadding);
            for (const auto& column : columns)
                column->dumpNext(os);
            os << std::endl;
        }

        for (auto* column : columns)
            column->reset();
    }

    ~Table()
    {
        std::destroy(columns.begin(), columns.end());
        for (AbstractColumn* columnPtr : columns)
            delete columnPtr;
    }

private:
    std::wstring title;
    // Must use polymorphism because Columns
    // might contains items of different types.
    std::vector<AbstractColumn*> columns;
    int leftPadding, rightPadding;
};

#endif //PROJ1_TABLE_H
