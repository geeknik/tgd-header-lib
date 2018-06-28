/*****************************************************************************

  tgd-export

  Export the content of the first layer from a tile file. If a layer name
  was specified the first layer of that name ies exported.

  Reads from the specified input file and writes to stdout. If the layer
  doesn't exist, nothing is written.

  Examples:

  tgd-export input.tgd buildings -o buildings.layer

  tgd-export input.tgd -o first.layer

*****************************************************************************/

#include <tgd_header/buffer.hpp>
#include <tgd_header/file_sink.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/reader.hpp>

#include <clara.hpp>

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    std::string input_file_name;
    std::string output_file_name;
    std::string layer_name;
    bool help = false;

    const auto cli
        = clara::Opt(layer_name, "layer name")
            ["-l"]["--layer"]
            ("layer to export (default: first layer)")
        | clara::Opt(output_file_name, "file")
            ["-o"]["--output"]
            ("output file (default: stdout)")
        | clara::Help(help)
        | clara::Arg(input_file_name, "FILE")
            ("data");

    const auto result = cli.parse(clara::Args(argc, argv));
    if (!result) {
        std::cerr << "Error in command line: " << result.errorMessage() << '\n';
        return 2;
    }

    if (help) {
        std::cout << "Export layer content.\n\n";
        std::cout << cli;
        return 0;
    }

    if (input_file_name.empty()) {
        std::cerr << "Missing input file. Try 'tgd-export -h'.\n";
        return 2;
    }

    if (output_file_name.empty()) {
        std::cerr << "Missing -o/--output option. Try 'tgd-export -h'.\n";
        return 2;
    }

    tgd_header::file_source source{input_file_name};
    tgd_header::reader<decltype(source)> reader{source};

    while (auto& layer = reader.next_layer()) {
        if (layer_name.empty() || layer.name().data() == layer_name) {
            reader.read_content();
            layer.decode_content();
            tgd_header::file_sink out{output_file_name};
            out.write(layer.content());
            return 0;
        }
    }

    return 0;
}

