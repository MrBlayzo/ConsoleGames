#pragma once
#include <string>

class ConsoleEngine{
public:
    ConsoleEngine();
    void clear();
    template<typename... Args>
    void print(Args... args){
        ((std::cout << args), ...);
    };
    std::string get();

private:
};