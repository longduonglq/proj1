#include <iostream>
#include <iomanip>
#include <fcntl.h>
#include "ui/Table.h"
#include "ui/OptionUI.h"
#include "statisticsUI.h"
#include "ui/Parameter.h"
#include <optional>
#include "statisticsUI.h"
#include <type_traits>
#include "ui/MixedColumn.h"

int main()
{
//    std::optional<double> op = std::make_optional(54);
//    std::optional<double> nop = std::nullopt;
//
//    auto req = RequireValuedOptional(std::ref(op), "2 please");
//    auto mcol = new MixedColumn(0, 5, L"col");
//    mcol->addItems("long", op, nop, 546345.34563456, std::wstring(L"dsfs"));
//    auto eqal = new MixedColumn(0, 5, L"");
//    eqal->repeatedAddItems(std::vector<char>(5, '='));
//
//    auto smallCol = new MixedColumn(0, 5, L"quartiles");
//    smallCol->addItems("q1", "q2", "q3");
//    auto smallCol2 = new MixedColumn(0, 5, L"vales");
//    smallCol2->addItems(1, 3, 5);
//    auto smallTable = new Table({smallCol, smallCol2}, L"",-1, false);
//
//    auto mcol1 = new MixedColumn(0, 5, L"col");
//    mcol1->addItems("lonasdfa g\nasfasd\nfasfafa", 345,453, smallTable);
//
//
//    auto tbl = Table({mcol, eqal, mcol1}, L"demo");
//    tbl.dumpTableTo(std::wcout);

    auto ui = StatsUI();
    ui.run();
    return 0;
}
