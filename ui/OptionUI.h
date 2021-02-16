//
// Created by dop on 2/13/21.
//

#ifndef PROJ1_OPTIONUI_H
#define PROJ1_OPTIONUI_H

#include <optional>
#include "Prerequisite.h"
#include "Parameter.h"
#include "UIExcept.h"

class SingularOption
{
public:
    std::function<void(std::ostream&)> invokeOption;

    SingularOption() = default;

    template<typename OptionHandler_t, typename ...AbstractParameter>
    SingularOption(OptionHandler_t optionHandler, AbstractParameter... requiredParams)
    {
        bindHandler(optionHandler, requiredParams...);
    }

    template <typename OptionHandler_t, typename ...AbstractParameters>
    void bindHandler(OptionHandler_t optionHandler, AbstractParameters... requiredParams)
    {
        invokeOption = [this, optionHandler, requiredParams...] ( std::ostream& os )
        {
            for (auto& prerequisite: prerequisites)
            {
                if (!prerequisite->isSatisfied())
                {
                    os << prerequisite->getErrorMsg() << std::endl;
                    return;
                }
            }
            try
            {
                optionHandler(requiredParams.collectParam()...);
            }
            catch (UIExcept& e)
            {
                os << e.what() << std::endl;
                return;
            }
        };
    }

    SingularOption& require(std::shared_ptr<AbstractPrerequisite> condition)
    {
        prerequisites.push_back(condition);
        return *this;
    }

private:
    std::vector<std::shared_ptr<AbstractPrerequisite>> prerequisites;
};

class OptionUI
{
public:
    OptionUI() :
        choiceCollector("Option: ")
    {}

    template<typename ...AbstractParameter, typename OptionHandler_t>
    SingularOption& addOption(char optionCharacter,
                              OptionHandler_t optionHandler,
                              AbstractParameter... requiredParams)
    {
        assert (options.find(optionCharacter) == options.end());

        auto singularOptionIt = options.insert(
            std::make_pair(
                optionCharacter,
                std::make_unique<SingularOption>(
                    optionHandler,
                    requiredParams...
                )
            )
        );
        return *(singularOptionIt.first->second);
    }

    void processOption(char optionCharacter)
    {
        auto option = options.find(optionCharacter);
        option->second->invokeOption(std::cout);
    }

    virtual void init() = 0;
    virtual void showCurrentState() = 0;

    void run()
    {
        this->init();
        this->showCurrentState();

        char userChoice;
        while (userChoice = choiceCollector.collectParam(),
               userChoice != terminateCharacter)
        {
            if (options.find(userChoice) == options.end())
            {
                std::wcout << "Option was not registered" << std::endl;
            }
            else
            {
                processOption(userChoice);
            }
            this->showCurrentState();
        }
    }

protected:
    CharParameter choiceCollector;
    std::optional<char> terminateCharacter;
    std::map<char, std::unique_ptr<SingularOption>> options;
};

#endif //PROJ1_OPTIONUI_H
