//
// Created by dop on 2/13/21.
//

#ifndef PROJ1_STATISTICSUI_H
#define PROJ1_STATISTICSUI_H

#include "ui/Column.h"
#include "common.h"
#include "input.h"
#include <iostream>
#include "ui/OptionUI.h"
#include "statistics.h"

using namespace std::placeholders;

class StatsUI : public OptionUI, public Statistics<long>
{
public:
    StatsUI() = default;

    void showCurrentState() override
    {
        auto optionColumn1 = new StringColumn(
        {
        L"A> Load data file",
        L"B> Minimum",
        L"C> Maximum",
        L"D> Range",
        L"E> Size",
        L"F> Sum",
        L"G> Mean",
        L"H> Median",
        L"I> Frequencies",
        L"J> Mode",
        L"K> Standard Deviation",
        L"L> Variance",
        L"W> Display result and write to file."
        },
        0, 5,
        L""
        );
        auto optionColumn2 = new StringColumn(
        {
        L"M> Mid Range",
        L"N> Quartiles",
        L"O> Interquartile Range",
        L"P> Outliers",
        L"Q> Sum of Squares",
        L"R> Mean Absolute Deviation",
        L"S> Root Mean Square",
        L"T> Standard Error of the Mean",
        L"U> Coefficient of Variation",
        L"V> Relative Standard Deviation"
        },
        0, 5,
        L""
        );

        auto table = Table({ optionColumn1, optionColumn2 }, L"3> Descriptive Statistics");
        table.dumpTableTo(std::wcout);
    }

    void init() override
    {
        this->terminateCharacter = '0';
        choiceCollector = CharParameter ("Option: ",
                                         [](const char& c){ return c == '0' || (tolower(c) >= 'a' && tolower(c) <= 'w');});

        auto nonEmptyVector = std::shared_ptr<AbstractPrerequisite>(
            new RequireNonEmptyVector(std::ref(elements), "No elements in array")
        );

        addOption('a',
                  std::bind(&StatsUI::loadFileOptionHandler, this, _1),
                  StringParameter("Enter file path: "));
        addOption('b',
                  statsDisplayAdapter(L"Minimum", std::bind(&Statistics::getMin, this))
                  ).require(nonEmptyVector);
        addOption('c',
                  statsDisplayAdapter(L"Maximum", std::bind(&Statistics::getMax, this))
                  ).require(nonEmptyVector);
        addOption('d',
                  statsDisplayAdapter(L"Range", std::bind(&Statistics::getRange, this))
                  ).require(nonEmptyVector);
        addOption('e',
                  statsDisplayAdapter(L"Size", std::bind(&Statistics::getSize, this))
                  ).require(nonEmptyVector);
        addOption('f',
                  statsDisplayAdapter(L"Sum", std::bind(&Statistics::getSum, this))
        ).require(nonEmptyVector);
        addOption('g',
                  statsDisplayAdapter(L"Mean", std::bind(&Statistics::getMean, this))
        ).require(nonEmptyVector);
        addOption('h',
                  optionalStatDisplayAdapter(L"Median", std::bind(&Statistics::getMedian, this))
        ).require(nonEmptyVector);
        addOption('i',
                  frequencyTableDisplayAdapter(std::bind(&Statistics::getFrequencyTable, this))
        ).require(nonEmptyVector);
        addOption('j',
                  statsDisplayAdapter(L"Mode", std::bind(&Statistics::getSize, this))
        ).require(nonEmptyVector);
        addOption('k',
                  statsDisplayAdapter(L"Standard Deviation", std::bind(&Statistics::getStandardDeviation, this))
        ).require(nonEmptyVector);
        addOption('l',
                  statsDisplayAdapter(L"Variance", std::bind(&Statistics::getVariance, this))
        ).require(nonEmptyVector);
        addOption('m',
                  statsDisplayAdapter(L"Mid Range", std::bind(&Statistics::getMidRange, this))
        ).require(nonEmptyVector);
        addOption('n',
                  quartilesDisplayAdapter(std::bind(&Statistics::getQuartiles, this))
        ).require(nonEmptyVector);
        addOption('o',
                  optionalStatDisplayAdapter(L"Interquartile Range", std::bind(&Statistics::getIQR, this))
        ).require(nonEmptyVector);
        addOption('p',
                  vectorStatDisplayAdapter(L"Outliers", std::bind(&Statistics::getOutliers, this))
        ).require(nonEmptyVector);
        addOption('q',
                  statsDisplayAdapter(L"Sum of Squares", std::bind(&Statistics::getSumOfSquares, this))
        ).require(nonEmptyVector);
        addOption('r',
                  statsDisplayAdapter(L"Mean Absolute Deviation", std::bind(&Statistics::getMeanAbsoluteDeviation, this))
        ).require(nonEmptyVector);
        addOption('s',
                  statsDisplayAdapter(L"Root Mean Square", std::bind(&Statistics::getRootMeanSquare, this))
        ).require(nonEmptyVector);
        addOption('t',
                  statsDisplayAdapter(L"Standard Error of the Mean", std::bind(&Statistics::getStdErrorOfMean, this))
        ).require(nonEmptyVector);
        addOption('u',
                  statsDisplayAdapter(L"Coefficient of Variation", std::bind(&Statistics::getCoefficientOfVariation, this))
        ).require(nonEmptyVector);
        addOption('v',
                  statsDisplayAdapter(L"Relative Standard Deviation", std::bind(&Statistics::getRelativeStd, this))
        ).require(nonEmptyVector);
        addOption('w',
                  statsDisplayAdapter(L"Display result a--", std::bind(&Statistics::getSize, this))
        ).require(nonEmptyVector);
    }

