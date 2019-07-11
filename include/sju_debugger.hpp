#ifndef __SJU_DEBUGGER_H__
#define __SJU_DEBUGGER_H__

#include "include.hpp"
#include "sju_cpu.hpp"
#include "sju_assembler.hpp"

class sju_debugger {
public:
    sju_debugger() = default;

    ~sju_debugger() = default;

    void attach(sju_cpu &cpu, std::vector<int> &instructions);

    void interactive();

private:
    const bool handle_command(const std::vector<std::string> &args);

    const bool check_breakpoints();
    void run_while_finish();

private:
    sju_cpu *cpu;
    std::vector<int> original_instructions;

    bool is_finished = true;
    std::vector<int> breakpoints;
public:
    sju_debugger(sju_debugger &&) = delete;

    sju_debugger &operator=(sju_debugger &&) = delete;

    sju_debugger(const sju_debugger &) = delete;

    sju_debugger &operator=(const sju_debugger &) = delete;
};

#endif