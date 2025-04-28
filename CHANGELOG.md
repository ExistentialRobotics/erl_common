# 2025-04-28

- Add: add ColorBar, Legend, Shades, etc. to PlplotFig
- Add: serialization.hpp
- Update test of PlplotFig

# 2025-04-23

- Add: use plplot to draw graphs. Enabled by default. Use `-DERL_USE_PLPLOT=OFF` to disable it.
- Add: FactoryPattern
- Add: DataBufferManager
- Add: float precision selection
- Improve MKL configuration
- Improve support to Eigen < 3.4
- Fix python binding

# 2024-11-14

- Add: a function to cast `Eigen::Matrix<Eigen::Vector<T, M>>` to Numpy array of shape `(W, H, M)`.

# 2024-08-15

- Update script for installing dependencies in Ubuntu 22.04.
- `erl::common::BlockTimer` now can be set silent with `verbose=false`.
- Improve Pangolin helper classes.

# 2024-07-25

- Add support to change logging level globally
- Fix python binding error in Ubuntu 20.04
- Use `-Wl,--disable-new-dtags` to avoid RPATH issues in Ubuntu distributions, which do not support `RUNPATH` in ELF
  files very well

# 2024-07-24

- Add erl::common::PangolinPlotterImage

# 2024-07-22

- Add Tracy
- Update setup.py to set `ERL_USE_TRACY=OFF` by default
- Improve Eigen IO
- Fix bug: a progress bar is still printed when it is closed
