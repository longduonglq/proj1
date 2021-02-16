//
// Created by dop on 2/13/21.
//
#include "Table.h"
#include "Column.h"

void tableDemo()
{
    /// Create a new column
    auto labelCol = new StringColumn(
        {L"Coefficient of Variation", L"Relative Standard Deviation", L"Skewness"},
        0, 5,
        L"stats"
    );
    auto valueCol = new DoubleColumn(
        {0.58757, 58.75535, -1.2984}, // ELements in the column
        0, 5, // left and right padding. 0 and 5 are usually fine.
        L"values" // label of column
    );
    auto equalCol = new StringColumn(
        std::vector<std::wstring>(3, std::wstring(L"=")),
        0, 5,
        L""
    );

    /// Add all column created to a table
    auto table = Table(
        { labelCol, equalCol, valueCol }, // All column
        L"demo table" // label of table
    );
    // write to cout
    table.dumpTableTo(std::wcout);
    // if need to write to file, simply replace std::cout with std::ofstream.
    // Ex: table.dumpTableTo(std::ofstream("dfsdf"));


    {
        auto valueCol = new IntColumn(
            {12, 65, 92},
            0, 5,
            L"Value"
        );
        auto freqCol = new IntColumn(
            std::vector<int>(3, 1),
            0, 5,
            L"Frequency"
        );
        auto freqPercentCol = new DoubleColumn(
            {25.00, 25.00, 25.00},
            0, 5,
            L"Frequency %"
        );

        auto table = Table(
            { valueCol, freqCol, freqPercentCol },
            L"Frequency Table");

        table.dumpTableTo(std::wcout);
    }
}