# scivis-qt

Interactive scientific data visualization using C++, Qt6 and LibTorch.

## Overview

Personal project started to explore the intersection of C++ UI development 
and ML inference in a scientific visualization context. It is a  working prototype in active development.

A simple binary classifier is trained in Python on synthetic 2D data and exported 
as TorchScript. The C++ application loads the model via LibTorch and visualizes:
- The dataset (two classes or more, scatter plot)
- The decision regions (precomputed inference grid)
- Real-time inference at the mouse cursor position

Two frontends are provided, both backed by the same C++ data layer (`ScatterData`):
- **Qt Widgets** — custom rendering via `QPainter`
- **QML** — custom rendering via `QQuickPaintedItem` (`ScatterViewBase`), `ApplicationWindow` with toolbar

## Stack

- **C++17** — core application
- **Qt6 Widgets + QPainter** — Widgets frontend
- **Qt6 QML + QQuickPaintedItem** — QML frontend
- **LibTorch** — PyTorch C++ API for model inference
- **Python + PyTorch + scikit-learn** — model training and TorchScript export


## Build

### Prerequisites
- Qt6 (`qt6-base-dev`, `qt6-declarative-dev`, `qt6-base-dev-tools`)
- LibTorch (CPU) — download from pytorch.org, extract locally
- CMake >= 3.16, GCC with C++17
- Python 3.9+ with PyTorch and scikit-learn (for model training  and preset generation)

### Steps
```bash
# Set LibTorch path in CMakeLists.txt
# set(CMAKE_PREFIX_PATH "/path/to/libtorch")

mkdir build && cd build
cmake .. 
make
./scivis-qt
```

### CMake options
```bash
cmake .. -DSCIVISQT_ENABLE_WIDGETS=OFF  # QML only
cmake .. -DSCIVISQT_ENABLE_QML=OFF      # Widgets only
cmake .. -DSCIVISQT_BUILD_TESTS=ON      # build unit tests (not built by default)
# Both frontends ON by default
```


### Generate presets
```bash
cd ml
python3 generate_presets.py
```

Generates 4 preset datasets and models in `data/` and `models/`.

### Run unit tests
```bash
make tst_scatterwidget
make tst_scatterdata
./tests/tst_scatterwidget
./tests/tst_scatterdata
```

## What's in v0.1

- Qt Widgets frontend (QPainter)
- QML frontend (Canvas)
- Shared C++ backend (ScatterData)
- Decision region grid (precomputed LibTorch inference)
- Axes, ticks, grid lines and labels
- Real-time cursor inference with tooltip
- Python/PyTorch training script and TorchScript export
- Synthetic 2D dataset generation

## What's in v0.2

- QMainWindow with toolbar (Qt Widgets frontend)
- ApplicationWindow with toolbar (QML frontend)
- Load Model and Load Data via file dialog in both frontends
- 4 preset datasets: separated, overlapping, moons, three classes
- CMake options to enable/disable each frontend independently
- Precompiled headers for LibTorch

## What's in v0.3

- Unit tests (QTest) — coordinate transforms, grid size, predict robustness, data loading
- QML frontend refactored: Canvas/JavaScript replaced by QQuickPaintedItem (ScatterViewBase)
- ScatterView.qml — public QML component with title and legend as declarative elements

## Known limitations

- Window positioning may not work correctly under WSL/X11
- QML Text color rendering may be incorrect on some WSL/X11 configurations depending on Qt version and graphics driver

## Roadmap

- Additional unit tests (ScatterViewBase, numClasses)
- Precise decision boundary (binary search + path propagation)
- C++ data generation and model training (no Python at runtime)
- Configurable axis bounds (setRange())
- Window resizing support
- QRegion partial invalidation in paintEvent
- Load QML from disk in development mode
- Qt Creator integration


## License

MIT License — see [LICENSE](LICENSE) for details.