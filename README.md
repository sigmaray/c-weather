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
  libayatana-appindicator3-1 libcairo2-dev
```

Заголовок AppIndicator встроен в `include/` (можно собирать без `-dev` пакета, если есть `.so`).
cJSON вендорится в `third_party/`.

## Сборка и запуск

```bash
make
./c-weather
```

При первом запуске создаётся `settings.json` в текущей директории.

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
├── include/libayatana-appindicator/app-indicator.h
├── third_party/cJSON.{c,h}
└── src/
    ├── main.c        # трей, таймер, цикл GTK
    ├── settings.c    # settings.json
    ├── http.c        # libcurl
    ├── weather.c     # геокодинг и погода
    ├── history.c     # кольцевые буферы ошибок/запросов
    ├── icon.c        # PNG-иконки (Cairo)
    └── ui.c          # GTK-диалоги
```

## Лицензия

MIT
