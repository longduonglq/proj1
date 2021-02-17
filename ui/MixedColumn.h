//
// Created by dop on 2/15/21.
//

#ifndef PROJ1_MIXEDCOLUMN_H
#define PROJ1_MIXEDCOLUMN_H

#include <iostream>
#include <string>
#include <type_traits>
#include "configuration.h"
#include "Column.h"

/** MixedColumn aims to provide similar functionality
 *  to the original column class with the exception that
 *  it allows the storage of many different type in a column.
 */

//template <typename StringLikeType,
//        typename std::enable_if<std::is_same<StringLikeType, const char*>::value, int>::type = 0>
//std::size_t displayLength(StringLikeType cStr)
//{
//    return strlen(cStr);
//}

std::size_t displayLength(const char* cStr)
{
    return strlen(cStr);
}

//template <typename StringLikeType,
//typename std::enable_if<std::is_same<StringLikeType, const wchar_t*>::value, int>::type = 0>
//std::size_t displayLength(StringLikeType cStr)
//{
//    return wcslen(cStr);
//}

std::size_t displayLength(const wchar_t* cStr)
{
    return wcslen(cStr);
}

//template <typename StringLikeType,
//typename std::enable_if<std::is_same<StringLikeType, std::wstring>::value, int>::type = 0>
//std::size_t displayLength(StringLikeType cStr)
//{
//    return cStr.size();
//}

std::size_t displayLength(std::wstring WideStr)
{
    return WideStr.size();
}

std::size_t displayLength(char)
{
    return 1;
}


template<typename IntegerType,
        typename std::enable_if<std::is_integral<IntegerType>::value, int>::type = 0>
std::size_t displayLength(const IntegerType& var)
{
    if (var < 0) return static_cast<std::size_t>(std::log10(-var) + 1 + 1);
    else if (var == 0) return 1;
    else return static_cast<std::size_t>(std::log10(var) + 1);
}

template<typename FloatType,
         typename std::enable_if<std::is_floating_point<FloatType>::value, int>::type = 0>
std::size_t displayLength(const FloatType& var)
{
    if (long(var) == var)
    {
        return displayLength(long(var));
    }
    return displayLength((long)var) + config::FLOAT_NUMBER_DIGITS + 1;
}

template <typename T>
std::size_t displayLength(const std::vector<T>& vec)
{
    std::size_t totalLength = 0;
    if (vec.empty())
    {
        return displayLength("None");
    }
    for (auto it = vec.cbegin(); it != --vec.cend(); it++)
    {
        if (std::distance(vec.cbegin(), it) > config::ARRAY_MAX_WRAPPING_LENGTH)
        {
            totalLength + std::string(", ...").size();
            return totalLength;
        }
        totalLength += displayLength(*it) + std::string(", ").size();
    }
    totalLength -= std::string(", ").size();
    return totalLength;
}


class MixedColumn : public AbstractColumn
{
public:
    MixedColumn(int _leftPadding, int _rightPadding, std::wstring&& _title)
        :
        leftPadding {_leftPadding},
        rightPadding {_rightPadding},
        title {_title},
        maxCharLength {0},
        titlePrinted {false}
    {}

    template <typename ...ItemTypes>
    MixedColumn(int _leftPadding, int _rightPadding, std::wstring&& _title, ItemTypes... items)
        :
        leftPadding {_leftPadding},
        rightPadding {_rightPadding},
        title {_title},
        maxCharLength {0}
    {
        addItems(items...);
    }

    template <typename T>
    void repeatedAddItems(std::vector<T> _items)
    {
        for (auto item: _items)
            addItems(item);
    }

    template <typename ...ItemTypes>
    void addItems(ItemTypes... items);

    template<>
    void addItems()
    {
        currentIt = items.begin();
        maxCharLength = std::max(maxCharLength, title.size());
    }

//    template <typename ...OtherTypes>
//    void addItems(MixedColumn* mixedColumn, OtherTypes... otherArgs)
//    {
//        maxCharLength = std::max(maxCharLength, static_cast<std::size_t>(mixedColumn->getColumnWidth()));
//        items.push_back(
//            [this, mixedColumnPtr = std::unique_ptr<MixedColumn>(mixedColumn)] (std::wostream&)
//            {
//                for (std::size_t i = 0; i <= mixedColumnPtr->getSize(); i++)
//                {
//                    mixedColumnPtr->dumpNext();
//                }
//            }
//        );
//        addItems(otherArgs...);
//    }

    template <typename ...OtherTypes>
    void addItems(Table* table, OtherTypes... otherArgs)
    {
        maxCharLength = std::max(maxCharLength, static_cast<std::size_t>(table->tableWidth));
        items.push_back(
            [tablePtr = std::shared_ptr<Table>(table)] (std::wostream& os)
            {
                tablePtr->dumpTableTo(os);
            }
        );
        addItems(otherArgs...);
    }

    template <typename T, typename ...OtherType>
    void addItems(std::vector<T> vec, OtherType... otherArgs)
    {
        maxCharLength = std::max(maxCharLength, displayLength(vec));
        items.push_back(
            [this, vec](std::wostream& os)
            {
                if (vec.empty())
                    os << "None";
                else
                {
                    for (auto it = vec.cbegin(); it != --vec.cend(); it++)
                    {
                        if (it != vec.cbegin()
                            && std::distance(vec.cbegin(), it) % config::ARRAY_MAX_WRAPPING_LENGTH == 0)
                            os << std::endl;
                        os << *it << L", ";
                    }
                    os << vec.back();
                }
                os << nSpace(maxCharLength - displayLength(vec) + rightPadding);
            }
        );
        addItems(otherArgs...);
    }

