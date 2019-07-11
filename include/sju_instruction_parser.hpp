#ifndef __SJU_INSTRUCTION_PARSER_HPP__
#define __SJU_INSTRUCTION_PARSER_HPP__

#include "include.hpp"
#include "model.hpp"

enum term_type {
    NONE,
    OP_CODE_ADD,
    OP_CODE_SUB,
    OP_CODE_STORE,
    OP_CODE_LOAD,
    OP_CODE_JMP,
    OP_CODE_JZ,
    OP_CODE_JZU,
    SYMBOL,
    VARIABLE_SYMBOL
};

class sju_instruction_parser {
public:
    sju_instruction_parser() = default;

    ~sju_instruction_parser() = default;

    static void show_instructions();

    const bool parse(const std::string &file, std::vector<section> &sections);

private:
    const bool read_file(const std::string& file);
    const bool pre_processing();
    const bool parse_sections(std::vector<section> &sections);
    const bool finalize_offset(std::vector<section> &sections);
private:
    std::string content;
    std::vector<std::string> symbol_table;
    std::map<int, int> offset_table;
public:
    sju_instruction_parser(sju_instruction_parser &&) = delete;

    sju_instruction_parser &operator=(sju_instruction_parser &&) = delete;

    sju_instruction_parser(const sju_instruction_parser &) = delete;

    sju_instruction_parser &operator=(const sju_instruction_parser &) = delete;
};

#endif
