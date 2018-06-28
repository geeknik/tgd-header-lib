/*****************************************************************************

  tgd-filter

  Filter tile files by name and/or content type.

  Reads from the input file and writes all matching layers to the output file
  (or stdout if no output file was specified). Layers must fulfill all
  requirements to match.

  Examples:

  tgd-filter input.tgd -o output.tgd -n roads # all layers named "roads"

  tgd-filter input.tgd -o output.tgd -t png # all embedded png files

  tgd-filter input.tgd -o output.tgd -z 12 -n water # all layers with zoom
                                                    # level 12 and name "water"

*****************************************************************************/

#include <tgd_header/file_sink.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/reader.hpp>
#include <tgd_header/stream.hpp>

#include <clara.hpp>

#include <cstdint>
#include <iostream>
#include <string>

static constexpr const std::uint8_t any_zoom = 255;

class matcher {

    std::string m_name;
    tgd_header::layer_content_type m_type;
    std::uint8_t m_zoom;

public:

    matcher(std::string name, tgd_header::layer_content_type type, std::uint8_t zoom) :
        m_name(std::move(name)),
        m_type(type),
        m_zoom(zoom) {
    }

    bool operator()(const tgd_header::layer& layer) noexcept {
        if (m_name.empty() && m_type == tgd_header::layer_content_type::unknown && m_zoom == any_zoom) {
            return true;
        }

        if (m_zoom != any_zoom && m_zoom != layer.tile().zoom()) {
            return false;
        }
        if (m_type != tgd_header::layer_content_type::unknown && m_type != layer.content_type()) {
            return false;
        }
        if (!m_name.empty() && m_name != layer.name().data()) {
            return false;
        }

        return true;
    }

}; // class matcher

static tgd_header::layer_content_type parse_content_type(const std::string& content_type) {
    if (content_type.empty()) {
        return tgd_header::layer_content_type::unknown;
    }

    if (content_type == "png") {
        return tgd_header::layer_content_type::png;
    }

    if (content_type == "jpg") {
        return tgd_header::layer_content_type::jpeg;
    }

    if (content_type == "mvt") {
        return tgd_header::layer_content_type::vt2;
    }

    throw std::runtime_error{"unknown content type: " + content_type};
}

int main(int argc, char *argv[]) {
    std::string input_file_name;
    std::string output_file_name;
    std::string layer_name;
    std::string content_type;
    int zoom = any_zoom;
    bool help = false;
    bool verbose = false;

    const auto cli
        = clara::Opt(layer_name, "name")
            ["-n"]["--name"]
            ("filter by layer name")
        | clara::Opt(content_type, "type")
            ["-t"]["--type"]
            ("filter by content type")
        | clara::Opt(output_file_name, "file")
            ["-o"]["--output"]
            ("output file (default: stdout)")
        | clara::Opt(verbose)
            ["-v"]["--verbose"]
            ("verbose output")
        | clara::Opt(zoom, "zoom")
            ["-z"]["--zoom"]
            ("filter by zoom level (default: any)")
        | clara::Help(help)
        | clara::Arg(input_file_name, "FILE")
            ("data");

    const auto result = cli.parse(clara::Args(argc, argv));
    if (!result) {
        std::cerr << "Error in command line: " << result.errorMessage() << '\n';
        return 2;
    }

    if (help) {
        std::cout << "Filter tile input file by name, content type, and/or zoom level.\n\n";
        std::cout << cli;
        return 0;
    }

    if (input_file_name.empty()) {
        std::cerr << "Missing input file. Try 'tgd-filter -h'.\n";
        return 2;
    }

    if (output_file_name.empty()) {
        std::cerr << "Missing -o/--output option. Try 'tgd-filter -h'.\n";
        return 2;
    }

    if (zoom != 255 && (zoom < 0 || zoom > 30)) {
        std::cerr << "Invalid value for -z/--zoom option.\n";
        return 2;
    }

    matcher match{layer_name, parse_content_type(content_type), std::uint8_t(zoom)};

    tgd_header::file_source source{input_file_name};
    tgd_header::reader<decltype(source)> reader{source};

    tgd_header::file_sink output_file{output_file_name};

    while (auto& layer = reader.next_layer()) {
        if (verbose) {
            std::cout << "Considering layer '"
                      << layer.name().data()
                      << "' of type "
                      << layer.content_type()
                      << " in tile "
                      << layer.tile();
        }
        if (match(layer)) {
            if (verbose) {
                std::cout << ": MATCHED\n";
            }
            reader.read_content();
            layer.write(output_file);
        } else if (verbose) {
            std::cout << ": DOES NOT MATCH\n";
        }
    }
}

