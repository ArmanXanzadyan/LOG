#pragma once
#include "Filter.h"
#include "Parser.h"
#include "Stats.h"
#include <istream>
#include <memory>
#include <vector>

struct AnalysisResult
{
    std::vector<LogEntry> matches;
    int totalLines = 0;
    int parsedLines = 0;
};

// Facade + pipeline: read -> parse -> filter -> notify observers -> collect.
class Analyzer
{
public:
    Analyzer(std::unique_ptr<ILineParser> parser, std::unique_ptr<IFilter> filter);
    void subscribe(IEntryObserver* observer) { m_observers.push_back(observer); }
    AnalysisResult run(std::istream& in);

private:
    std::unique_ptr<ILineParser> m_parser;
    std::unique_ptr<IFilter> m_filter;
    std::vector<IEntryObserver*> m_observers;
};
