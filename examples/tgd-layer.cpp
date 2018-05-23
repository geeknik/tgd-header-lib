/*****************************************************************************

  tgd-layer

  Show metadata of a layer from a tile file.

  Reads from the specified input file and writes metadata of all layers with
  the specified name to stdout. If the layer doesn't exist, nothing is written.

  Examples:

  tgd-layer input.tgd buildings

*****************************************************************************/

#include <tgd_header/buffer.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/reader.hpp>
#include <tgd_header/stream.hpp>

#include <iostream>
#include <string>

void print_usage() {
    std::cout << "tgd-layer FILE LAYER\n";
    std::cout << "Show metadata of layer LAYER in the FILE.\n";
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage();
        return 2;
    }

    std::string name{argv[2]};

    tgd_header::file_source source{argv[1]};
    tgd_header::reader<decltype(source)> reader{source};

    while (auto& layer = reader.next_layer()) {
        if (layer.name().data() == name) {
            reader.read_content();
            layer.decode_content();
            std::cout << "LAYER " << layer.name().data() << '\n';
            std::cout << "  tile (zoom/x/y): " << layer.tile() << '\n';
            std::cout << "  content type:    " << layer.content_type() << '\n';
            std::cout << "  compression:     " << layer.compression_type() << '\n';
            std::cout << "  compressed size: " << layer.wire_content_length() << '\n';
            std::cout << "  original size:   " << layer.content_length() << "\n\n";
        }
    }

    return 0;
}