    template <typename T, typename ...OtherTypes>
    void addItems(std::optional<T> op, OtherTypes... otherArgs)
    {
        if (op.has_value())
            maxCharLength = std::max(maxCharLength, displayLength(op.value()));
        else
            maxCharLength = std::max(maxCharLength, displayLength("None"));
        items.push_back(
            [this, op](std::wostream& os)
            {
                if (op.has_value())
                    os << op.value() << nSpace(maxCharLength - displayLength(op.value()) + rightPadding);
                else
                    os << L"None" << nSpace(maxCharLength - displayLength("None") + rightPadding);
            }
        );
        addItems(otherArgs...);
    }

//    template <typename StringLikeType,
//            typename std::enable_if<std::is_convertible<StringLikeType, const char*>::value, int>::type = 0,
//            typename ...OtherType
//            >
//    void addItems(StringLikeType str, OtherType... otherArgs)
//    {
//        maxCharLength = std::max(maxCharLength, displayLength(str));
//        items.push_back(
//            [this, str](std::wostream& os)
//            {
//                os << str << nSpace(maxCharLength - displayLength(str) + rightPadding);
//            }
//        );
//        addItems(otherArgs...);
//    }
    template <typename ...OtherTypes>
    void addItems(const char* str, OtherTypes... otherArgs)
    {
        maxCharLength = std::max(maxCharLength, displayLength(str));
        items.push_back(
            [this, str](std::wostream& os)
            {
                os << str << nSpace(maxCharLength - displayLength(str) + rightPadding);
            }
        );
        addItems(otherArgs...);
    }

//    template <typename StringLikeType,
//    typename std::enable_if<std::is_convertible<StringLikeType, const wchar_t*>::value, int>::type = 0,
//    typename ...OtherType
//    >
//    void addItems(StringLikeType str, OtherType... otherArgs)
//    {
//        maxCharLength = std::max(maxCharLength, displayLength(str));
//        items.push_back(
//        [this, str](std::wostream& os)
//        {
//            os << str << nSpace(maxCharLength - displayLength(str) + rightPadding);
//        }
//        );
//        addItems(otherArgs...);
//    }
    template <typename ...OtherTypes>
    void addItems(const wchar_t* str, OtherTypes... otherArgs)
    {
        maxCharLength = std::max(maxCharLength, displayLength(str));
        items.push_back(
        [this, str](std::wostream& os)
        {
            os << str << nSpace(maxCharLength - displayLength(str) + rightPadding);
        }
        );
        addItems(otherArgs...);
    }

//    template <typename StringLikeType,
//    typename std::enable_if<std::is_same<StringLikeType, std::wstring>::value, int>::type = 0,
//    typename ...OtherType
//    >
//    void addItems(StringLikeType str, OtherType... otherArgs)
//    {
//        maxCharLength = std::max(maxCharLength, displayLength(str));
//        items.push_back(
//        [this, str](std::wostream& os)
//        {
//            os << str << nSpace(maxCharLength - displayLength(str) + rightPadding);
//        }
//        );
//        addItems(otherArgs...);
//    }

    template <typename ...OtherTypes>
    void addItems(std::wstring str, OtherTypes... otherArgs) {
        maxCharLength = std::max(maxCharLength, displayLength(str));
        items.push_back(
        [this, str](std::wostream &os) {
            os << str << nSpace(maxCharLength - displayLength(str) + rightPadding);
        }
        );
        addItems(otherArgs...);
    }

    template <typename IntegerType,
              typename std::enable_if<std::is_integral<IntegerType>::value, int>::type = 0,
              typename ...OtherType>
    void addItems(IntegerType integer, OtherType... otherArgs)
    {
        maxCharLength = std::max(maxCharLength, displayLength(integer));
        items.push_back(
            [this, integer](std::wostream& os)
            {
                os << integer << nSpace(maxCharLength - displayLength(integer) + rightPadding);
            }
        );
        addItems(otherArgs...);
    }

    template <typename FloatingType,
              typename std::enable_if<std::is_floating_point<FloatingType>::value, int>::type = 0,
              typename ...OtherTypes>
    void addItems(FloatingType floatVar, OtherTypes... otherArgs)
    {
        maxCharLength = std::max(maxCharLength, displayLength(floatVar));
        items.push_back(
            [this, floatVar](std::wostream& os)
            {
                os << std::fixed << std::setprecision(config::FLOAT_NUMBER_DIGITS)
                    << floatVar
                    << nSpace(maxCharLength - displayLength(floatVar) + rightPadding);
            }
        );
        addItems(otherArgs...);
    }

    std::wstring nSpace(int n)
    {
        /// Call to nSpace instead of std::setw is absolutely **_necessary_**
        /// because the Column class requires explicit and granular space management
        /// in order to function properly.
        return std::wstring(n, L' ');
    }

    void dumpNext(std::wostream& os)
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

        os << nSpace(leftPadding);
        std::invoke(*currentIt, os);

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

private:
    std::vector<std::function<void(std::wostream&)>> items;
    int leftPadding, rightPadding;
    std::size_t maxCharLength;
    std::wstring title;
    bool titlePrinted;

    decltype(items.cbegin()) currentIt;
};


#endif //PROJ1_MIXEDCOLUMN_H
