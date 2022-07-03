#ifndef DIARGS_HPP
#define DIARGS_HPP 1

#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <optional>
#include <algorithm>

namespace diargs {

    struct ArgsPair {
        int argc {};
        char **argv {};
    };

    struct IArgument
    {
        std::string longform {};
        char shortform {};
        
        IArgument() = default;
        IArgument(const std::string &Longform, char Shortform) : 
            longform(Longform), shortform(Shortform) {}
        virtual ~IArgument() = default;

        virtual std::optional<std::vector<std::string_view>::iterator> parse(
                std::vector<std::string_view> &argv,
                std::vector<std::string_view>::iterator arg) = 0;
    };

    template<typename T>
    struct MultiArgument : public IArgument
    {
        T *flag;
        
        MultiArgument(T &Flag) : flag(&Flag) {}
        MultiArgument(const std::string &Longform, T &Flag) :
            IArgument(Longform, 0), flag(&Flag) {}
        MultiArgument(const std::string &Longform, char Shortform, T &Flag) :
            IArgument(Longform, Shortform), flag(&Flag) {}
        ~MultiArgument() {}

        std::optional<std::vector<std::string_view>::iterator> parse(
                std::vector<std::string_view> &argv,
                std::vector<std::string_view>::iterator arg) {
            if(++arg == argv.end()) return std::nullopt;
            std::stringstream ss((*arg).data());
            ss >> std::noskipws >> *flag;
            return arg;
        }
    };

    template<typename T>
    struct ToggleArgument : public IArgument
    {
        T *flag;
        T set;

        ToggleArgument(T &Flag, const T Set) : 
            flag(&Flag), set(Set) {}
        ToggleArgument(const std::string &Longform, T &Flag, const T Set) :
            IArgument(Longform, 0), flag(&Flag), set(&Set) {}
        ToggleArgument(const std::string &Longform, char Shortform, 
                       T &Flag, const T Set) :
            IArgument(Longform, Shortform), flag(&Flag), set(&Set) {}
        ~ToggleArgument() {}

        std::optional<std::vector<std::string_view>::iterator> parse(
                std::vector<std::string_view> &argv,
                std::vector<std::string_view>::iterator arg) {
            *flag = set;
            return arg;
        }
    };

    template<typename T>
    struct OrderedArgument : public IArgument
    {
        T *flag;
        std::optional<T> dflt;
        OrderedArgument(T &Flag) :
            flag(&Flag), dflt(std::nullopt) {}
        OrderedArgument(T &Flag, T Dflt) :
            flag(&Flag), dflt(Dflt) {}
        ~OrderedArgument() {}
        std::optional<std::vector<std::string_view>::iterator> parse(
                std::vector<std::string_view> &argv,
                std::vector<std::string_view>::iterator arg) {
            std::stringstream ss((*arg).data());
            ss >> std::noskipws >> *flag;
            return arg;
        }       
    };

    struct ArgumentList
    {
        std::vector<std::unique_ptr<IArgument>> arguments;

        void addArgument() {}
        template<class T, class... ArgumentPack>
        void addArgument(T arg, ArgumentPack... args) {
            arguments.push_back(std::make_unique<T>(arg));
            addArgument(args...);
        }

        template<class... ArgumentPack>
        ArgumentList(ArgumentPack... arguments) {
            addArgument(arguments...);
        }
    };

    class ArgumentParser
    {
        void (*m_abortfunc)(int);
    public:
        ArgumentParser(
                void (*failfunc)(int),
                ArgumentList &arguments,
                ArgsPair argcv);
    };
}

#endif
