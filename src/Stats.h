#pragma once
#include "LogEntry.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

// Observer: gets notified about every accepted entry.
class IEntryObserver
{
public:
    virtual ~IEntryObserver() = default;
    virtual void onEntry(const LogEntry& e) = 0;
};

class LevelCounter : public IEntryObserver
{
public:
    void onEntry(const LogEntry& e) override { ++m_counts[e.level]; }
    const std::map<std::string, int>& counts() const { return m_counts; }

private:
    std::map<std::string, int> m_counts;
};

class HourlyCounter : public IEntryObserver
{
public:
    void onEntry(const LogEntry& e) override { ++m_counts[e.hour()]; }
    const std::map<int, int>& counts() const { return m_counts; }

private:
    std::map<int, int> m_counts;
};

// Most frequent messages, with digits masked so "id=17" and "id=42" group together.
class TopMessages : public IEntryObserver
{
public:
    void onEntry(const LogEntry& e) override;
    std::vector<std::pair<std::string, int>> top(size_t n) const;

private:
    std::map<std::string, int> m_counts;
};
