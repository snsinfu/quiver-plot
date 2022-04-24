#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include <getopt.hpp>

#include "plot.hpp"
#include "spec.hpp"


struct program_options
{
    bool                       help = false;
    std::optional<std::string> output;
    std::string                spec;
};

static void            show_usage();
static program_options parse_options(int argc, char** argv);
static quiver_spec     load_quiver_spec(std::string const& filename);


int
main(int argc, char** argv)
{
    try {
        program_options options;

        try {
            options = parse_options(argc, argv);
        } catch (std::exception const& err) {
            std::cerr << "error: " << err.what() << '\n';
            show_usage();
            return 1;
        }

        if (options.help) {
            show_usage();
            return 0;
        }

        auto spec = load_quiver_spec(options.spec);

        if (options.output) {
            spec.rendering.output = *options.output;
        }

        if (!spec.rendering.output) {
            spec.rendering.output = std::filesystem::path{options.spec}.replace_extension(".png");
        }

        produce_quiver_plot(spec);
    } catch (std::exception const& err) {
        std::cerr << "error: " << err.what() << '\n';
        return 1;
    }

    return 0;
}


void
show_usage()
{
    std::string const usage =
        "usage: quiver [-h] [-o output] spec\n"
        "\n"
        "  spec       JSON file specifying the quiver plot to produce\n"
        "\n"
        "options:\n"
        "  -o output  Output PNG image file name\n"
        "  -h         Print this help message and exit\n"
        "\n";
    std::cerr << usage;
}


program_options
parse_options(int argc, char** argv)
{
    program_options options;
    cxx::getopt getopt;

    for (int ch; (ch = getopt(argc, argv, "ho:")) != -1; ) {
        switch (ch) {
        case 'h':
            // Stop parsing options if help is requested.
            options.help = true;
            return options;

        case 'o':
            options.output = getopt.optarg;
            break;

        default:
            throw std::runtime_error{"unrecognized command-line option"};
        }
    }

    argc -= getopt.optind;
    argv += getopt.optind;

    // Require exactly one positional argument.
    if (argc != 1) {
        throw std::runtime_error{"spec file is not given"};
    }
    options.spec = argv[0];

    return options;
}


quiver_spec
load_quiver_spec(std::string const& filename)
{
    std::ifstream file{filename};
    std::string json;
    if (!std::getline(file, json, '\0')) {
        throw std::runtime_error{"failed to load spec file"};
    }
    return parse_quiver_spec(json);
}
