/*****************************************************************************

  tgd-layer

  Show metadata of all layers from a tile file.

  Reads from the specified input file and writes metadata of all layers to
  stdout.

  Examples:

  tgd-layer input.tgd

*****************************************************************************/

#include <tgd_header/buffer.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/reader.hpp>
#include <tgd_header/stream.hpp>

#include <iostream>

void print_usage() {
    std::cout << "tgd-list FILE\n\n";
    std::cout << "Show metadata of all layers in the FILE.\n";
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_usage();
        return 2;
    }

    tgd_header::file_source source{argv[1]};
    tgd_header::reader<decltype(source)> reader{source};

    while (auto& layer = reader.next_layer()) {
        reader.read_content();
        layer.decode_content();
        std::cout << "LAYER " << layer.name().data() << '\n';
        std::cout << "  tile (zoom/x/y): " << layer.tile() << '\n';
        std::cout << "  content type:    " << layer.content_type() << '\n';
        std::cout << "  compression:     " << layer.compression_type() << '\n';
        std::cout << "  compressed size: " << layer.wire_content_length() << '\n';
        std::cout << "  original size:   " << layer.content_length() << "\n\n";
    }

    return 0;
}

