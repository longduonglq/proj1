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

template <typename T>
class Statistics
{
public:
    using Quartiles = struct {
        std::optional<double> Q1, Q2, Q3;
    };

    using FrequencyEntry = struct {
        T value;
        long frequency;
        double frequencyPercentage;
    };

    void loadDataFromFilePath(std::string path)
    {
        ifstream statsFile(path);
        if (statsFile.is_open())
        {
            clear();
            T currentValue;
            while (statsFile >> currentValue)
                elements.push_back(currentValue);
            std::sort(elements.begin(), elements.end());
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
    Statistics(std::vector<T>&& elements)
    :
    elements {std::move(elements)}
    {
        std::sort(elements.begin(), elements.end());
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
                std::accumulate(elements.cbegin(), elements.cend(), 0, std::plus<>())
            );
            return _sumCache.value();
        }
    }

    std::size_t getSize() const
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

    std::optional<double> getMedian() const
    {
        return getMedianInRange(elements.begin(), elements.end());
    }

    const T& getMode() const
    {
        auto modeIt = elements.cbegin();
        std::size_t modeFrequency = 0;

        auto current = elements.cbegin();
        while (current != elements.cend())
        {
            std::size_t currentFrequency = std::count(current, elements.cend(), *current);
            if (currentFrequency >= modeFrequency)
            {
                modeIt = current;
                do
                {
                    current++;
                } while (*current == *modeIt && current != elements.cend());
            }
        }
        return *modeIt;
    }

    const double& getVariance() const
    {
        if (_varianceCache.has_value())
            return _varianceCache.value();
        else
        {
            _varianceCache.emplace(
            std::transform_reduce(
            elements.cbegin(), elements.cend(),
            0.0,
            std::plus<>(),
            [this](const T& element) { return pow(element - this->getMean(), 2); }
            ) / getSize()
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
            // TODO: take care of degenerate cases here
            if (getSize() % 2 == 0)
            {
                _quartilesCache.emplace( Quartiles {
                                         .Q1 = getMedianInRange(elements.cbegin(), elements.cbegin() + getSize() / 2),
                                         .Q2 = getMedianInRange(elements.cbegin(), elements.cend()),
                                         .Q3 = getMedianInRange(elements.cbegin() + 1 + getSize() / 2, elements.cend())
                                         });
            }
            else
            {
                _quartilesCache.emplace( Quartiles {
                                         .Q1 = getMedianInRange(elements.cbegin(), elements.cbegin() + getSize() / 2),
                                         .Q2 = getMedianInRange(elements.cbegin(), elements.cend()),
                                         .Q3 = getMedianInRange(elements.cbegin() + 2 + getSize() / 2, elements.cend())
                                         });
            }
            return _quartilesCache.value();
        }
    }

    std::optional<double> getIQR() const
    {
        auto& quartiles = getQuartiles();
        if (!quartiles.Q3.has_value() || !quartiles.Q1.has_value())
            return std::nullopt;
        return quartiles.Q3.value() - quartiles.Q1.value();
    }

    std::optional<std::pair<double, double>> getOutlierFence() const
    {
        if (!getIQR().has_value()) return std::nullopt;
        const auto& q = getQuartiles();
        return std::make_pair(q.Q3.value() + 1.5 * getIQR().value(), q.Q1.value() - 1.5 * getIQR().value());
    }

    std::vector<T> getOutliers() const
    {
        auto outliers = std::vector<T>();
        if (!getOutlierFence().has_value()) return elements;
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
        );
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

    std::vector<FrequencyEntry> getFrequencyTable()
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

        auto medianIt = elements.cbegin();
        if (distance % 2 == 0)
        {
            std::advance(medianIt, distance / 2);
            return std::make_optional((*medianIt + *std::next(medianIt)) / 2.0);
        }
        else
        {
            std::advance(medianIt, (distance + 1) / 2);
            return std::make_optional(*medianIt);
        }
    }
};

#endif //PROJ1_STATISTICS_H
