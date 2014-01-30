# Stimfit

Issue tracker and Wiki are still on [Google Code](https://code.google.com/p/stimfit).

## Introduction

Stimfit is a free, fast and simple program for viewing and analyzing electrophysiological data. It's currently available for GNU/Linux, Mac OS X and Windows. It features an embedded Python shell that allows you to extend the program functionality by using numerical libraries such as [NumPy](http://numpy.scipy.org) and [SciPy](http://www.scipy.org). A standalone Python module for file i/o that doesn't depend on the graphical user interface is also available.


## Source code structure
| Directory       | Description |
| --------------- | ----------- |
|./src/libstfio   | File i/o library for common electrophysiology formats |
|./src/pystfio    | Python wrapper around libstfio |
|./src/stimfit    | Stimfit program |
|./src/stimfit/py | stf module that gets imported into the embedded Python shell |

libstfio is a private library that won't be installed system-wide. You may therefore end up with two copies of libstfio.so: One in the private stimfit library directory (/usr/lib/stimfit/ or similar), the other one in the Python site packages path for pystfio. libstfio may turn into a system-wide library in the future.
