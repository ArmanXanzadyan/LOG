#include "Parser.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace
{
std::string normalizeLevel(std::string level)
{
    std::transform(level.begin(), level.end(), level.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    if (level == "WARN")
        return "WARNING";
    if (level == "ERR")
        return "ERROR";
    return level;
}
}

RegexLineParser::RegexLineParser(const std::string& pattern)
    : m_regex(pattern, std::regex::ECMAScript | std::regex::icase)
{
}

std::optional<LogEntry> RegexLineParser::parse(const std::string& line, int lineNumber) const
{
    std::smatch m;
    if (!std::regex_match(line, m, m_regex))
        return std::nullopt;

    LogEntry e;
    e.timestamp = m[1].str();
    if (e.timestamp.size() > 10)
        e.timestamp[10] = ' ';  // unify "T" separator
    e.level = normalizeLevel(m[2].str());
    e.message = m[3].str();
    e.lineNumber = lineNumber;
    return e;
}

std::unique_ptr<ILineParser> ParserFactory::create(const std::string& format)
{
    // 2026-11-17 10:00:01.123 ERROR Something failed
    if (format == "default")
        return std::make_unique<RegexLineParser>(
            R"(^(\d{4}-\d{2}-\d{2}[ T]\d{2}:\d{2}:\d{2})(?:[.,]\d+)?\s+\[?(ERROR|ERR|WARNING|WARN|INFO|DEBUG)\]?:?\s+(.*)$)");
    // [2026-11-17 10:00:01] ERROR: Something failed
    if (format == "bracket")
        return std::make_unique<RegexLineParser>(
            R"(^\[(\d{4}-\d{2}-\d{2}[ T]\d{2}:\d{2}:\d{2})(?:[.,]\d+)?\]\s+(ERROR|ERR|WARNING|WARN|INFO|DEBUG):?\s+(.*)$)");
    throw std::invalid_argument("Unknown log format: " + format);
}

std::vector<std::string> ParserFactory::formats()
{
    return {"default", "bracket"};
}
