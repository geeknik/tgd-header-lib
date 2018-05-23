/*****************************************************************************

  tgd-filter

  Filter tile files by name and/or content type.

  Reads from the input file and writes all matching layers to the output file
  (or stdout if no output file was specified).

  Examples:

  tgd-filter input.tgd -o output.tgd -n roads # all layers named "roads"

  tgd-filter input.tgd -o output.tgd -t png # all embedded png files

*****************************************************************************/

#include <tgd_header/file_sink.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/reader.hpp>
#include <tgd_header/stream.hpp>

#include <clara.hpp>

#include <iostream>
#include <string>

class matcher {

    std::string m_name;
    tgd_header::layer_content_type m_type;

public:

    matcher(const std::string& name, tgd_header::layer_content_type type) :
        m_name(name),
        m_type(type) {
    }

    bool operator()(const tgd_header::layer& layer) {
        if (m_name.empty() && m_type == tgd_header::layer_content_type::unknown) {
            return true;
        }
        if (m_type != tgd_header::layer_content_type::unknown && m_type == layer.content_type()) {
            return true;
        }
        if (!m_name.empty() && m_name == layer.name().data()) {
            return true;
        }
        return false;
    }

}; // class matcher

static tgd_header::layer_content_type parse_content_type(const std::string& content_type) {
    if (content_type.empty()) {
        return tgd_header::layer_content_type::unknown;
    } else if (content_type == "png") {
        return tgd_header::layer_content_type::png;
    } else if (content_type == "jpg") {
        return tgd_header::layer_content_type::jpeg;
    } else if (content_type == "mvt") {
        return tgd_header::layer_content_type::vt2;
    }

    throw std::runtime_error{"unknown content type: " + content_type};
}

int main(int argc, char *argv[]) {
    std::string input_file_name;
    std::string output_file_name;
    std::string layer_name;
    std::string content_type;
    bool help = false;

    const auto cli
        = clara::Opt(layer_name, "name")
            ["-n"]["--name"]
            ("layer name")
        | clara::Opt(content_type, "type")
            ["-t"]["--type"]
            ("content type")
        | clara::Opt(output_file_name, "file")
            ["-o"]["--output"]
            ("output file (default: stdout)")
        | clara::Help(help)
        | clara::Arg(input_file_name, "FILE").required()
            ("data");

    const auto result = cli.parse(clara::Args(argc, argv));
    if (!result) {
        std::cerr << "Error in command line: " << result.errorMessage() << '\n';
        return 2;
    }

    if (help) {
        std::cout << "Filter input file by name and/or content type.\n\n";
        std::cout << cli;
        return 0;
    }

    matcher match{layer_name, parse_content_type(content_type)};

    tgd_header::file_source source{input_file_name};
    tgd_header::reader<decltype(source)> reader{source};

    tgd_header::file_sink output_file{output_file_name};

    while (auto& layer = reader.next_layer()) {
        if (match(layer)) {
            reader.read_content();
            layer.write(output_file);
        }
    }
}

