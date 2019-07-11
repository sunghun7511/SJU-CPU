#include "sju_instruction_parser.hpp"

void sju_instruction_parser::show_instructions() {
    info("SJU CPU Instructions");
    info("");
    info("end (000)\t\t| exit program");
    info("add (1xx)\t\t| add memory[xx] value to ac register");
    info("sub (2xx)\t\t| subtract memory[xx] from ac register");
    info("load (3xx)\t| load memory[xx] to ac register");
    info("store (5xx)\t| store ac register to memory[xx]");
    info("jmp (6xx)\t\t| jump to memory[xx]");
    info("jz (7xx)\t\t| jump to memory[xx] if ac register is zero");
    info("jzu (8xx)\t\t| jump to memory[xx] if ac register is zero or more");
    info("input (901)\t| input value to ac register");
    info("output (902)\t| output value from ac register");
}

const bool sju_instruction_parser::parse(const std::string &file, std::vector<section> &sections) {
    bool result;

    debug("Read content from {}", file);
    result = read_file(file);
    if (!result) {
        error("Read file content failed.");
        return false;
    }

    debug("Now pre-processing");
    result = pre_processing();
    if (!result) {
        error("Pre-processing failed.");
        return false;
    }

    debug("Now parsing sections");
    result = parse_sections(sections);
    if (!result) {
        error("Parse section failed.");
        return false;
    }

    debug("Start finalize offset");
    result = finalize_offset(sections);
    if (!result) {
        error("Finilize offset failed.");
        return false;
    }

    return true;
}


const bool sju_instruction_parser::read_file(const std::string &file) {
    std::ifstream ifs(file);
    content = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    if (content.empty()) {
        error("File is empty");
        return false;
    }
    return true;
}

const bool sju_instruction_parser::pre_processing() {
    std::string pool;

    boost::algorithm::to_lower(content);

    bool comment = false;
    int start = 0, length = 0;
    for (int i = 0; i < content.size(); i++) {
        char c = content.at(i);
        if (!comment && (c == '#' || c == ';')) {
            comment = true;
            start = i;
            length = 1;
        } else if (comment && c == '\n') {
            comment = false;
            content.erase(start, length);
            i -= length;
        }

        if (comment) {
            length++;
        }
    }
    if (comment) {
        content.erase(start, length);
    }
    boost::trim_if(content, boost::is_any_of("\r\n\t "));
    return true;
}

