#pragma once
#include "LogEntry.h"
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <vector>

// Strategy: how a raw text line becomes a LogEntry.
class ILineParser
{
public:
    virtual ~ILineParser() = default;
    virtual std::optional<LogEntry> parse(const std::string& line, int lineNumber) const = 0;
};

// Parser driven by a regex with groups: 1 = timestamp, 2 = level, 3 = message.
class RegexLineParser : public ILineParser
{
public:
    explicit RegexLineParser(const std::string& pattern);
    std::optional<LogEntry> parse(const std::string& line, int lineNumber) const override;

private:
    std::regex m_regex;
};

// Factory: creates a parser by format name.
class ParserFactory
{
public:
    static std::unique_ptr<ILineParser> create(const std::string& format);
    static std::vector<std::string> formats();
};
