# scivis-qt

Interactive scientific data visualization using C++, Qt6 and LibTorch.

## Overview

Personal project started to explore the intersection of C++ UI development 
and ML inference in a scientific visualization context. It is a first working prototype, 
deliberately kept simple, with more features planned.

A simple binary classifier is trained in Python on synthetic 2D data and exported 
as TorchScript. The C++ application loads the model via LibTorch and visualizes:
- The dataset (two classes, scatter plot)
- The decision regions (precomputed inference grid)
- Real-time inference at the mouse cursor position

## Stack

- **C++17** — core application
- **Qt6 Widgets + QPainter** — UI and custom rendering
- **LibTorch** — PyTorch C++ API for model inference
- **Python + PyTorch** — model training and TorchScript export

## Project structure
scivis-qt/
├── src/                  # C++ source
│   ├── main.cpp
│   ├── ScatterWidget.h
│   └── ScatterWidget.cpp
├── ml/                   # Python scripts
│   └── train_and_export.py
├── models/               # TorchScript model (not tracked by git)
├── data/                 # Generated dataset (not tracked by git)
└── CMakeLists.txt

## Build

### Prerequisites
- Qt6 (`qt6-base-dev`)
- LibTorch (CPU) — download from pytorch.org, extract locally
- CMake >= 3.16, GCC with C++17
- Python 3.9+ with PyTorch (for model training)

### Steps
```bash
# Set LibTorch path in CMakeLists.txt
# set(CMAKE_PREFIX_PATH "/path/to/libtorch")

mkdir build && cd build
cmake .. 
make
./scivisQt
```

### Generate model and data
```bash
cd models
python3 ../ml/train_and_export.py
```

## Roadmap
- Precise decision boundary (binary search + path propagation)
- Dynamic grid resolution
- QML version
- Multi-class support
- `setRange()` for configurable axis bounds
- `classColor()` for extensible class-to-color mapping

## License

MIT License — see [LICENSE](LICENSE) for details.