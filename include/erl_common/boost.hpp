#pragma once

#ifdef ERL_USE_BOOST
    #include "eigen.hpp"
    #include "logging.hpp"
    #include "opencv.hpp"

    #include <boost/program_options.hpp>

    #include <string>
    #include <vector>

namespace erl::common::program_options {
    // Functionality for parsing program options using boost::program_options

    namespace po = boost::program_options;

    [[nodiscard]] inline std::string
    GetBoostOptionName(const std::string &prefix, const std::string &name) {
        if (prefix.empty()) { return name; }
        return prefix + "." + name;
    }

    struct ParseOptionBase {
        virtual ~ParseOptionBase() = default;  // enable polymorphism
    };

    template<typename M>
    struct ParseOption;

    struct ProgramOptionsData {
        std::vector<std::string> args;
        po::options_description desc;
        po::variables_map vm;
        // Map from option name to its parser.
        // This is used to keep the option parsers alive across multiple Parse() calls.
        // Some parsers need to store intermediate data (e.g., for Eigen matrices).
        // So we need to keep them in memory.
        std::unordered_map<std::string, std::shared_ptr<ParseOptionBase>> option_parsers;

        template<typename M>
        std::shared_ptr<ParseOption<M>>
        GetOptionParser(const std::string &option_name) {
            using T = ParseOption<M>;
            if (const auto it = option_parsers.find(option_name); it != option_parsers.end()) {
                auto parser = std::dynamic_pointer_cast<T>(it->second);
                return parser;
            }
            auto parser = std::make_shared<T>();
            option_parsers[option_name] = parser;
            return parser;
        }

        void
        Parse() {
            if (args.empty()) { return; }
            auto parsed = po::command_line_parser(args).options(desc).allow_unregistered().run();
            po::store(parsed, vm);
            po::notify(vm);
            vm.clear();  // clear variables_map to allow reparsing and avoid notifying twice.
            args = po::collect_unrecognized(parsed.options, po::include_positional);
        }
    };

    template<typename T>
    struct ParseOption : ParseOptionBase {
        void
        Run(ProgramOptionsData &po_data, const std::string &option_name, T &member) {
            // do not set default_value here to avoid overwriting parsed values because
            // po_data.vm is cleared after each Parse() call.
            po_data.desc.add_options()(option_name.c_str(), po::value<T>(&member));
            po_data.Parse();
        }
    };

    template<typename T>
    struct ParseOption<std::vector<T>> : ParseOptionBase {
        void
        Run(ProgramOptionsData &po_data, const std::string &option_name, std::vector<T> &member) {
            po_data.desc.add_options()(
                option_name.c_str(),
                po::value<std::vector<T>>(&member)->multitoken());
            po_data.Parse();
        }
    };

    template<typename T1, typename T2>
    struct ParseOption<std::pair<T1, T2>> : ParseOptionBase {
        void
        Run(ProgramOptionsData &po_data,
            const std::string &option_name,
            std::pair<T1, T2> &member) {

            std::string first_option_name = option_name + ".first";
            std::string second_option_name = option_name + ".second";
            po_data.GetOptionParser<T1>(first_option_name)
                ->Run(po_data, first_option_name, member.first);
            po_data.GetOptionParser<T2>(second_option_name)
                ->Run(po_data, second_option_name, member.second);
        }
    };

