#include "sju_cpu.hpp"

void sju_cpu::reset() {
    this->pc = 0;
    this->ac_register = 0;
}

void sju_cpu::clear() {
    this->pc = 0;
    this->ac_register = 0;
    for (auto &x : this->memory) {
        x = 0;
    }
}

const bool sju_cpu::execute(bool is_debug) {
    bool end = false;
    do {
        if (pc < 0 || pc >= MEMORY_SIZE) {
            throw segmentation_fault_exception();
        }
        int instruction = this->memory[pc];

        int op_code = instruction / 100;
        int operand = instruction % 100;

        if (op_code == 0 && operand == 0) { // end
            end = true;
            break;
        } else if (op_code == 1) { // add
            this->ac_register += memory[operand];
        } else if (op_code == 2) { // substract
            this->ac_register -= memory[operand];
        } else if (op_code == 3) { // store
            memory[operand] = this->ac_register;
        } else if (op_code == 5) { // load
            this->ac_register = memory[operand];
        } else if (op_code == 6) { // jmp
            this->pc = operand;
            continue;
        } else if (op_code == 7) { // jz
            if (this->ac_register == 0) {
                this->pc = operand;
                continue;
            }
        } else if (op_code == 8) { // jzu
            if (this->ac_register >= 0) {
                this->pc = operand;
                continue;
            }
        } else if (op_code == 9 && operand == 1) { // input
            std::cout << "Input : ";

            std::cin >> this->ac_register;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else if (op_code == 9 && operand == 2) { // output
            std::cout << "Output : " << this->ac_register << std::endl;
        } else {
            throw unknown_instruction_exception("Unknown OP Code");
        }
        pc++;
    } while (!is_debug);

    return end;
}

int &sju_cpu::operator[](int index) {
    if (index < 0 || index >= MEMORY_SIZE) {
        throw segmentation_fault_exception();
    }
    return this->memory[index];
}

const int sju_cpu::get_pc() {
    return this->pc;
}

const int sju_cpu::get_ac_register() {
    return this->ac_register;
}

void sju_cpu::setup_memory(std::vector<int> &vector) {
    std::copy(vector.begin(), vector.end(), this->memory);
}
