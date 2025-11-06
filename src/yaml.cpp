#include "erl_common/yaml.hpp"

#include <any>
#include <fstream>
#include <stack>

namespace erl::common {

    bool
    YamlableBase::operator==(const YamlableBase &other) const {
        const std::string yaml_str = AsYamlString();
        const std::string other_yaml_str = other.AsYamlString();
        // if the YAML string is the same, the object is the same in terms of saving and loading
        return yaml_str == other_yaml_str;
    }

    bool
    YamlableBase::operator!=(const YamlableBase &other) const {
        return !(*this == other);
    }

    bool
    YamlableBase::FromYamlString(const std::string &yaml_string) {
        const YAML::Node node = YAML::Load(yaml_string);
        return FromYamlNode(node);
    }

    std::string
    YamlableBase::AsYamlString() const {
        YAML::Emitter emitter;
        emitter.SetIndent(4);
        emitter.SetSeqFormat(YAML::Flow);
        emitter << AsYamlNode();
        return emitter.c_str();
    }

    bool
    YamlableBase::FromYamlFile(const std::string &yaml_file) {
        if (!std::filesystem::exists(yaml_file)) {
            ERL_WARN("File does not exist: {}", yaml_file);
            return false;
        }
        const auto node = YAML::LoadFile(yaml_file);
        return FromYamlNode(node);
    }

    void
    YamlableBase::AsYamlFile(const std::string &yaml_file) const {
        std::ofstream ofs(yaml_file);
        if (!ofs.good()) {
            ERL_WARN("Failed to open file: {}", yaml_file);
            return;
        }
        YAML::Emitter emitter(ofs);
        emitter.SetIndent(4);
        emitter.SetSeqFormat(YAML::Flow);
        emitter << AsYamlNode();
    }

    bool
    YamlableBase::Write(std::ostream &s) const {
        if (!s.good()) { return false; }
        const std::string yaml_str = AsYamlString() + "\n";
        const auto len = static_cast<std::streamsize>(yaml_str.size());
        s.write(reinterpret_cast<const char *>(&len), sizeof(len));
        s.write(yaml_str.data(), len);
        return s.good();
    }

    bool
    YamlableBase::Read(std::istream &s) {
        if (!s.good()) { return false; }
        std::streamsize len;
        s.read(reinterpret_cast<char *>(&len), sizeof(len));
        std::string yaml_str(len, '\0');
        s.read(yaml_str.data(), len);
        return FromYamlString(yaml_str) && s.good();
    }

    bool
    YamlableBase::FromCommandLine(const std::vector<std::string> &args) {
        std::vector<const char *> argv_vec;
        argv_vec.reserve(args.size());
        for (const auto &arg: args) { argv_vec.push_back(arg.c_str()); }
        return FromCommandLine(static_cast<int>(args.size()), argv_vec.data());
    }

    bool
    YamlableBase::FromCommandLine(const int argc, char *argv[]) {
        std::vector<const char *> argv_vec;
        argv_vec.reserve(argc);
        for (int i = 0; i < argc; ++i) { argv_vec.push_back(argv[i]); }
        return FromCommandLine(argc, argv_vec.data());
    }

    bool
    YamlableBase::FromCommandLine(const int argc, const char *argv[]) {
#ifdef ERL_USE_BOOST
        program_options::ProgramOptionsData po_data;

        namespace po = boost::program_options;

        std::string config_file;
        po_data.desc.add_options()           //
            ("help,h", "Show help message")  //
            ("config", po::value<std::string>(&config_file), "Path to YAML config file");
        auto parsed =
            po::command_line_parser(argc, argv).options(po_data.desc).allow_unregistered().run();
        po::store(parsed, po_data.vm);
        po::notify(po_data.vm);
        const bool print_help = po_data.vm.count("help") > 0;

        if (!config_file.empty()) {
            ERL_ASSERTM(
                std::filesystem::exists(config_file),
                "Config file does not exist: {}",
                config_file);
            ERL_ASSERTM(FromYamlFile(config_file), "Failed to load config file: {}", config_file);
        }

        po_data.args = po::collect_unrecognized(parsed.options, po::include_positional);
        po_data.vm.clear();

        if (!FromCommandLineImpl(po_data, "")) { return false; }

        if (print_help) {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl
                      << po_data.desc << std::endl;
            exit(0);
        }
        return true;
#else
        (void) argc;
        (void) argv;
        ERL_ERROR("Not compiled with Boost. Cannot parse command line arguments.");
        return false;
#endif
    }

    void
    UpdateYamlNode(const YAML::Node &src, YAML::Node &dst, const bool ignore_unknown) {

        std::stack<std::pair<const YAML::Node &, YAML::Node>> node_stack;
        node_stack.emplace(src, dst);
        while (!node_stack.empty()) {
            auto [current_src, current_dst] = node_stack.top();
            node_stack.pop();

            ERL_ASSERTM(
                current_src.IsMap() && current_dst.IsMap(),
                "Both source and destination nodes must be maps.");

            for (const auto &item: current_src) {
                const auto &key = item.first.as<std::string>();
                const auto &src_value = item.second;

                if (!current_dst[key].IsDefined()) {
                    if (ignore_unknown) { continue; }
                    ERL_FATAL("Unknown key in YAML node: {}", key);
                }

                if (src_value.IsMap()) {
                    YAML::Node dst_value = current_dst[key];
                    ERL_ASSERTM(
                        dst_value.IsMap(),
                        "Destination node must be a map for key: {}",
                        key);
                    node_stack.emplace(src_value, dst_value);
                } else {
                    current_dst[key] = src_value;
                }
            }
        }
    }

}  // namespace erl::common
