#include "Stats.h"
#include <algorithm>
#include <cctype>

void TopMessages::onEntry(const LogEntry& e)
{
    std::string key;
    for (char c : e.message)
    {
        bool digit = std::isdigit(static_cast<unsigned char>(c)) != 0;
        if (!digit)
            key += c;
        else if (key.empty() || key.back() != '#')
            key += '#';
    }
    ++m_counts[key];
}

std::vector<std::pair<std::string, int>> TopMessages::top(size_t n) const
{
    std::vector<std::pair<std::string, int>> v(m_counts.begin(), m_counts.end());
    std::stable_sort(v.begin(), v.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
    if (v.size() > n)
        v.resize(n);
    return v;
}
