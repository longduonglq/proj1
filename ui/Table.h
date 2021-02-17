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
#include <sstream>
#include "Column.h"
#include "configuration.h"

void tableDemo();

class Table {
public:
    int tableWidth;

    Table(const std::vector<AbstractColumn*>& _columns, std::wstring _title)
    :
    columns {_columns},
    title {_title},
    tableWidth {0},
    consoleWidth {config::CONSOLE_WIDTH}
    {
        // Center table
        tableWidth = std::transform_reduce(
        columns.cbegin(), columns.cend(),
        0,
        std::plus<int> (),
        [](const auto& a) { return a->getColumnWidth(); }
        );
        leftPadding = (config::CONSOLE_WIDTH - tableWidth) / 2;
        rightPadding = leftPadding;
    }

    Table(const std::vector<AbstractColumn*>& _columns, std::wstring _title, int _consoleWidth, bool selfCentered=true)
    :
    columns {_columns},
    title {_title},
    tableWidth {0}
    {
        // Center table
        tableWidth = std::transform_reduce(
        columns.cbegin(), columns.cend(),
        0,
        std::plus<int> (),
        [](const auto& a) { return a->getColumnWidth(); }
        );

        if (!selfCentered)
        {
            consoleWidth = tableWidth;
        }
        else
        {
            consoleWidth = _consoleWidth;
        }
        leftPadding = (consoleWidth - tableWidth) / 2;
        rightPadding = leftPadding;
    }

    std::wstring nSpace(int n) const
    {
        return std::wstring(n, ' ');
    }

    std::wstring doubleLine(int n) const
    {
        return std::wstring(n, L'=');
    }

    std::wstring singleLine(int n)
    {
        return std::wstring(n, '_');
    }

    void dumpTableTo(std::wostream& os) const
    {
        if (!title.empty())
        {
            os << nSpace(leftPadding)
               << title
               << std::endl;

            os << nSpace(leftPadding)
               << doubleLine(consoleWidth - leftPadding - rightPadding)
               << std::endl;
        }

        for (std::size_t i = 0; i <= columns.at(0)->getSize(); i++)
        {
            os << nSpace(leftPadding);
            for (auto columnIt = columns.begin(); columnIt != columns.end(); columnIt++)
            {
                auto& column = *columnIt;
                std::wstringstream tmpOs;
                column->dumpNext(tmpOs);
                wchar_t t ;
                while (tmpOs.get(t))
                {
                    if (t != '\n')
                    {
                        os << t;
                    }
                    else
                    {
                        auto previousColumnsWidth =
                        std::transform_reduce(
                            columns.begin(), columnIt,
                            leftPadding,
                            std::plus<>(),
                            [](const auto& col){ return col->getColumnWidth();}
                        );
                        os << t
                            << nSpace(previousColumnsWidth);
                    }
                }
            }
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
    int consoleWidth;
    std::wstring title;
    // Must use polymorphism because Columns
    // might contains items of different types.
    std::vector<AbstractColumn*> columns;
    int leftPadding, rightPadding;
};

#endif //PROJ1_TABLE_H
