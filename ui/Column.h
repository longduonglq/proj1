//
// Created by dop on 2/13/21.
//

#ifndef PROJ1_COLUMN_H
#define PROJ1_COLUMN_H

#include <iostream>
#include <iomanip>
#include "configuration.h"

class AbstractColumn
{
public:
    AbstractColumn() = default;
    virtual ~AbstractColumn() {};

    virtual const int getColumnWidth() const = 0;
    virtual const std::size_t getSize() = 0;
    virtual void dumpNext(std::wostream& os) = 0;
    virtual void reset() = 0;
};


template<typename T, std::size_t charLength(const T&)>
class Column : public AbstractColumn
{
public:
    Column(
        const std::vector<T>& _items,
        int _leftPadding,
        int _rightPadding,
        std::wstring _title
    )
    :
        items {_items},
        leftPadding {_leftPadding},
        rightPadding {_rightPadding},
        title {_title},
        currentIt {items.cbegin()},
        titlePrinted {false},
        maxCharLength {getMaxCharLength()}
    {}

    virtual ~Column() override = default;

    std::wstring nSpace(int n)
    {
        return std::wstring(n, ' ');
    }

    void dumpNext(std::wostream& os) override
    {
        if (currentIt == items.cend())
            return;

        if (!titlePrinted && !title.empty())
        {
            os << nSpace(leftPadding)
               << title
               << nSpace(maxCharLength - title.size() +  rightPadding);
            titlePrinted = true;
            return;
        }

        os << nSpace(leftPadding)
           << *currentIt
           << nSpace(maxCharLength - charLength(*currentIt) + rightPadding);

        currentIt++;
    }

    const int getColumnWidth() const override
    {
        return leftPadding + maxCharLength + rightPadding;
    }

    const std::size_t getSize() override
    {
        return items.size();
    }

    void reset() override
    {
        currentIt = items.cbegin();
        titlePrinted = false;
    }

    void addItem(T item)
    {
        items.push_back(item);
        currentIt = items.cbegin();
        maxCharLength = std::max(maxCharLength, getMaxCharLength());
    }

private:
    std::wstring title;
    std::vector<T> items;
    int leftPadding = 0;
    int rightPadding = 0;
    bool titlePrinted;

    decltype(items.cbegin()) currentIt;

    int maxCharLength;
    int getMaxCharLength()
    {
        std::size_t maxCharLength = 0;
        for (const auto& item : items)
        {
            maxCharLength = std::max(maxCharLength, charLength(item));
        }
        maxCharLength = std::max(maxCharLength, title.size());
        return maxCharLength;
    }
};


/// Create some convenient column type
inline std::size_t strLength(const std::wstring& s)
{
    return s.size();
}
using StringColumn = Column<std::wstring, &strLength>;

inline std::size_t intLength(const int& s)
{
    if (s < 0) return static_cast<std::size_t>(log10(-s) + 1 + 1);
    else if (s == 0) return 1;
    else return static_cast<std::size_t>(log10(s) + 1);
}
using IntColumn = Column<int, &intLength>;

inline std::size_t longLength(const long& s)
{
    if (s < 0) return static_cast<std::size_t>(log10(-s) + 1 + 1);
    else if (s == 0) return 1;
    else return static_cast<std::size_t>(log10(s) + 1);
}
using LongColumn = Column<long, &longLength>;

// Specialize for double
inline std::size_t doubleLength(const double& value)
{
    return intLength(value) + config::FLOAT_NUMBER_DIGITS;
}

template<>
inline void Column<double, &doubleLength>::dumpNext(std::wostream& os)
{
    if (currentIt == items.cend())
        return;

    if (!titlePrinted && !title.empty())
    {
        os << nSpace(leftPadding)
           << title
           << nSpace(maxCharLength - title.size() + rightPadding);
        titlePrinted = true;
        return;
    }

    os << nSpace(leftPadding)
       << std::fixed
       << std::setprecision(config::FLOAT_NUMBER_DIGITS)
       << *currentIt
       << nSpace(maxCharLength - doubleLength(*currentIt) + rightPadding).c_str();

    currentIt++;
}
using DoubleColumn = Column<double, &doubleLength>;

#endif //PROJ1_COLUMN_H
