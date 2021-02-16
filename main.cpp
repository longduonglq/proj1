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

int main()
{
    std::optional<double> op = std::make_optional(54);

    // auto req = RequireValuedOptional(std::ref(op), "2 please");

    auto ui = StatsUI();
    ui.run();
    return 0;
}