const bool sju_instruction_parser::parse_sections(std::vector<section> &sections) {
    std::vector<std::string> terms;
    boost::split(terms, content, boost::is_any_of(" \t\n\r"), boost::token_compress_on);

    if (terms.empty() || (terms.size() == 1 && terms[0].empty())) {
        error("Cannot found instructions");
        return false;
    }

    int loc = 0;
    section now_section;
    std::string last_symbol;
    term_type last_type = term_type::NONE;
    for (auto &term : terms) {
        if (term.at(term.size() - 1) == ':') {
            if (!now_section.name.empty()) {
                sections.push_back(now_section);
            }
            std::string section_name = term.substr(0, term.size() - 1);

            if (section_name.empty()) {
                error("Empty section name");
                return false;
            }

            for (auto &sec : sections) {
                if (sec.name == section_name) {
                    error("Duplicate section name");
                    return false;
                }
            }
            std::vector<std::string>::iterator it;
            if ((it = std::find(symbol_table.begin(), symbol_table.end(), section_name)) != symbol_table.end()) {
                int idx = std::distance(symbol_table.begin(), it);

                if (offset_table.find(idx) != offset_table.end()) {
                    error("Duplicate symbol : {}", section_name);
                    return false;
                }
                offset_table[idx] = loc;
            } else {
                symbol_table.push_back(section_name);
                offset_table[symbol_table.size() - 1] = loc;
            }
            now_section = section(loc, section_name);
        } else {
            if (now_section.name.empty()) {
                symbol_table.emplace_back("main");
                offset_table[0] = loc;
                now_section = section(loc, "main");
            }

            if (last_type == term_type::VARIABLE_SYMBOL) {
                int var;
                try {
                    var = std::stoi(term);
                } catch (std::invalid_argument &exc) {
                    error("Invalid argument : {}", term);
                    return false;
                } catch (std::out_of_range &exc) {
                    error("Out of range : {}", term);
                    return false;
                }

                if (!last_symbol.empty()) {
                    std::vector<std::string>::iterator it;
                    if ((it = std::find(symbol_table.begin(), symbol_table.end(), last_symbol)) != symbol_table.end()) {
                        offset_table[std::distance(symbol_table.begin(), it)] = loc;
                    } else {
                        symbol_table.push_back(last_symbol);
                        offset_table[symbol_table.size() - 1] = loc;
                    }
                }

                now_section.instructions.emplace_back(instruction(loc, instruction_type::VARIABLE, var));
                last_type = term_type::NONE;
                loc++;
            } else if (last_type == term_type::NONE) {
                if (term == "end") {
                    now_section.instructions.emplace_back(instruction(loc, instruction_type::END, 0));
                    last_type = term_type::NONE;
                    loc++;
                } else if (term == "input") {
                    now_section.instructions.emplace_back(instruction(loc, instruction_type::INPUT, 0));
                    last_type = term_type::NONE;
                    loc++;
                } else if (term == "output") {
                    now_section.instructions.emplace_back(instruction(loc, instruction_type::OUTPUT, 0));
                    last_type = term_type::NONE;
                    loc++;
                } else if (term == "add") {
                    last_type = term_type::OP_CODE_ADD;
                } else if (term == "sub") {
                    last_type = term_type::OP_CODE_SUB;
                } else if (term == "store") {
                    last_type = term_type::OP_CODE_STORE;
                } else if (term == "load") {
                    last_type = term_type::OP_CODE_LOAD;
                } else if (term == "jmp") {
                    last_type = term_type::OP_CODE_JMP;
                } else if (term == "jz") {
                    last_type = term_type::OP_CODE_JZ;
                } else if (term == "jzu") {
                    last_type = term_type::OP_CODE_JZU;
                } else if (term.at(term.size() - 1) == '=') {
                    last_symbol = term.substr(0, term.size() - 1);
                    last_type = term_type::VARIABLE_SYMBOL;
                } else {
                    last_symbol = term;
                    last_type = term_type::SYMBOL;
                }
            } else if (last_type == term_type::SYMBOL) {
                if (term == "=") {
                    last_type = term_type::VARIABLE_SYMBOL;
                } else if (term.at(0) == '=') {
                    int var;
                    try {
                        var = std::stoi(term.substr(1));
                    } catch (std::invalid_argument &exc) {
                        error("Invalid argument : {}", term);
                        return false;
                    } catch (std::out_of_range &exc) {
                        error("Out of range : {}", term);
                        return false;
                    }

                    if (!last_symbol.empty()) {
                        std::vector<std::string>::iterator it;
                        if ((it = std::find(symbol_table.begin(), symbol_table.end(), last_symbol)) !=
                            symbol_table.end()) {
                            int idx = std::distance(symbol_table.begin(), it);

                            if (offset_table.find(idx) != offset_table.end()) {
                                error("Duplicate symbol : {}", last_symbol);
                                return false;
                            }
                            offset_table[idx] = loc;
                        } else {
                            symbol_table.push_back(last_symbol);
                            offset_table[symbol_table.size() - 1] = loc;
                        }
                    }

                    now_section.instructions.emplace_back(instruction(loc, instruction_type::VARIABLE, var));
                    last_type = term_type::NONE;
                    loc++;
                } else {
                    error("Unknown symbol location : {}", term);
                    return false;
                }
            } else {
                int var;

                std::vector<std::string>::iterator it;
                if ((it = std::find(symbol_table.begin(), symbol_table.end(), term)) != symbol_table.end()) {
                    var = std::distance(symbol_table.begin(), it);
                } else {
                    symbol_table.push_back(term);
                    var = symbol_table.size() - 1;
                }
                instruction_type inst_type;

                if (last_type == term_type::OP_CODE_ADD) {
                    inst_type = instruction_type::ADD;
                } else if (last_type == term_type::OP_CODE_SUB) {
                    inst_type = instruction_type::SUB;
                } else if (last_type == term_type::OP_CODE_LOAD) {
                    inst_type = instruction_type::LOAD;
                } else if (last_type == term_type::OP_CODE_STORE) {
                    inst_type = instruction_type::STORE;
                } else if (last_type == term_type::OP_CODE_JMP) {
                    inst_type = instruction_type::JMP;
                } else if (last_type == term_type::OP_CODE_JZ) {
                    inst_type = instruction_type::JZ;
                } else {
                    inst_type = instruction_type::JZU;
                }

                now_section.instructions.emplace_back(instruction(loc, inst_type, var));
                last_type = term_type::NONE;
                loc++;
            }
        }
    }
    if (!now_section.name.empty()) {
        sections.emplace_back(now_section);
    }
    return true;
}

const bool sju_instruction_parser::finalize_offset(std::vector<section> &sections) {
    for (auto &sec : sections) {
        for (auto &ins : sec.instructions) {
            if (ins.type == instruction_type::ADD
                || ins.type == instruction_type::SUB
                || ins.type == instruction_type::LOAD
                || ins.type == instruction_type::STORE
                || ins.type == instruction_type::JMP
                || ins.type == instruction_type::JZ
                || ins.type == instruction_type::JZU) {
                if (offset_table.find(ins.target) == offset_table.end()) {
                    error("Unknown symbol : {}", ins.target);
                    return false;
                }
                ins.target = offset_table[ins.target];
            }
        }
    }
    return true;
}

instruction::instruction(int loc, instruction_type type, int target) : loc(loc), type(type), target(target) {}

section::section(int loc, std::string name) : loc(loc), name(std::move(name)) {}
