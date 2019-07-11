#ifndef __SJU_ASSEMBLER_HPP__
#define __SJU_ASSEMBLER_HPP__

#include "include.hpp"
#include "model.hpp"

class sju_assembler {
public:
    sju_assembler() = delete;
    ~sju_assembler() = delete;

    static const bool assemble(std::vector<section> &sections, std::vector<int> &instructions);

    static const std::string disassemble(int instruction);
private:

public:
    sju_assembler(sju_assembler&&) = delete;
    sju_assembler& operator=(sju_assembler&&) = delete;

    sju_assembler(const sju_assembler&) = delete;
    sju_assembler& operator=(const sju_assembler&) = delete;
};

#endif