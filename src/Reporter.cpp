#include "Reporter.h"
#include <stdexcept>

namespace
{
std::string csvEscape(const std::string& s)
{
    std::string r = "\"";
    for (char c : s)
        r += (c == '"') ? std::string("\"\"") : std::string(1, c);
    return r + "\"";
}

std::string htmlEscape(const std::string& s)
{
    std::string r;
    for (char c : s)
    {
        switch (c)
        {
        case '<': r += "&lt;"; break;
        case '>': r += "&gt;"; break;
        case '&': r += "&amp;"; break;
        case '"': r += "&quot;"; break;
        default: r += c;
        }
    }
    return r;
}
}

void TextReporter::entries(const ReportData& d, std::ostream& out) const
{
    for (const auto& e : d.result.matches)
        out << e.lineNumber << ": " << e.timestamp << " " << e.level << " " << e.message << "\n";
}

void TextReporter::summary(const ReportData& d, std::ostream& out) const
{
    out << "\n=== Summary ===\n";
    out << "Lines: " << d.result.totalLines << ", parsed: " << d.result.parsedLines
        << ", matched: " << d.result.matches.size() << "\n";
    out << "By level:\n";
    for (const auto& [level, n] : d.levels.counts())
        out << "  " << level << ": " << n << "\n";
    out << "By hour:\n";
    for (const auto& [hour, n] : d.hours.counts())
        out << "  " << (hour < 10 ? "0" : "") << hour << ":00  " << std::string(n, '#') << " " << n << "\n";
    out << "Top messages:\n";
    for (const auto& [msg, n] : d.top.top(5))
        out << "  " << n << " x " << msg << "\n";
}

void CsvReporter::header(const ReportData&, std::ostream& out) const
{
    out << "line,timestamp,level,message\n";
}

void CsvReporter::entries(const ReportData& d, std::ostream& out) const
{
    for (const auto& e : d.result.matches)
        out << e.lineNumber << "," << e.timestamp << "," << e.level << "," << csvEscape(e.message) << "\n";
}

void HtmlReporter::header(const ReportData&, std::ostream& out) const
{
    out << "<!doctype html><html><head><meta charset=\"utf-8\"><title>Log report</title><style>"
           "body{font-family:sans-serif;margin:24px}table{border-collapse:collapse}"
           "td,th{border:1px solid #ccc;padding:4px 8px;text-align:left}"
           ".ERROR{color:#b00020}.WARNING{color:#a15c00}</style></head><body><h1>Log report</h1>\n";
}

void HtmlReporter::entries(const ReportData& d, std::ostream& out) const
{
    out << "<table><tr><th>Line</th><th>Time</th><th>Level</th><th>Message</th></tr>\n";
    for (const auto& e : d.result.matches)
        out << "<tr class=\"" << e.level << "\"><td>" << e.lineNumber << "</td><td>" << e.timestamp << "</td><td>"
            << e.level << "</td><td>" << htmlEscape(e.message) << "</td></tr>\n";
    out << "</table>\n";
}

void HtmlReporter::summary(const ReportData& d, std::ostream& out) const
{
    out << "<h2>Summary</h2><p>Lines: " << d.result.totalLines << ", parsed: " << d.result.parsedLines
        << ", matched: " << d.result.matches.size() << "</p><ul>\n";
    for (const auto& [level, n] : d.levels.counts())
        out << "<li>" << level << ": " << n << "</li>\n";
    out << "</ul>\n";
}

void HtmlReporter::footer(std::ostream& out) const
{
    out << "</body></html>\n";
}

std::unique_ptr<Reporter> ReporterFactory::create(const std::string& name)
{
    if (name == "text")
        return std::make_unique<TextReporter>();
    if (name == "csv")
        return std::make_unique<CsvReporter>();
    if (name == "html")
        return std::make_unique<HtmlReporter>();
    throw std::invalid_argument("Unknown report type: " + name);
}
