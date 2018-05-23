/*****************************************************************************

  tgd-export

  Export the content of a layer from a tile file.

  Reads from the specified input file and writes to stdout. If the layer
  doesn't exist, nothing is written.

  Examples:

  tgd-export input.tgd buildings >buildings.layer

*****************************************************************************/

#include <tgd_header/buffer.hpp>
#include <tgd_header/file_sink.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/reader.hpp>

#include <iostream>
#include <string>

void print_usage() {
    std::cout << "tgd-export FILE LAYER\n";
    std::cout << "Write content of layer LAYER in the FILE to stdout.\n";
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
            tgd_header::file_sink out{"-"};
            out.write(layer.content());
        }
    }

    return 0;
}

