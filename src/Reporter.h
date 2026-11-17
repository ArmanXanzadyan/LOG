#pragma once
#include "Analyzer.h"
#include "Stats.h"
#include <memory>
#include <ostream>
#include <string>

struct ReportData
{
    const AnalysisResult& result;
    const LevelCounter& levels;
    const HourlyCounter& hours;
    const TopMessages& top;
};

// Template Method: write() fixes the report order, subclasses fill the parts.
class Reporter
{
public:
    virtual ~Reporter() = default;
    void write(const ReportData& d, std::ostream& out) const
    {
        header(d, out);
        entries(d, out);
        summary(d, out);
        footer(out);
    }

protected:
    virtual void header(const ReportData&, std::ostream&) const {}
    virtual void entries(const ReportData& d, std::ostream& out) const = 0;
    virtual void summary(const ReportData&, std::ostream&) const {}
    virtual void footer(std::ostream&) const {}
};

class TextReporter : public Reporter
{
protected:
    void entries(const ReportData& d, std::ostream& out) const override;
    void summary(const ReportData& d, std::ostream& out) const override;
};

class CsvReporter : public Reporter
{
protected:
    void header(const ReportData& d, std::ostream& out) const override;
    void entries(const ReportData& d, std::ostream& out) const override;
};

class HtmlReporter : public Reporter
{
protected:
    void header(const ReportData& d, std::ostream& out) const override;
    void entries(const ReportData& d, std::ostream& out) const override;
    void summary(const ReportData& d, std::ostream& out) const override;
    void footer(std::ostream& out) const override;
};

class ReporterFactory
{
public:
    static std::unique_ptr<Reporter> create(const std::string& name);
};
