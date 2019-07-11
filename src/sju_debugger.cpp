#include "sju_debugger.hpp"

void sju_debugger::attach(sju_cpu &cpu_, std::vector<int> &instructions) {
    breakpoints.clear();
    this->cpu = &cpu_;
    this->original_instructions = instructions;
}

void sju_debugger::interactive() {
    std::string command;
    std::vector<std::string> args;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        boost::split(args, command, boost::is_any_of(" "));

        try {
            if (!handle_command(args)) {
                break;
            }
        } catch (segmentation_fault_exception &exc) {
            error("Segmentation fault / {}", exc.what());
            debug("Program ended.");
            is_finished = true;
        } catch (unknown_instruction_exception &exc) {
            error("Unknown instruction error / {}", exc.what());
            debug("Program ended.");
            is_finished = true;
        } catch (std::exception &exc) {
            error("Unknown error / {}", exc.what());
            error_exit("Exit with error");
        }
    }
}

const bool sju_debugger::handle_command(const std::vector<std::string> &args) {
    std::string choice;

    if (args[0] == "exit" || args[0] == "quit") {
        debug("bye");
        return false;
    } else if (args[0] == "help" || args[0] == "h") {
        debug("(h)elp\t\t\t| print help");
        debug("(r)un\t\t\t| run program while breakpoint");
        debug("(n)exti\t\t\t| run one instruction");
        debug("reset\t\t\t| reset ac register, pc");
        debug("reload\t\t\t| reset & reload instructions");
        debug("((b)reak)point\t| manage breakpoints");
        debug("(i)nfo\t\t\t| see info");
        debug("((d)isas)semble\t| disassemble");
    } else if (args[0] == "r" || args[0] == "run") {
        run_while_finish();
    } else if (args[0] == "n" || args[0] == "nexti") {
        if (is_finished) {
            cpu->reset();
            debug("Reset pc and ac register to start program.");
        }
        is_finished = cpu->execute(true);
        if (is_finished) {
            debug("Program ended.");
        }
    } else if (args[0] == "reset") {
        cpu->reset();
        debug("Reset");
    } else if (args[0] == "reload") {
        cpu->clear();
        cpu->setup_memory(original_instructions);
        debug("Reload");
    } else if (args[0] == "b" || args[0] == "break" || args[0] == "breakpoint") {
        if (args.size() == 1) {
            error("Please input arguments");
            error("");
            error("b *[addr]");
            error("b (l)ist");
            error("b (r)emove [index]");
            error("b (c)lear");
            return true;
        }

        if (args[1].at(0) == '*') {
            std::string addr_str = args[1].substr(1);
            int addr = -1;
            try {
                addr = std::stoi(addr_str);
            } catch (std::invalid_argument &exc) {
                error("Invalid argument");
                return true;
            } catch (std::out_of_range &exc) {
                error("Out of range");
                return true;
            }

            if (addr < 0 || addr >= MEMORY_SIZE) {
                error("Out of range");
                return true;
            }

            if (std::find(breakpoints.begin(), breakpoints.end(), addr) != breakpoints.end()) {
                error("Already added");
                return true;
            }

            breakpoints.push_back(addr);
        } else if (args[1] == "l" || args[1] == "list") {
            for (int i = 0; i < breakpoints.size(); i++) {
                debug("{} : {}", i, breakpoints[i]);
            }
        } else if (args[1] == "r" || args[1] == "remove") {
            if (args.size() == 2) {
                error("Please input arguments");
                return true;
            }

            int index = std::stoi(args[2]);
            breakpoints.erase(breakpoints.begin() + index);
        } else if (args[1] == "c" || args[1] == "clear") {
            breakpoints.clear();
        } else {
            error("Please input arguments");
            error("");
            error("b *[addr]");
            error("b (l)ist");
            error("b (r)emove [index]");
            error("b (c)lear");
        }
    } else if (args[0] == "i" || args[0] == "info") {
        if (args.size() == 1) {
            debug("PC : {}", cpu->get_pc());
            debug("AC Register : {}", cpu->get_ac_register());
        } else if (args[1] == "h" || args[1] == "help") {
            debug("(i)nfo (h)elp\t\t\t\t\t| print info help");
            debug("(i)nfo\t\t\t\t\t\t\t| view pc & ac register");
            debug("(i)nfo (a)c\t\t\t\t\t\t| view ac register");
            debug("(i)nfo (p)c\t\t\t\t\t\t| view pc");
            debug("(i)nfo [addr]\t\t\t\t\t| view memory");
            debug("(i)nfo [begin addr]-[end addr]\t| view memory");
        } else if (args[1] == "p" || args[1] == "pc" || args[1] == "PC") {
            debug("PC : {}", cpu->get_pc());
        } else if (args[1] == "a" || args[1] == "ac" || args[1] == "AC") {
            debug("AC Register : {}", cpu->get_ac_register());
        } else {
            int base;
            if ((base = args[1].find('-')) != std::string::npos) {
                int begin, end;
                try {
                    begin = std::stoi(args[1].substr(0, base));
                    end = std::stoi(args[1].substr(base + 1));
                } catch (std::invalid_argument &exc) {
                    error("Invalid argument");
                    return true;
                } catch (std::out_of_range &exc) {
                    error("Out of range");
                    return true;
                }

                for (int i = begin; i <= end; i++) {
                    debug("{} : {:03d}", i, (*cpu)[i]);
                }
            } else {
                try {
                    base = std::stoi(args[1]);
                } catch (std::invalid_argument &exc) {
                    error("Invalid argument");
                    return true;
                } catch (std::out_of_range &exc) {
                    error("Out of range");
                    return true;
                }
                debug("{} : {:03d}", base, (*cpu)[base]);
            }
        }
    } else if (args[0] == "d" || args[0] == "disas" || args[0] == "disassemble") {
        if (args.size() == 1) {
            error("Please input arguments");
            error("");
            error("d [addr]");
            error("d [begin addr]-[end addr]");
            return true;
        }
        int base;
        if ((base = args[1].find('-')) != std::string::npos) {
            int begin, end;
            try {
                begin = std::stoi(args[1].substr(0, base));
                end = std::stoi(args[1].substr(base + 1));
            } catch (std::invalid_argument &exc) {
                error("Invalid argument");
                return true;
            } catch (std::out_of_range &exc) {
                error("Out of range");
                return true;
            }

            for (int i = begin; i <= end; i++) {
                debug("{} : {:03d}\t\t{}", i, (*cpu)[i], sju_assembler::disassemble((*cpu)[i]));
            }
        } else {
            try {
                base = std::stoi(args[1]);
            } catch (std::invalid_argument &exc) {
                error("Invalid argument");
                return true;
            } catch (std::out_of_range &exc) {
                error("Out of range");
                return true;
            }
            debug("{} : {:03d}\t\t{}", base, (*cpu)[base], sju_assembler::disassemble((*cpu)[base]));
        }
    }
    return true;
}

const bool sju_debugger::check_breakpoints() {
    for (auto i : breakpoints) {
        if (cpu->get_pc() == i) {
            debug("Breakpoint at {}", i);
            return true;
        }
    }
    return false;
}

void sju_debugger::run_while_finish() {
    if (is_finished) {
        cpu->reset();
        debug("Reset pc and ac register to start program.");
        is_finished = false;
    }
    if (!is_finished) {
        do {
            is_finished = cpu->execute(true);
            if (is_finished) {
                debug("Program ended.");
            }
        } while (!is_finished && !check_breakpoints());
    }
}