#pragma once
#include "LogEntry.h"
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <vector>

// Strategy: decides whether an entry is kept.
class IFilter
{
public:
    virtual ~IFilter() = default;
    virtual bool accept(const LogEntry& e) const = 0;
};

class LevelFilter : public IFilter
{
public:
    explicit LevelFilter(std::set<std::string> levels) : m_levels(std::move(levels)) {}
    bool accept(const LogEntry& e) const override { return m_levels.count(e.level) > 0; }

private:
    std::set<std::string> m_levels;
};

class RegexFilter : public IFilter
{
public:
    explicit RegexFilter(const std::string& pattern) : m_regex(pattern, std::regex::ECMAScript | std::regex::icase) {}
    bool accept(const LogEntry& e) const override { return std::regex_search(e.message, m_regex); }

private:
    std::regex m_regex;
};

// Empty bound = unbounded. Bounds are inclusive.
class TimeRangeFilter : public IFilter
{
public:
    TimeRangeFilter(std::string from, std::string to) : m_from(std::move(from)), m_to(std::move(to)) {}
    bool accept(const LogEntry& e) const override
    {
        return (m_from.empty() || e.timestamp >= m_from) && (m_to.empty() || e.timestamp <= m_to);
    }

private:
    std::string m_from, m_to;
};

// Composite: a group of filters that behaves like one filter.
class AndFilter : public IFilter
{
public:
    void add(std::unique_ptr<IFilter> f) { m_children.push_back(std::move(f)); }
    bool accept(const LogEntry& e) const override
    {
        for (const auto& f : m_children)
            if (!f->accept(e))
                return false;
        return true;
    }

private:
    std::vector<std::unique_ptr<IFilter>> m_children;
};

// Decorator: inverts another filter.
class NotFilter : public IFilter
{
public:
    explicit NotFilter(std::unique_ptr<IFilter> inner) : m_inner(std::move(inner)) {}
    bool accept(const LogEntry& e) const override { return !m_inner->accept(e); }

private:
    std::unique_ptr<IFilter> m_inner;
};
