//
// Created by dop on 2/13/21.
//

#ifndef PROJ1_PREREQUISITE_H
#define PROJ1_PREREQUISITE_H


#include <map>
#include <vector>
#include <optional>

class AbstractPrerequisite
{
public:
    AbstractPrerequisite(const std::string& _errorMsg): errorMsg {_errorMsg}
    {}
    virtual ~AbstractPrerequisite() {} ;
    virtual bool isSatisfied() = 0;
    virtual const std::string getErrorMsg()
    {
        return errorMsg;
    }

private:
    std::string errorMsg;
};

class InvokeMethodRequirement : public AbstractPrerequisite
{
public:
    typedef std::function<bool(void)> CheckFunc_t;
    InvokeMethodRequirement(CheckFunc_t&& _checkFunc,
                            const std::string& _errorMsg)
        :
        AbstractPrerequisite (_errorMsg),
        checkFunc {_checkFunc}
    {}
    ~InvokeMethodRequirement() override = default;
    bool isSatisfied() override
    {
        return std::invoke(checkFunc);
    }
private:
    CheckFunc_t checkFunc;
};

template <typename OptionalType>
class RequireValuedOptional : public InvokeMethodRequirement
{
public:
    explicit RequireValuedOptional(std::reference_wrapper<OptionalType> _target,
                                   const std::string& _errorMsg) :
        InvokeMethodRequirement([_target]() { return _target.get().has_value(); }, _errorMsg ) {}
    virtual ~RequireValuedOptional() override {};
};

template <typename VecType>
class RequireNonEmptyVector : public InvokeMethodRequirement
{
public:
    explicit RequireNonEmptyVector(std::reference_wrapper<VecType> _target,
                                   const std::string& _errorMsg) :
        InvokeMethodRequirement([_target]() { return !_target.get().empty(); }, _errorMsg) {}
    virtual ~RequireNonEmptyVector() override {};
};

template <typename StringType>
class RequireNonEmptyString : public InvokeMethodRequirement
{
public:
    explicit RequireNonEmptyString(std::reference_wrapper<StringType> _target,
                                   const std::string& _errorMsg) :
        InvokeMethodRequirement([_target] () { return !_target.get().empty() ;}) {}
    virtual ~RequireNonEmptyString() override {};
};

#endif //PROJ1_PREREQUISITE_H
