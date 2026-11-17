#include "Analyzer.h"
#include "Reporter.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

namespace
{
void usage()
{
    std::cout << "Usage: log_analyzer <file> [options]\n"
                 "  --format default|bracket        log line format (default: default)\n"
                 "  --level ERROR,WARNING           keep only these levels\n"
                 "  --grep <regex>                  keep messages matching regex\n"
                 "  --exclude <regex>               drop messages matching regex\n"
                 "  --from \"YYYY-MM-DD HH:MM:SS\"    start time (inclusive)\n"
                 "  --to   \"YYYY-MM-DD HH:MM:SS\"    end time (inclusive)\n"
                 "  --report text|csv|html          output type (default: text)\n"
                 "  --out <file>                    write report to file instead of console\n";
}

std::set<std::string> splitLevels(const std::string& s)
{
    std::set<std::string> r;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        for (auto& c : item)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        r.insert(item == "WARN" ? "WARNING" : item);
    }
    return r;
}
}

int main(int argc, char** argv)
{
    if (argc < 2 || std::string(argv[1]) == "--help")
    {
        usage();
        return argc < 2 ? 1 : 0;
    }

    std::string file = argv[1], format = "default", report = "text", outFile, from, to;
    auto filter = std::make_unique<AndFilter>();
    try
    {
        for (int i = 2; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (i + 1 >= argc)
                throw std::invalid_argument("Missing value for " + arg);
            std::string val = argv[++i];
            if (arg == "--format") format = val;
            else if (arg == "--level") filter->add(std::make_unique<LevelFilter>(splitLevels(val)));
            else if (arg == "--grep") filter->add(std::make_unique<RegexFilter>(val));
            else if (arg == "--exclude") filter->add(std::make_unique<NotFilter>(std::make_unique<RegexFilter>(val)));
            else if (arg == "--from") from = val;
            else if (arg == "--to") to = val;
            else if (arg == "--report") report = val;
            else if (arg == "--out") outFile = val;
            else throw std::invalid_argument("Unknown option " + arg);
        }
        if (!from.empty() || !to.empty())
            filter->add(std::make_unique<TimeRangeFilter>(from, to));

        std::ifstream in(file);
        if (!in)
            throw std::runtime_error("Cannot open " + file);

        auto reporter = ReporterFactory::create(report);
        Analyzer analyzer(ParserFactory::create(format), std::move(filter));
        LevelCounter levels;
        HourlyCounter hours;
        TopMessages top;
        analyzer.subscribe(&levels);
        analyzer.subscribe(&hours);
        analyzer.subscribe(&top);

        AnalysisResult result = analyzer.run(in);
        ReportData data{result, levels, hours, top};

        if (outFile.empty())
            reporter->write(data, std::cout);
        else
        {
            std::ofstream out(outFile);
            if (!out)
                throw std::runtime_error("Cannot write " + outFile);
            reporter->write(data, out);
            std::cout << "Report written to " << outFile << "\n";
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
