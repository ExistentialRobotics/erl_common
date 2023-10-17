from .pyerl_common.storage import *
import numpy as np

__all__ = [
    "compute_c_strides",
    "compute_f_strides",
    "GridMapInfoXd",
    "GridMapInfo2D",
    "GridMapInfo3D",
    "TensorDouble2D",
    "TensorDouble3D",
    "TensorDoubleXd",
    "TensorInt2D",
    "TensorInt3D",
    "TensorIntXd",
    "TensorUnsigned2D",
    "TensorUnsigned3D",
    "TensorUnsignedXd",
    "GridMapDouble2D",
    "GridMapInt2D",
    "GridMapUnsigned2D",
    "GridMapDouble3D",
    "GridMapInt3D",
    "GridMapUnsigned3D",
    "GridMapDoubleXd",
    "GridMapIntXd",
    "GridMapUnsignedXd",
    "GridMapDrawer2D",
]


def save_numpy_as_eigen_binary(arr: np.ndarray, filename: str):
    if arr.ndim == 1:
        arr = arr.reshape(-1, 1)
    assert arr.ndim == 2, "Only support 1D or 2D array"
    with open(filename, "wb") as f:
        f.write(np.array(arr.shape).astype(np.int64).tobytes())
        f.write(arr.tobytes("F"))
