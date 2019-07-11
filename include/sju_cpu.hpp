#ifndef __SJU_CPU_HPP__
#define __SJU_CPU_HPP__

#include "include.hpp"

constexpr int MEMORY_SIZE = 100;

class sju_cpu {
public:
    sju_cpu() = default;

    ~sju_cpu() = default;

    void reset();

    void clear();

    const bool execute(bool is_debug);

    const int get_pc();

    const int get_ac_register();

    void setup_memory(std::vector<int> &vector);

public:
    int &operator[](int index);

private:
    int pc = 0;
    int ac_register = 0;
    int memory[MEMORY_SIZE] = {0,};
public:
    sju_cpu(sju_cpu &&) = delete;

    sju_cpu &operator=(sju_cpu &&) = delete;

    sju_cpu(const sju_cpu &) = delete;

    sju_cpu &operator=(const sju_cpu &) = delete;
};


#endif