    void loadFileOptionHandler(std::string&& path)
    {
        Statistics::loadDataFromFilePath(path);
        std::wcout << "File opened successfully!" << std::endl;
        for (auto& e: elements) std::wcout << e << " ";
        std::wcout << std::endl;
    }

    template <class WideString = std::wstring, typename Func>
    std::function<void(void)> statsDisplayAdapter(WideString name, Func statsGetter)
    {
        return [statsGetter, name] ()
        {
            double stat = statsGetter();
            auto nameColumn = new StringColumn (std::vector<std::wstring>{name}, 0, 5, L"");
            auto equalColumn = new StringColumn (std::vector<std::wstring>{std::wstring(L"=")}, 0, 5, L"");
            auto statColumn = new DoubleColumn (std::vector<double>{stat}, 0, 5, L"");
            auto table = Table({nameColumn, equalColumn, statColumn}, L"Result: ");
            table.dumpTableTo(std::wcout);
        };
    }

    template <class WideString = std::wstring, typename Func>
    std::function<void(void)> optionalStatDisplayAdapter(WideString name, Func statsGetter)
    {
        return [statsGetter, name] ()
        {
            std::optional<double> stat = statsGetter();
            auto nameColumn = new StringColumn (std::vector<std::wstring>{name}, 0, 5, L"");
            auto equalColumn = new StringColumn (std::vector<std::wstring>{std::wstring(L"=")}, 0, 5, L"");
            auto statColumn = new StringColumn (
                stat.has_value() ? std::vector<std::wstring>{std::to_wstring(stat.value())} : std::vector<std::wstring>{L"Unknown"} ,
                0, 5, L"");
            auto table = Table({nameColumn, equalColumn, statColumn}, L"Result: ");
            table.dumpTableTo(std::wcout);
        };
    }

    template <class WideString = std::wstring, typename Func>
    std::function<void(void)> vectorStatDisplayAdapter(WideString name, Func statsGetter)
    {
        return [statsGetter, name] ()
        {
            auto stat = statsGetter();
            auto nameColumn = new StringColumn (std::vector<std::wstring>{name}, 0, 5, L"");
            auto equalColumn = new StringColumn (std::vector<std::wstring>{std::wstring(L"=")}, 0, 5, L"");
            std::wstring vectorRepr;
            for (const auto& element: stat)
            {
                vectorRepr += std::to_wstring(element) + L", ";
            }
            auto statColumn = new StringColumn (
                std::vector<std::wstring>{vectorRepr},
                0, 5, L"");
            auto table = Table({nameColumn, equalColumn, statColumn}, L"Result: ");
            table.dumpTableTo(std::wcout);
        };
    }

    template <typename Func>
    std::function<void(void)> quartilesDisplayAdapter(Func quartilesGetter)
    {
        return [quartilesGetter] ()
        {
            Quartiles quartile = quartilesGetter();
            auto nameColumn = new StringColumn ({L"Q1", L"Q2", L"Q3"}, 0, 5, L"");
            auto equalColumn = new StringColumn (std::vector<std::wstring>(3, std::wstring(L"-->")), 0, 5, L"");
            auto statsColumn = new StringColumn ({}, 0, 5, L"");
            for (const auto& optionalQuartile: {quartile.Q1, quartile.Q2, quartile.Q3})
            {
                if (optionalQuartile.has_value())
                    statsColumn->addItem(to_wstring(optionalQuartile.value()));
                else statsColumn->addItem(L"Unknown");
            }
            auto table = Table({nameColumn, equalColumn, statsColumn}, L"Quartiles: ");
            table.dumpTableTo(std::wcout);
        };
    }

    template <typename Func>
    std::function<void(void)> frequencyTableDisplayAdapter(Func frequencyTableGetter)
    {
        return [frequencyTableGetter] ()
        {
            auto freqTable = frequencyTableGetter();
            std::vector<long> values;
            std::transform(freqTable.begin(), freqTable.end(),
                           std::back_inserter(values),
                           std::mem_fn(&FrequencyEntry::value));
            std::vector<long> frequency;
            std::transform(freqTable.begin(), freqTable.end(),
                           std::back_inserter(frequency),
                           std::mem_fn(&FrequencyEntry::frequency));
            std::vector<double> frequencyPercentage;
            std::transform(freqTable.begin(), freqTable.end(),
                           std::back_inserter(frequencyPercentage),
                           [](const auto& entry){return 100 * std::mem_fn(&FrequencyEntry::frequencyPercentage)(entry);}
            );

            auto valueColumn = new LongColumn (values, 0, 5, L"Values");
            auto freqColumn = new LongColumn (frequency, 0, 5, L"Frequency");
            auto freqPercentColumn = new DoubleColumn (frequencyPercentage, 0, 5, L"Percentage");
            auto table = Table({valueColumn, freqColumn, freqPercentColumn}, L"Frequency Table:");
            table.dumpTableTo(std::wcout);
        };
    }

    std::function<void(void)> displayAllResultAndWriteToFile()
};

#endif //PROJ1_STATISTICSUI_H
