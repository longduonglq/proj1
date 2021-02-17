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
#include "ui/MixedColumn.h"

using namespace std::placeholders;

class StatsUI : public OptionUI, public Statistics<long>
{
public:
    StatsUI() = default;

    void showCurrentState() override
    {
        auto optionColumn1 = new MixedColumn (0, 5,L"");
        optionColumn1->addItems(
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
            L"W> Display result and write to file.",
            L"0> Return"
        );
        auto optionColumn2 = new MixedColumn(0, 5,L"");
        optionColumn2->addItems(
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
        );
        Table({ optionColumn1, optionColumn2 }, L"3> Descriptive Statistics").dumpTableTo(std::wcout);
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
                  statsDisplayAdapter(L"Median", std::bind(&Statistics::getMedian, this))
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
                  statsDisplayAdapter(L"Interquartile Range", std::bind(&Statistics::getIQR, this))
        ).require(nonEmptyVector);
        addOption('p',
                  statsDisplayAdapter(L"Outliers", std::bind(&Statistics::getOutliers, this))
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
        addOption('w', std::bind(&StatsUI::displayAllResultAndWriteToFile, this)
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
            auto stat = statsGetter();
            auto nameColumn = new MixedColumn(0, 5, L"", name);
            auto equalColumn = new MixedColumn(0, 5, L"", L"=");
            auto statColumn = new MixedColumn(0, 5, L"", stat);
            Table({nameColumn, equalColumn, statColumn}, L"Result: ").dumpTableTo(std::wcout);
        };
    }

    template <typename Func>
    std::function<void(void)> quartilesDisplayAdapter(Func quartilesGetter)
    {
        return [quartilesGetter] ()
        {
            Quartiles  quartiles = quartilesGetter();
            auto nameColumn = new MixedColumn(0, 5, L"", "Q1", "Q2", "Q3");
            auto equalColumn = new MixedColumn(0, 5, L"");
            equalColumn->repeatedAddItems(std::vector<std::wstring>(3, L"-->"));
            auto statsColumn = new MixedColumn(0, 5, L"", quartiles.Q1, quartiles.Q2, quartiles.Q3);
            Table({nameColumn, equalColumn, statsColumn}, L"Quartiles: ").dumpTableTo(std::wcout);
        };
    }

    template <typename Func>
    Table* frequencyTableToUITable(Func frequencyTableGetter)
    {
        auto freqTable = frequencyTableGetter();
        std::vector<long> values;
        std::transform(freqTable.begin(), freqTable.end(), std::back_inserter(values), std::mem_fn(&FrequencyEntry::value));
        std::vector<long> frequency;
        std::transform(freqTable.begin(), freqTable.end(), std::back_inserter(frequency), std::mem_fn(&FrequencyEntry::frequency));
        std::vector<double> frequencyPercentage;
        std::transform(freqTable.begin(), freqTable.end(), std::back_inserter(frequencyPercentage),
                       [](const auto& entry){return 100 * std::mem_fn(&FrequencyEntry::frequencyPercentage)(entry);}
        );

        auto valueColumn = new MixedColumn (0, 5, L"Values");
        valueColumn->repeatedAddItems(values);
        auto freqColumn = new MixedColumn (0, 5, L"Frequency");
        freqColumn->repeatedAddItems(frequency);
        auto freqPercentColumn = new MixedColumn (0, 5, L"Percentage");
        freqPercentColumn->repeatedAddItems(frequencyPercentage);
        return new Table({valueColumn, freqColumn, freqPercentColumn}, L"", -1, false);
    }

    template <typename Func>
    std::function<void(void)> frequencyTableDisplayAdapter(Func frequencyTableGetter)
    {
        return [this, frequencyTableGetter] ()
        {
            auto table = frequencyTableToUITable(frequencyTableGetter);
            table->dumpTableTo(std::wcout);
            delete table;
        };
    }

    void displayAllResultAndWriteToFile()
    {
        auto statisticNameColumn = new MixedColumn (0, 5,L"Concept");
        statisticNameColumn->addItems(
        L"Minimum",
        L"Maximum",
        L"Range",
        L"Size",
        L"Sum",
        L"Mean",
        L"Median",
        L"Mode",
        L"Standard Deviation",
        L"Variance",
        L"Mid Range",
        L"Quartiles",
        L"Interquartile Range",
        L"Outliers",
        L"Sum of Squares",
        L"Mean Absolute Deviation",
        L"Root Mean Square",
        L"Standard Error of the Mean",
        L"Skewness",
        L"Kurtosis",
        L"Kurtosis Excess",
        L"Coefficient of Variation",
        L"Relative Standard Deviation",
        L"Frequency Table");

        auto quartiles = getQuartiles();
        auto quartileNames = new MixedColumn(0, 5, L"", "Q1", "Q2", "Q3");
        auto arrowColumn = new MixedColumn(0, 5, L"");
        arrowColumn->repeatedAddItems(std::vector<const char*>(3, "-->"));
        auto quartileValues = new MixedColumn(0, 5, L"", quartiles.Q1, quartiles.Q2, quartiles.Q3);
        auto* quartileTable = new Table({quartileNames, arrowColumn, quartileValues}, L"", -1, false);

        auto statisticValueColumn = new MixedColumn(0, 5, L"Values");
        statisticValueColumn->addItems(
            getMin(),
            getMax(),
            getRange(),
            getSize(),
            getSum(),
            getMean(),
            getMedian(),
            getMode(),
            getStandardDeviation(),
            getVariance(),
            getMidRange(),
            quartileTable,
            getIQR(),
            getOutliers(),
            getSumOfSquares(),
            getMeanAbsoluteDeviation(),
            getRootMeanSquare(),
            getStdErrorOfMean(),
            getSkewness(),
            getKurtosis(),
            getKurtosisExcess(),
            getCoefficientOfVariation(),
            to_wstring(getRelativeStd()) + L"%",
            frequencyTableToUITable(std::bind(&Statistics::getFrequencyTable, this))
        );

        auto equalColumn = new MixedColumn(0, 2, L"");
        equalColumn->repeatedAddItems(std::vector<char>(24, '='));

        auto table = Table({statisticNameColumn, equalColumn, statisticValueColumn}, L"Statistics");
        table.dumpTableTo(std::wcout);

        auto filePath = StringParameter ("Enter file path: ").collectParam();
        auto outFile = wofstream (filePath);
        while (!outFile.is_open())
        {
            std::wcout << L"ERROR: Cannot open file. Try again." << endl;
            filePath = StringParameter ("Enter file path: ").collectParam();
            outFile = wofstream (filePath);
        }
        table.dumpTableTo(outFile);
        std::wcout << L"Summary was written to file." << std::endl;
    }
};

#endif //PROJ1_STATISTICSUI_H
