//
// Created by dop on 2/13/21.
//

#ifndef PROJ1_STATISTICS_H
#define PROJ1_STATISTICS_H

#include <iostream>
#include <vector>
#include <optional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <functional>
#include <fstream>
#include "ui/Table.h"

using namespace std;

template <typename T>
class Statistics
{
public:
    using Quartiles = struct {
        optional<double> Q1, Q2, Q3;
    };

    using FrequencyEntry = struct {
        T value;
        long frequency;
        double frequencyPercentage;
    };

    void loadDataFromFilePath(string path)
    {
        ifstream statsFile(path);
        if (statsFile.is_open())
        {
            clear();
            T currentValue;
            while (statsFile >> currentValue)
                elements.push_back(currentValue);
            sort(elements.begin(), elements.end());
        }
        else throw UIExcept("Cannot open file");
    }

    void clear()
    {
        elements.clear();
        _meanCache.reset();
        _sumCache.reset();
        _varianceCache.reset();
        _quartilesCache.reset();
    }

    Statistics() :
        elements {}
    {}
    Statistics(vector<T>&& elements)
    :
    elements {move(elements)}
    {
        sort(elements.begin(), elements.end());
    }

    const T& getMin() const
    {
        return elements.front();
    }
    const T& getMax() const
    {
        return elements.back();
    }

    const T getRange()
    {
        return getMax() - getMin();
    }

    const T& getSum() const
    {
        if (_sumCache.has_value())
            return _sumCache.value();
        else
        {
            _sumCache.emplace(
                accumulate(elements.cbegin(), elements.cend(), 0, plus<>())
            );
            return _sumCache.value();
        }
    }

    size_t getSize() const
    {
        return elements.size();
    }

    const double& getMean() const
    {
        if (_meanCache.has_value())
            return _meanCache.value();
        else
        {
            _meanCache.emplace(static_cast<double>(getSum()) / getSize());
            return _meanCache.value();
        }
    }

    optional<double> getMedian() const
    {
        return getMedianInRange(elements.begin(), elements.end());
    }

    std::vector<T> getMode() const
    {
        auto freqTable = getFrequencyTable();
        auto maxEntry = std::max_element(freqTable.cbegin(), freqTable.cend(),
                                         [](const FrequencyEntry& entry1, const  FrequencyEntry& entry2)
                                         {
                                                return entry1.frequency < entry2.frequency;
                                         });
        auto modeElements = std::vector<T>();
        for (auto& entry : freqTable)
        {
            if (entry.frequency >= maxEntry->frequency)
                modeElements.push_back(entry.value);
        }
        return modeElements;
    }

    const double& getVariance() const
    {
        if (_varianceCache.has_value())
            return _varianceCache.value();
        else
        {
            _varianceCache.emplace(
            transform_reduce(
            elements.cbegin(), elements.cend(),
            0.0,
            plus<>(),
            [this](const T& element) { return pow(element - this->getMean(), 2); }
            ) / (getSize() - 1)
            );
            return _varianceCache.value();
        }
    }

    double getStandardDeviation() const
    {
        return sqrt(getVariance());
    }

    double getMidRange() const
    {
        return (getMax() + getMin()) / 2.0;
    }


    const Quartiles& getQuartiles() const
    {
        if (_quartilesCache.has_value())
            return _quartilesCache.value();
        else
        {
            if (getSize() % 2 == 0)
            {
                _quartilesCache.emplace(
                    Quartiles {
                        .Q1 = getMedianInRange(elements.cbegin(), elements.cbegin() + getSize() / 2),
                        .Q2 = getMedianInRange(elements.cbegin(), elements.cend()),
                        .Q3 = getMedianInRange(elements.cbegin() + getSize() / 2, elements.cend())
                    });
            }
            else
            {
                _quartilesCache.emplace(
                    Quartiles {
                        .Q1 = getMedianInRange(elements.cbegin(), elements.cbegin() + getSize() / 2),
                        .Q2 = getMedianInRange(elements.cbegin(), elements.cend()),
                        .Q3 = getMedianInRange(elements.cbegin() + getSize() / 2 + 1, elements.cend())
                    });
            }
            return _quartilesCache.value();
        }
    }

    optional<double> getIQR() const
    {
        auto& quartiles = getQuartiles();
        if (!quartiles.Q3.has_value() || !quartiles.Q1.has_value())
            return nullopt;
        return quartiles.Q3.value() - quartiles.Q1.value();
    }

