import numpy.typing as npt
import vedo
from typing import Union


def scatter2d(
    x: npt.NDArray, y: npt.NDArray, s: float, marker: str, c: Union[str, tuple, list, npt.NDArray]
) -> vedo.Glyph:
    """
    Args:
        x: x coordinates
        y: y coordinates
        s: marker size
        marker: options are ['.','o','O', '0', 'p','*','h','D','d','v','^','>','<','s', 'x', 'a']
        c: color name to paint all points or (3, N) array of colors for each point

    Returns:
        vedo.Glyph of the scatter
    """
    return vedo.Glyph([x, y], vedo.Marker(marker, s=s), c=c).z(1)
