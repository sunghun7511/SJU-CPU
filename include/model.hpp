#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include "include.hpp"

enum instruction_type {
    END = 000,
    ADD = 100,
    SUB = 200,
    STORE = 300,
    LOAD = 500,
    JMP = 600,
    JZ = 700,
    JZU = 800,
    INPUT = 901,
    OUTPUT = 902,
    VARIABLE = 1000,
};

class instruction {
public:
    instruction(int loc, instruction_type type, int target = 0);

    ~instruction() = default;

public:
    int loc;
    int target;
    instruction_type type;
};

class section {
public:
    section() = default;

    section(int loc, std::string name);

    ~section() = default;

public:
    int loc = 0;
    std::string name;
    std::vector<instruction> instructions;
};

#endif