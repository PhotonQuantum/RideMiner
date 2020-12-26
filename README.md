# RideMiner

Analysis and Visualization of Online Ride-Hailing Order Data in Chengdu.

Final project for Principles and Practice of Problem Solving (SJTU-CS241-2020).

## Get Started

### Requirements

- CMake 3.17 or higher
- C++ Compilers with C++17 support (gcc 10.2.0 & clang 11.0.0 on Archlinux tested)
- Qt 5.0

### Build

```shell
$ mkdir build && pushd build
$ cmake ..
$ make
$ popd
```

### Run

```shell
$ ./build/RideMiner
```

You need to have `Dataset-CS241-2020` prepared to run `RideMiner`.

## Third Parties

- [Qt](https://www.qt.io/) - A free and open-source widget toolkit for creating graphical user interfaces
- [AsyncFuture](https://github.com/benlau/asyncfuture) - Use QFuture like a Promise object
- [QtWaitingSpinner](https://github.com/snowwlex/QtWaitingSpinner) - A highly configurable, custom Qt widget for showing "waiting" or "loading" spinner icons
- [Qt-RangeSlider](https://github.com/ThisIsClark/Qt-RangeSlider) - Range slider in Qt
- [MeanShift](https://blog.csdn.net/SongJ12345666/article/details/103512309) - MeanShift implemented in C++

## License

This project is licensed under GNU General Public License v3.0 - see the [LICENSE.md](LICENSE.md) file for details.

<p align="center">&mdash;💖&mdash;</p>
<p align="center"><i>Built with love by LightQuantum</i></p>
