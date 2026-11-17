#include "Analyzer.h"
#include "Reporter.h"
#include <iostream>
#include <sstream>

static int g_failed = 0;
#define CHECK(cond)                                                                       \
    do                                                                                    \
    {                                                                                     \
        if (!(cond))                                                                      \
        {                                                                                 \
            std::cerr << "FAILED: " #cond " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            ++g_failed;                                                                   \
        }                                                                                 \
    } while (0)

static const char* kLog =
    "2026-11-17 09:15:00 INFO Server started\n"
    "2026-11-17 10:00:01.250 ERROR Connection 17 lost\n"
    "garbage line\n"
    "2026-11-17T10:30:00 WARN Disk 80% full\n"
    "2026-11-17 11:00:00 [ERROR] Connection 42 lost\n";

static void testParser()
{
    auto p = ParserFactory::create("default");
    auto e = p->parse("2026-11-17T10:30:00 WARN Disk full", 1);
    CHECK(e.has_value());
    CHECK(e->timestamp == "2026-11-17 10:30:00");
    CHECK(e->level == "WARNING");
    CHECK(e->hour() == 10);
    CHECK(!p->parse("not a log line", 2));

    auto b = ParserFactory::create("bracket");
    CHECK(b->parse("[2026-11-17 10:30:00] ERROR: boom", 1)->message == "boom");
}

static void testFilters()
{
    LogEntry e{"2026-11-17 10:00:00", "ERROR", "Connection lost", 1};
    CHECK(LevelFilter({"ERROR"}).accept(e));
    CHECK(!LevelFilter({"INFO"}).accept(e));
    CHECK(RegexFilter("conn.*lost").accept(e));
    CHECK(TimeRangeFilter("2026-11-17 09:00:00", "").accept(e));
    CHECK(!TimeRangeFilter("", "2026-11-17 09:00:00").accept(e));
    CHECK(!NotFilter(std::make_unique<RegexFilter>("lost")).accept(e));
}

static void testAnalyzer()
{
    auto f = std::make_unique<AndFilter>();
    f->add(std::make_unique<LevelFilter>(std::set<std::string>{"ERROR"}));
    Analyzer a(ParserFactory::create("default"), std::move(f));
    LevelCounter levels;
    HourlyCounter hours;
    TopMessages top;
    a.subscribe(&levels);
    a.subscribe(&hours);
    a.subscribe(&top);

    std::istringstream in(kLog);
    auto r = a.run(in);
    CHECK(r.totalLines == 5);
    CHECK(r.parsedLines == 4);
    CHECK(r.matches.size() == 2);
    CHECK(levels.counts().at("ERROR") == 2);
    CHECK(hours.counts().at(10) == 1);
    CHECK(top.top(1)[0].first == "Connection # lost");
    CHECK(top.top(1)[0].second == 2);

    std::ostringstream csv;
    ReporterFactory::create("csv")->write({r, levels, hours, top}, csv);
    CHECK(csv.str().rfind("line,timestamp,level,message\n", 0) == 0);
}

int main()
{
    testParser();
    testFilters();
    testAnalyzer();
    std::cout << (g_failed ? "SOME TESTS FAILED\n" : "All tests passed\n");
    return g_failed ? 1 : 0;
}
