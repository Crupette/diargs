#include "diargs.hpp"
#include <string_view>
#include <list>

namespace diargs
{

ArgumentParser::ArgumentParser(
        void (*failfunc)(int), 
        ArgumentList &arguments,
        ArgsPair argcv)
{
    std::vector<std::string_view> argv(argcv.argc);
    for(int arg = 0; arg < argcv.argc; arg++) {
        argv[arg] = std::string_view(argcv.argv[arg]);
    }

    for(auto it = argv.begin() + 1; it != argv.end(); it++) {
        if(it->at(0) == '-') {
            if(it->at(1) == '-') {
                std::string_view parsing = std::string_view(it->data() + 2);
                if(parsing.size() == 0) continue;
                for(auto &argdef : arguments.arguments) {
                    if(argdef->longform != parsing) continue;
                    auto newit = argdef->parse(argv, it);
                    if(newit == std::nullopt) {
                        failfunc(-1);
                        return;
                    }
                    it = newit.value_or(argv.end());
                    break;
                }
                continue;
            }
            std::string_view parsing = (*it);
            for(char c : parsing) {
                for(auto &argdef : arguments.arguments) {
                    if(argdef->shortform != c) continue;
                    auto newit = argdef->parse(argv, it);
                    if(newit == std::nullopt) {
                        failfunc(-1);
                        return;
                    }
                    it = newit.value_or(argv.end());
                }
            }
            continue;
        }
        for(auto argit = arguments.arguments.begin(); 
                argit != arguments.arguments.end();
                argit++) {
            if((*argit)->shortform != 0 || !(*argit)->longform.empty()) continue;
            it = (*argit)->parse(argv, it).value_or(argv.end());
            arguments.arguments.erase(argit);
            break;
        }
    }
}

}
