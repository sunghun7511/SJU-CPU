#include "sju_assembler.hpp"

const bool sju_assembler::assemble(std::vector<section> &sections, std::vector<int> &instructions) {
    int last_loc = -1, i;
    for (auto& sec : sections) {
        for (auto& inst : sec.instructions) {
            while (last_loc + 1 < inst.loc) {
                instructions.push_back(0);
            }
            i = (inst.type + inst.target) % 1000;
            instructions.push_back(i);
            last_loc ++;
        }
    }
    return true;
}

const std::string sju_assembler::disassemble(int instruction) {
    const int op_code = instruction / 100;
    const int operand = instruction % 100;

    if (op_code == 0 && operand == 0) {
        return "end";
    } else if (op_code == 1) {
        return "add " + std::to_string(operand);
    } else if (op_code == 2) {
        return "sub " + std::to_string(operand);
    } else if (op_code == 3) {
        return "store " + std::to_string(operand);
    } else if (op_code == 5) {
        return "load " + std::to_string(operand);
    } else if (op_code == 6) {
        return "jmp " + std::to_string(operand);
    } else if (op_code == 7) {
        return "jz " + std::to_string(operand);
    } else if (op_code == 8) {
        return "jzu " + std::to_string(operand);
    } else if (op_code == 9 && operand == 1) {
        return "input";
    } else if (op_code == 9 && operand == 2) {
        return "output";
    }
    return "";
}