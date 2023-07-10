import numpy as np
import numpy.typing as npt
import vedo


def lidar_rays(
    pos: npt.NDArray[np.float64],
    directions: npt.NDArray[np.float64],
    ranges: npt.NDArray[np.float64],
    color: str = "k",
    alpha: float = 1.0,
    lw: int = 1,
) -> vedo.Lines:
    start_pts = np.repeat(pos.reshape(2, 1), ranges.shape[0], axis=1)  # (2, N)
    end_pts = start_pts + directions * ranges  # (2, N)
    return vedo.Lines(
        start_pts.T,  # (N, 2)
        end_pts.T,  # (N, 2)
        c=color,
        alpha=alpha,
        lw=lw,
    )


def curve(
    x: npt.NDArray,
    y: npt.NDArray,
    dotted: bool = False,
    res: int = 1,
    scale: float = 1,
    lw: int = 1,
    c: str = "k4",
    alpha: float = 1.0,
) -> vedo.Lines:
    pts = np.hstack([x.reshape(-1, 1), y.reshape(-1, 1)])
    return vedo.Lines(pts[:-1], pts[1:], dotted, res, scale, lw, c, alpha)
