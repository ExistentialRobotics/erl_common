import numpy as np

import vedo
import numpy.typing as npt


def polygon_mesh_2d(vertices: npt.NDArray[np.float64]):
    return vedo.Mesh(
        [
            np.insert(vertices.T, 2, 0, axis=1).tolist(),  # (N, 3)
            [tuple(range(vertices.shape[1]))],
        ]
    )
