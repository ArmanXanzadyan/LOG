# Log Analyzer (C++17)

Կուրսային աշխատանք «Ծրագրավորման տեխնոլոգիաներ» — թեմա №28՝ log ֆայլերի վերլուծիչ.

Ծրագիրը կարդում է log ֆայլ, ֆիլտրում է ըստ մակարդակի / regex-ի / ժամանակի, հաշվում վիճակագրություն
և արդյունքը տալիս text, CSV կամ HTML տեսքով։

Մանրամասն նկարագրությունը և կիրառված design pattern-ները՝ [docs/index.html](docs/index.html).

## Build

```
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Linux / MinGW:

```
cmake -S . -B build && cmake --build build
```

## Run

```
build/Release/log_tests.exe
build/Release/log_analyzer.exe samples/sample.log --level ERROR,WARN --from "2026-11-17 10:00:00"
build/Release/log_analyzer.exe samples/sample.log --grep "connection" --report html --out report.html
```

| Option | Meaning |
|---|---|
| `--format default\|bracket` | log line format |
| `--level ERROR,WARNING` | keep only these levels |
| `--grep <regex>` | keep messages matching regex |
| `--exclude <regex>` | drop messages matching regex |
| `--from`, `--to` | time range `"YYYY-MM-DD HH:MM:SS"` (inclusive) |
| `--report text\|csv\|html` | output type |
| `--out <file>` | write to file |

## Design patterns

Strategy, Composite, Decorator, Factory, Observer, Template Method, Facade — տես `docs/index.html`.
