/*****************************************************************************

  tgd-info

  Show metadata of all layers or the named layer from a tile file.

  Reads from the specified input file and writes metadata to stdout. If the
  option -n/--name was no specified, all layers are examined, otherwise only
  the layers with the specified name.

  Examples:

  tgd-info input.tgd

  tgd-info input.tgd -n roads

*****************************************************************************/

#include <tgd_header/buffer.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/reader.hpp>
#include <tgd_header/stream.hpp>

#include <clara.hpp>

#include <iostream>

int main(int argc, char *argv[]) {
    std::string input_file_name;
    std::string layer_name;
    bool help = false;

    const auto cli
        = clara::Opt(layer_name, "name")
            ["-n"]["--name"]
            ("layer name")
        | clara::Help(help)
        | clara::Arg(input_file_name, "FILE")
            ("data");

    const auto result = cli.parse(clara::Args(argc, argv));
    if (!result) {
        std::cerr << "Error in command line: " << result.errorMessage() << '\n';
        return 2;
    }

    if (help) {
        std::cout << "Show layer metadata.\n\n";
        std::cout << cli;
        return 0;
    }

    if (input_file_name.empty()) {
        std::cerr << "Missing input file. Try 'tgd-info -h'.\n";
        return 2;
    }

    tgd_header::file_source source{input_file_name};
    tgd_header::reader<decltype(source)> reader{source};

    while (auto& layer = reader.next_layer()) {
        if (layer_name.empty() || layer.has_name(layer_name)) {
            reader.read_content();
            layer.decode_content();
            std::cout << "LAYER " << layer.name() << '\n';
            std::cout << "  tile (zoom/x/y): " << layer.tile() << '\n';
            std::cout << "  content type:    " << layer.content_type() << '\n';
            std::cout << "  compression:     " << layer.compression_type() << '\n';
            std::cout << "  compressed size: " << layer.wire_content_length() << '\n';
            std::cout << "  original size:   " << layer.content_length() << "\n\n";
        }
    }

    return 0;
}

