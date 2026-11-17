#include "Analyzer.h"
#include <string>

Analyzer::Analyzer(std::unique_ptr<ILineParser> parser, std::unique_ptr<IFilter> filter)
    : m_parser(std::move(parser)), m_filter(std::move(filter))
{
}

AnalysisResult Analyzer::run(std::istream& in)
{
    AnalysisResult r;
    std::string line;
    while (std::getline(in, line))
    {
        ++r.totalLines;
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        auto entry = m_parser->parse(line, r.totalLines);
        if (!entry)
            continue;
        ++r.parsedLines;
        if (m_filter && !m_filter->accept(*entry))
            continue;

        for (auto* o : m_observers)
            o->onEntry(*entry);
        r.matches.push_back(std::move(*entry));
    }
    return r;
}
