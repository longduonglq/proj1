//
// Created by dop on 2/13/21.
//

#ifndef PROJ1_BASECONVERTER_H
#define PROJ1_BASECONVERTER_H

#include <vector>
#include <mutex>
#include <map>
#include <cassert>
#include <numeric>

const int MIN_BASE = 2;
const int MAX_BASE = 36;

const std::vector<char>& getAlphabetForBase(int base)
{
    assert(base >= MIN_BASE && base <= MAX_BASE);
    static std::map<int, std::vector<char>> _alphabetCache;
    auto alphabetIt = _alphabetCache.find(base);
    if (alphabetIt != _alphabetCache.end())
        return alphabetIt->second;
    else
    {
        auto alphabet = std::vector<char>(base, 0);
        int numberRange = std::min(base, 10);
        std::iota(alphabet.begin(), alphabet.begin() + numberRange, '0');

        int letterRange = base - numberRange;
        std::iota(alphabet.begin() + numberRange, alphabet.end(), 'A');

        auto insertedIt = _alphabetCache.insert(std::make_pair(base, alphabet));
        return insertedIt.first->second;
    }
}

std::vector<char> convertDecimalToBase(int value, int base)
{
    auto converted = std::vector<char>();
    auto& alphabet = getAlphabetForBase(base);
    int remain = value;
    while (remain > 0)
    {
        converted.push_back(alphabet.at(remain % base));
        remain /= base;
    }
    std::reverse(converted.begin(), converted.end());
    return converted;
}

#endif //PROJ1_BASECONVERTER_H
