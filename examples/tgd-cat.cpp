/*****************************************************************************

  tgd-cat

  Concatenate layers into a tile.

  Reads layers from the input files and writes them out into the output
  file (or stdout if no output file was specified). Files with suffix .tgd
  are copied to the output. Files with .png or .jpg suffix are written
  uncompressed, .mvt files are written compressed, all other files are
  written uncompressed unless the -c/--compression option was specified.

  Examples:

  tgd-cat roads.mvt sat.png -o tile.tgd

  tgd-cat streets.tgd buildings.tgd shops.tgd -o tile.tgd

*****************************************************************************/

#include <tgd_header/file_sink.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/string_sink.hpp>

#include <clara.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * Split a file name into the base name and the suffix.
 */
static std::pair<std::string, std::string> split_filename(const std::string& filename) {
    std::pair<std::string, std::string> file;

    const auto pos = filename.find_first_of('.');
    file.first = filename.substr(0, pos);
    if (pos != std::string::npos) {
        file.second = filename.substr(pos + 1);
    }

    return file;
}

/**
 * Read complete contents of the specified file into a buffer.
 */
static tgd_header::buffer read_file(const std::string& filename) {
    tgd_header::file_source source{filename};
    return source.read(source.file_size());
}

static void write_layer(const std::string& filename, tgd_header::file_sink& output_file, const tgd_header::tile_address& tile, tgd_header::layer_compression_type compression) {
    tgd_header::layer layer;

    const auto f = split_filename(filename);

    if (f.second == "tgd") {
        output_file.write(read_file(filename));
        return;
    }

    if (f.second == "png") {
        layer.set_content_type(tgd_header::layer_content_type::png);
    } else if (f.second == "jpg") {
        layer.set_content_type(tgd_header::layer_content_type::jpeg);
    } else if (f.second == "mvt") {
        layer.set_content_type(tgd_header::layer_content_type::vt2);
        layer.set_compression_type(tgd_header::layer_compression_type::zlib);
    } else {
        layer.set_compression_type(compression);
    }

    layer.set_name(tgd_header::buffer{f.first.data(), f.first.size()});

    layer.set_tile(tile);

    layer.set_content(read_file(filename));

    layer.write(output_file);
}

int main(int argc, char *argv[]) {
    std::vector<std::string> input_files;
    std::string output_file_name;
    uint32_t x = 0;
    uint32_t y = 0;
    unsigned int zoom = 0;
    bool help = false;
    bool want_compression = false;
    bool verbose = false;

    const auto cli
        = clara::Opt(zoom, "zoom")
            ["-z"]["--zoom"]
            ("set zoom")
        | clara::Opt(x, "x")
            ["-x"]
            ("set x")
        | clara::Opt(y, "y")
            ["-y"]
            ("set y")
        | clara::Opt(want_compression)
            ["-c"]["--compression"]
            ("enable compression")
        | clara::Opt(verbose)
            ["-v"]["--verbose"]
            ("enable verbose output")
        | clara::Opt(output_file_name, "file")
            ["-o"]["--output"]
            ("output file (default: stdout)")
        | clara::Help(help)
        | clara::Arg(input_files, "FILE").required()
            ("data");

    const auto result = cli.parse(clara::Args(argc, argv));
    if (!result) {
        std::cerr << "Error in command line: " << result.errorMessage() << '\n';
        return 2;
    }

    if (help) {
        std::cout << "Concatenate layers into a tile.\n\n";
        std::cout << cli;
        return 0;
    }

    tgd_header::layer_compression_type compression =
        want_compression ? tgd_header::layer_compression_type::zlib
                         : tgd_header::layer_compression_type::uncompressed;

    tgd_header::tile_address tile{static_cast<uint8_t>(zoom), x, y};

    tgd_header::file_sink sink{output_file_name};

    for (const auto& filename : input_files) {
        if (verbose) {
            std::cerr << "Reading " << filename << '\n';
        }
        write_layer(filename, sink, tile, compression);
    }
}