    template<typename Scalar_, int Rows_, int Cols_, int Options_, int MaxRows_, int MaxCols_>
    struct ParseOption<Eigen::Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>>
        : ParseOptionBase {
        std::vector<Scalar_> values;

        void
        Run(ProgramOptionsData &po_data,
            const std::string &option_name,
            Eigen::Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_> &member) {

            // add options
            values.reserve(member.size());
            auto options = po_data.desc.add_options();

            options(
                option_name.c_str(),
                po::value<std::vector<Scalar_>>(&values)->multitoken(),
                member.IsRowMajor ? "Values of the Eigen matrix, the order should be row-major"
                                  : "Values of the Eigen matrix, the order should be column-major");

            long n_rows = Rows_;
            long n_cols = Cols_;
            if (Rows_ == Eigen::Dynamic) {
                n_rows = member.rows();
                options(
                    (option_name + ".rows").c_str(),
                    po::value<long>(&n_rows)->default_value(n_rows),
                    "Number of rows for dynamic-size Eigen matrix");
            }
            if (Cols_ == Eigen::Dynamic) {
                n_cols = member.cols();
                options(
                    (option_name + ".cols").c_str(),
                    po::value<long>(&n_cols)->default_value(n_cols),
                    "Number of columns for dynamic-size Eigen matrix");
            }

            // parse options
            po_data.Parse();

            if (values.empty()) { return; }

            // post-processing
            using Mat = Eigen::Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>;
            if (Rows_ > 0 && Cols_ > 0) {
                ERL_ASSERTM(
                    values.size() == static_cast<std::size_t>(Rows_ * Cols_),
                    "expecting {} values for option {}, got {}",
                    Rows_ * Cols_,
                    option_name,
                    values.size());
                member = Eigen::Map<Mat>(values.data(), Rows_, Cols_);
                return;
            }
            if (Rows_ > 0) {
                ERL_ASSERTM(
                    values.size() % Rows_ == 0,
                    "expecting multiple of {} values for option {} ({} x -1), got {}",
                    Rows_,
                    option_name,
                    Rows_,
                    values.size());
                const int cols = static_cast<int>(values.size()) / Rows_;
                ERL_ASSERTM(
                    n_cols <= 0 || cols == n_cols,
                    "mismatched number of columns: {} vs {}",
                    cols,
                    n_cols);
                member.resize(Rows_, cols);
                member = Eigen::Map<Mat>(values.data(), Rows_, cols);
                return;
            }
            if (Cols_ > 0) {
                ERL_ASSERTM(
                    values.size() % Cols_ == 0,
                    "expecting multiple of {} values for option {} (-1 x {}), got {}",
                    Cols_,
                    option_name,
                    Cols_,
                    values.size());
                const int rows = static_cast<int>(values.size()) / Cols_;
                ERL_ASSERTM(
                    n_rows <= 0 || rows == n_rows,
                    "mismatched number of rows: {} vs {}",
                    rows,
                    n_rows);
                member.resize(rows, Cols_);
                member = Eigen::Map<Mat>(values.data(), rows, Cols_);
                return;
            }
            ERL_ASSERTM(
                n_rows > 0 && n_cols > 0,
                "For option {} with fully dynamic size (-1 x -1), both rows and cols must be "
                "specified and > 0",
                option_name);
            const int size = static_cast<int>(values.size());
            ERL_ASSERTM(
                size == n_rows * n_cols,
                "expecting {} values for option {} ({} x {}), got {}",
                n_rows * n_cols,
                option_name,
                n_rows,
                n_cols,
                size);
            member.resize(n_rows, n_cols);
            member = Eigen::Map<Mat>(values.data(), n_rows, n_cols);
        }
    };

    #ifdef ERL_USE_OPENCV

    template<>
    struct ParseOption<cv::Scalar> : ParseOptionBase {
        std::vector<double> values;

        void
        Run(ProgramOptionsData &po_data, const std::string &option_name, cv::Scalar &member) {
            po_data.desc.add_options()(
                option_name.c_str(),
                po::value<std::vector<double>>(&values)->multitoken());
            po_data.Parse();

            if (!values.empty()) { return; }

            ERL_ASSERTM(
                values.size() <= 4,
                "expecting up to 4 values for option {}, got {}",
                option_name,
                values.size());
            for (size_t i = 0; i < values.size(); ++i) { member[i] = values[i]; }
        }
    };

    #endif  // ERL_USE_OPENCV

}  // namespace erl::common::program_options

    #define ERL_PARSE_BOOST_OPTION_ENUM(T)                                                        \
        template<>                                                                                \
        struct erl::common::program_options::ParseOption<T>                                       \
            : erl::common::program_options::ParseOptionBase {                                     \
            void                                                                                  \
            Run(erl::common::program_options::ProgramOptionsData &po_data,                        \
                const std::string &option_name,                                                   \
                T &member) {                                                                      \
                using yaml_convert = YAML::convert<T>;                                            \
                auto option = yaml_convert::encode(member).as<std::string>();                     \
                po_data.desc.add_options()(option_name.c_str(), po::value<std::string>(&option)); \
                po_data.Parse();                                                                  \
                yaml_convert::decode(YAML::Node(option), member);                                 \
            }                                                                                     \
        }

#else
    #define ERL_PARSE_BOOST_OPTION_ENUM(T)
#endif  // ERL_USE_BOOST
