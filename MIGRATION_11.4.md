# Миграция g4tube на Geant4 11.4.2 (latest stable) + Qt6

## Цель
Перевести xtube и CoreCT с Geant4 10.7.3 на **11.4.2** (последняя stable; 11.5.0.beta — пре-релиз, не берём). Qt6 — режим по умолчанию в 11.4 (`GEANT4_USE_QT`).

## Принятые решения
1. Устанавливать Geant4 11.4.2 в `/home/user/geant4/11.4.2` (пользовательский prefix, без sudo).
2. AnodeMessenger (WIP в рабочем дереве, ломает сборку) — **исключить из сборки**, файлы `include/AnodeMessenger.hh` и `src/AnodeMessenger.cpp` не трогать.
3. Минимальная миграция: только то, что нужно чтобы собиралось на 11.4.2 + Qt6. Без G4RunManagerFactory, G4Random, чистки мёртвых инклудов.

## Проверенные факты
- На машине установлен только Geant4 10.7.3 (`/usr/local/lib/Geant4-10.7.3`).
- Коммит `ffda2e9` уже мигрировал код на 11.4.1+Qt6 (правки `include/Analysis.hh`, `src/RunAction.cpp`, `src/Detector.cpp`), но CMake в рабочем дереве откачен на Qt5/10.7.3 (uncommitted). Сборки 11.x на этой машине нет.
- Env: Ubuntu 22.04, gcc 11.4.0, cmake 3.22, Qt 6.2.4 (dev-пакеты есть), gengetopt установлен, 16 ядер, ~228G свободно, интернет доступен, `/usr/local` непишем (sudo требует пароль).
- Аудит кода: CoreCT/xtube совместимы с 11.4 — блокирующих API нет. Имена виз-сцен `OGLSQt`/`OGLSX`/`OGL` существуют в 11.4.2.
- Tarball исходников доступен: `https://cern.ch/geant4-data/releases/geant4.11.4.2.tar.gz` (302 на CDN).
- В CoreCT есть `#if 0` блок с битой строкой (`corect.cc:608`) — не компилируется, не трогаем.

## Шаги

### 1. Сборка и установка Geant4 11.4.2 с Qt6
```
mkdir -p /home/user/geant4/src
# скачать geant4.11.4.2.tar.gz и распаковать в /home/user/geant4/src/geant4.11.4.2

cmake -S geant4.11.4.2 -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/home/user/geant4/11.4.2 \
  -DGEANT4_INSTALL_DATA=ON \          # авто-скачивание данных (~3 ГБ)
  -DGEANT4_USE_QT=ON \                # Qt6 по умолчанию
  -DGEANT4_BUILD_MULTITHREADED=ON \
  -DGEANT4_USE_OPENGL_X11=ON \
  -DGEANT4_USE_RAYTRACER_X11=ON \
  -DGEANT4_USE_SYSTEM_EXPAT=ON

cmake --build build -j16
cmake --install build
```
Проверка: `/home/user/geant4/11.4.2/bin/geant4-config --version` == 11.4.2; в `build/CMakeCache.txt` Qt6 сконфигурирован.

### 2. xtube — корневой `CMakeLists.txt`
- `find_package(Geant4 ...)` HINTS → `/home/user/geant4/11.4.2/lib/cmake/Geant4`.
- `find_package(Qt5 COMPONENTS Core REQUIRED)` → `find_package(Qt6 COMPONENTS Core REQUIRED)`.
- `target_link_libraries(... Qt5::Core)` → `Qt6::Core`.
- Исключить WIP из глобов:
  ```
  file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
  list(REMOVE_ITEM sources ${PROJECT_SOURCE_DIR}/src/AnodeMessenger.cpp)
  # аналогично для headers (или оставить .hh — безвредно)
  ```
- Код уже совместим с 11.x (коммит `ffda2e9`); ошибки, если всплывут, чинить по ходу.

### 3. CoreCT — `CoreCT/CMakeLists.txt`
- `cmake_minimum_required(VERSION 2.6)` → `cmake_minimum_required(VERSION 3.16)`.
- HINTS → `/home/user/geant4/11.4.2/lib/cmake/Geant4`.
- Добавить `set(CMAKE_CXX_STANDARD 17)` + `CMAKE_CXX_STANDARD_REQUIRED ON`.
- `include(${Geant4_USE_FILE})` / `${Geant4_LIBRARIES}` оставить (deprecated, но работает); при проблемах — переход на `Geant4::ui_all Geant4::vis_all`.
- Код: правок не требуется (аудит чист).

### 4. Сборка и дымовые проверки
- Пересобрать xtube (root/build) и corect (CoreCT/build), предварительно очистив старые кэши (т.к. они указывают на 10.7.3).
- xtube: batch-прогон по временной macro с `/run/beamOn 20` (без `/vis`) — проверить запись CSV.
- corect: batch `-t CorePhantom -m <маленькая_макро>` (не runct.mac — там 12.7M событий).
- Qt GUI / `/vis/open OGLSQt` вручную на headless-машине не проверить — только конфигурации.

### 5. Документация
- `AGENTS.md`: обновить хардкод-пути Geant4 (10.7.3 → `/home/user/geant4/11.4.2`), пометить, что 11.4 использует Qt6 по умолчанию.

## Вне скоупа
- AnodeMessenger (`include/AnodeMessenger.hh`, `src/AnodeMessenger.cpp`) — исключён из сборки, файлы сохраняются.
- Дальнейшая модернизация (G4RunManagerFactory, `G4Random::setTheEngine`, чистка мёртвых инклудов) — отдельно при желании.

## Точка продолжения
Начинать с шага 1: загрузка исходников geant4.11.4.2 в `/home/user/geant4/src`.