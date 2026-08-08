# C Weather App

Порт tray-приложения из `ts-weather` на C (GTK 3 + Ayatana AppIndicator).

Показывает текущую температуру в системном трее и обновляет её с заданным интервалом.

## Возможности

- Температура и погодная иконка в трее
- Автообновление по `updateIntervalInSeconds`
- Местоположение: город+страна или широта+долгота
- Провайдеры: `open-meteo` и `openweathermap`
- Диалоги: подробности, настройки, справка, история API-запросов и ошибок

## Зависимости (Ubuntu/Debian)

```bash
sudo apt-get install build-essential pkg-config libgtk-3-dev libcurl4-openssl-dev \
  libayatana-appindicator3-dev libcairo2-dev
```

Опционально для линтера: `cppcheck`.

Заголовок AppIndicator также встроен в `include/` (можно собирать без `-dev`, если есть `.so` — Makefile подхватит его через fallback).
cJSON вендорится в `third_party/`.

## Сборка и запуск

```bash
make
./c-weather
```

Линтер:

```bash
make lint   # требует cppcheck
```

При первом запуске создаётся `settings.json` в текущей директории.

Трей (Ayatana AppIndicator) работает на **Linux**. Сборка на macOS и Windows поддерживается через stub AppIndicator (бинарь линкуется, иконка в трее не появляется).

## CI

GitHub Actions:

- [`.github/workflows/ci.yml`](.github/workflows/ci.yml) — `cppcheck`, сборка с `-Werror` (Ubuntu gcc/clang, macOS, Windows)
- [`.github/workflows/release.yml`](.github/workflows/release.yml) — после каждого пуша в `main` создаётся GitHub Release с бинарниками Linux / macOS / Windows

Бинарники динамически линкуются (нужны системные GTK/curl; на Linux ещё AppIndicator).

## Настройки

Файл `settings.json` (camelCase, как в `ts-weather` / `go-weather`):

```json
{
  "city": "New York City",
  "country": "United States",
  "latitude": null,
  "longitude": null,
  "updateIntervalInSeconds": 60,
  "apiProvider": "open-meteo",
  "apiKey": null
}
```

Либо координаты вместо города/страны. Для OpenWeatherMap укажите `apiProvider` и `apiKey`.

## Взаимодействие

Окно при старте не открывается. Правый клик по иконке температуры в трее:

- Обновить сейчас
- Подробная информация о погоде
- Настройки
- Как пользоваться
- История запросов / ошибок API
- Выйти

## Структура

```
c-weather/
├── Makefile
├── README.md
├── .github/workflows/ci.yml
├── .github/workflows/release.yml
├── include/libayatana-appindicator/app-indicator.h
├── third_party/cJSON.{c,h}
└── src/
    ├── main.c              # трей, таймер, цикл GTK
    ├── settings.c          # settings.json
    ├── http.c              # libcurl
    ├── weather.c           # геокодинг и погода
    ├── history.c           # кольцевые буферы ошибок/запросов
    ├── icon.c              # PNG-иконки (Cairo)
    ├── ui.c                # GTK-диалоги
    ├── compat.h            # POSIX-совместимость (Windows)
    └── appindicator_stub.c # stub AppIndicator для macOS/Windows
```

## Лицензия

MIT
