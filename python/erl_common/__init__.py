import numpy as np

from . import dict_ext, vedo_ext
from .pyerl_common import Logging, set_global_random_seed

__all__ = [
    "dict_ext",
    "Logging",
    "save_numpy_as_eigen_binary",
    "set_global_random_seed",
    "vedo_ext",
]


def save_numpy_as_eigen_binary(arr: np.ndarray, filename: str):
    if arr.ndim == 1:
        arr = arr.reshape(-1, 1)
    assert arr.ndim == 2, "Only support 1D or 2D array"
    with open(filename, "wb") as f:
        f.write(np.array(arr.size).astype(np.int64).tobytes())
        f.write(np.array(arr.shape).astype(np.int64).tobytes())
        f.write(arr.tobytes("F"))
