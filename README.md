# DynMatch: Dynamic Matching in Practice

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++11](https://img.shields.io/badge/C++-11-blue.svg)](https://en.cppreference.com/w/cpp/11)
[![CMake](https://img.shields.io/badge/build-CMake-blue)](https://cmake.org/)
[![Linux](https://img.shields.io/badge/platform-Linux-blue)](https://github.com/DynGraphLab/DynMatch)
[![macOS](https://img.shields.io/badge/platform-macOS-blue)](https://github.com/DynGraphLab/DynMatch)
[![Homebrew](https://img.shields.io/badge/homebrew-available-orange)](https://github.com/DynGraphLab/homebrew-dyngraphlab)
[![GitHub Stars](https://img.shields.io/github/stars/DynGraphLab/DynMatch)](https://github.com/DynGraphLab/DynMatch/stargazers)
[![GitHub Issues](https://img.shields.io/github/issues/DynGraphLab/DynMatch)](https://github.com/DynGraphLab/DynMatch/issues)
[![GitHub Last Commit](https://img.shields.io/github/last-commit/DynGraphLab/DynMatch)](https://github.com/DynGraphLab/DynMatch/commits)
[![arXiv](https://img.shields.io/badge/arXiv-2004.09099-b31b1b)](https://arxiv.org/abs/2004.09099)
[![Heidelberg University](https://img.shields.io/badge/Heidelberg-University-c1002a)](https://www.uni-heidelberg.de)

<p align="center">
  <img src="./img/dynmatch-banner.png" alt="DynMatch Banner" width="600"/>
</p>

Part of the [DynGraphLab &mdash; Dynamic Graph Algorithms](https://github.com/DynGraphLab) open source framework. Developed at the [Algorithm Engineering Group, Heidelberg University](https://ae.ifi.uni-heidelberg.de).

## Description

In recent years, significant advances have been made in the design and analysis of fully dynamic maximal matching algorithms. However, these theoretical results have received very little attention from the practical perspective. Few of the algorithms are implemented and tested on real datasets, and their practical potential is far from understood. DynMatch bridges the gap between theory and practice for the fully dynamic maximal matching problem. We engineer several algorithms and empirically study them on an extensive set of dynamic instances.

An [overview talk](https://drive.google.com/file/d/1HMCTWshidkOCyDLB923CXELYYSBUddq8/view) and [slides](https://drive.google.com/file/d/1rtv6jwSe4DD88sevEHLkG3ESVtcr9_ak/view?usp=sharing) explaining the algorithms are available.

## Install via Homebrew

```console
brew install DynGraphLab/dyngraphlab/dynmatch
```

Then run:
```console
dynmatch FILE --algorithm=dynblossom --dynblossom_maintain_opt
```

## Installation (from source)

```console
git clone https://github.com/DynGraphLab/DynMatch
cd DynMatch
./compile_withcmake.sh
```

All binaries are placed in `./deploy/`. Alternatively, use the standard CMake process:

```console
mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make && cd ..
```

## Usage

```console
dynmatch FILE [options]
```

### Options

| Option | Description |
|:-------|:-----------|
| `FILE` | Path to dynamic graph sequence file |
| `--algorithm=TYPE` | One of {staticblossom, dynblossom, randomwalk, neimansolomon, baswanaguptasen} |
| `-seed=<int>` | Seed for the random number generator |
| `-eps=<double>` | Epsilon: limits search depth of random walk or augmenting path search to 2/eps-1 |
| `--dynblossom_lazy` | Only start augmenting path searches after x newly inserted edges on an endpoint |
| `--dynblossom_maintain_opt` | Maintain optimum in dynblossom (without this the algorithm is called UNSAFE) |
| `-measure_graph_only` | Only measure graph construction time |
| `-help` | Print help |

## Input Format

Dynamic graph sequence format. The first line starts with `#` followed by the number of nodes and updates. Each subsequent line specifies an operation: `1 u v` for edge insertion, `0 u v` for edge deletion.

```
# 30399 87627
1 1 2
1 51 52
0 1 2
```

## License

The program is licensed under the [MIT License](https://opensource.org/licenses/MIT).
If you publish results using our algorithms, please acknowledge our work by citing the following paper:

```bibtex
@inproceedings{DBLP:conf/esa/Henzinger0P020,
  author       = {Monika Henzinger and
                  Shahbaz Khan and
                  Richard D. Paul and
                  Christian Schulz},
  title        = {Dynamic Matching Algorithms in Practice},
  booktitle    = {28th Annual European Symposium on Algorithms, {ESA} 2020},
  series       = {LIPIcs},
  volume       = {173},
  pages        = {58:1--58:20},
  publisher    = {Schloss Dagstuhl - Leibniz-Zentrum f{\"{u}}r Informatik},
  year         = {2020},
  doi          = {10.4230/LIPICS.ESA.2020.58}
}
```
