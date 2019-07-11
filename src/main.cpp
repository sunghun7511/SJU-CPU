#include "include.hpp"
#include "sju_assembler.hpp"
#include "sju_debugger.hpp"
#include "sju_instruction_parser.hpp"
#include "sju_cpu.hpp"

static void run(std::vector<int> &instructions, bool is_debug);

int main(int argc, char *argv[]) {
    po::variables_map map;
    std::string file;
    bool is_debug = false, result;

    log::set_level(log::level::info);
    // log::set_pattern("%^[%H:%M:%S %z] [%L] [thread-%t] %v%$");
    log::set_pattern("%^[%H:%M:%S] [%L] %v%$");

    po::options_description description("=-=-=-= SJUCPU =-=-=-=");

    description.add_options()
            ("help,h", "Show help")
            ("instructions,i", "Show available instructions")
            ("file,f", po::value<std::string>(&file)->value_name("FILE"), "Set target file for run")
            ("assemble", "Just assemble file")
            ("debug,d", "Enable debug mode");

    po::store(po::parse_command_line(argc, argv, description), map);
    po::notify(map);

    if (map.count("debug")) {
        is_debug = true;
        log::set_level(log::level::debug);
    }

    if (map.count("help")) {
        std::cout << description << std::endl;
    } else if (map.count("instructions")) {
        sju_instruction_parser::show_instructions();
    } else if (map.count("file")) {
        std::vector<section> sections;
        std::vector<int> instructions;

        sju_instruction_parser parser;

        debug("Start parse sections from file");
        result = parser.parse(file, sections);
        if (!result) {
            error_exit("Parsing failed");
        }

        debug("Start assemble process");
        result = sju_assembler::assemble(sections, instructions);
        if (!result) {
            error_exit("Assemble failed..");
        }

        if (map.count("assemble")) {
            for (auto i : instructions) {
                std::cout << std::setw(3) << std::setfill('0') << std::right << i << std::endl;
            }
        } else {
            debug("Run instructions");
            run(instructions, is_debug);
        }
    } else {
        std::cout << description << std::endl;
        error_exit("You need to input some arguments");
    }

    return 0;
}

void run(std::vector<int> &instructions, bool is_debug) {
    sju_cpu cpu;

    cpu.setup_memory(instructions);
    if (is_debug) {
        sju_debugger dbg;

        debug("Start debug mode");
        dbg.attach(cpu, instructions);
        dbg.interactive();
        debug("debug mode finished");
    } else {
        try {
            cpu.execute(false);
        } catch (segmentation_fault_exception &exc) {
            error("Segmentation fault / {}", exc.what());
        } catch (unknown_instruction_exception &exc) {
            error("Unknown instruction error / {}", exc.what());
        } catch (std::exception &exc) {
            error("Unknown error / {}", exc.what());
            error_exit("Exit with error");
        }
        debug("Finished");
    }
}

inline void error_exit(const char *const message, int code) {
    error(message);
    std::exit(code);
}

segmentation_fault_exception::segmentation_fault_exception() : segmentation_fault_exception("Unknown Error") {}

segmentation_fault_exception::segmentation_fault_exception(const std::string &reason) : reason(reason) {}

const char *segmentation_fault_exception::what() const noexcept {
    return this->reason.c_str();
}

unknown_instruction_exception::unknown_instruction_exception() : unknown_instruction_exception("Unknown Error") {}

unknown_instruction_exception::unknown_instruction_exception(const std::string &reason) : reason(reason) {}

const char *unknown_instruction_exception::what() const noexcept {
    return this->reason.c_str();
}