    optional<pair<double, double>> getOutlierFence() const
    {
        if (!getIQR().has_value()) return std::nullopt;
        const auto& q = getQuartiles();
        return std::make_pair(q.Q1.value() - 1.5 * getIQR().value(), q.Q3.value() + 1.5 * getIQR().value());
    }

    std::vector<T> getOutliers() const
    {
        auto outliers = std::vector<T>();
        if (!getOutlierFence().has_value()) return outliers;
        auto fence = getOutlierFence().value();
        std::copy_if(elements.cbegin(), elements.cend(),
                     std::back_inserter(outliers),
                     [&fence](const auto& e){ return e < fence.first || e > fence.second; });
        return outliers;
    }

    double getSumOfSquares() const
    {
        return
        std::transform_reduce(
            elements.cbegin(), elements.cend(),
            0.0,
            std::plus<>(),
            [this](const T& element) { return pow(element - this->getMean(), 2);}
        );
    }

    double getMeanAbsoluteDeviation() const
    {
        return
            std::transform_reduce(
            elements.cbegin(), elements.cend(),
            0.0,
            std::plus<>(),
            [this](const T& element) { return abs(element - this->getMean()); }
        ) / getSize();
    }

    double getRootMeanSquare() const
    {
        return
        sqrt(
                std::transform_reduce(
                    elements.cbegin(), elements.cend(),
                    0.0,
                    std::plus<>(),
                    [](const T& element) { return element * element; }
                ) / getSize()
        );
    }

    double getStdErrorOfMean() const
    {
        return getStandardDeviation() / sqrt(getSize());
    }

    double getCoefficientOfVariation() const
    {
        return getStandardDeviation() / getMean();
    }

    double getRelativeStd() const
    {
        return (100.0 * getStandardDeviation()) / getMean();
    }

    double getSkewness() const
    {
        return
        std::transform_reduce(
            elements.cbegin(), elements.cend(),
            0.0,
            std::plus<>(),
            [this](const auto& e)
            {
                return pow(e - this->getMean(), 3);
            }
        ) / (getSize() * pow(getStandardDeviation(), 3));
    }

    double getKurtosis() const
    {
        double n = getSize();
        double coefficient = n * (n + 1) / ((n - 1) * (n - 2) * (n - 3));
        return
        coefficient * std::transform_reduce(
        elements.cbegin(), elements.cend(),
        0.0,
        std::plus<>(),
        [this](const auto& e)
        {
            return pow(e - this->getMean(), 4);
        }
        ) / pow(getStandardDeviation(), 4);
    }

    double getKurtosisExcess() const
    {
        double n = getSize();
        double adjustmentTerm = -3* (n - 1) * (n - 1) / ((n - 2) * (n - 3));
        return getKurtosis() + adjustmentTerm;
    }

    std::vector<FrequencyEntry> getFrequencyTable() const
    {
        auto frequencyTable = std::vector<FrequencyEntry> ();
        auto it = elements.cbegin();
        while (it != elements.cend())
        {
            auto frequencyEntry = FrequencyEntry {
                .value = *it,
                .frequency = 1,
                .frequencyPercentage = 0
            };
            while (it != elements.cend() && * ++it == frequencyEntry.value)
                frequencyEntry.frequency++;
            frequencyTable.push_back(frequencyEntry);
        }

        long totalFrequency = std::transform_reduce(
            frequencyTable.cbegin(), frequencyTable.cend(),
            0,
            std::plus<>(),
            [](const FrequencyEntry& entry) { return entry.frequency; }
        );
        for (auto& entry: frequencyTable)
        {
            entry.frequencyPercentage = static_cast<double>(entry.frequency) / totalFrequency;
        }
        return frequencyTable;
    }

protected:
    std::vector<T> elements;

    // caches for statistics that are used many times
    mutable std::optional<T> _sumCache;
    mutable std::optional<double> _meanCache;
    mutable std::optional<double> _varianceCache;
    mutable std::optional<Quartiles> _quartilesCache;

    /// Helpers
    std::optional<double> getMedianInRange(decltype(elements.cbegin()) lowBound, decltype(elements.cbegin()) highBound) const
    {
        ptrdiff_t distance = std::distance(lowBound, highBound);
        if (distance <= 2)
            return std::nullopt;

        if (distance % 2 == 0)
        {
            auto medianIndex = distance / 2 + std::distance(elements.cbegin(), lowBound);
            return std::make_optional((elements.at(medianIndex) + elements.at(medianIndex - 1)) / 2.0);
        }
        else
        {
            auto medianIndex = distance / 2 + std::distance(elements.cbegin(), lowBound);
            return std::make_optional(elements.at(medianIndex));
        }
    }
};

#endif //PROJ1_STATISTICS_H
