import numpy as np
import numpy.typing as npt
from vedo import Grid

# vedo.Picture changes to vedo.Image
try:
    from vedo import Picture
except ImportError:
    from vedo import Image as Picture


def image_mesh(pic: Picture, x_coords: np.ndarray, y_coords: np.ndarray) -> Grid:
    """a modified version of vedo.Picture.tomesh that implements custom extent in matplotlib

    Args:
        pic: vedo.Picture
        x_coords: x coordinates along image width-axis from 0 to width
        y_coords: y coordinates along image height-axis from 0 to height

    Returns:

    """
    grid = Grid(s=[x_coords, y_coords])
    grid.pickable(True).wireframe(False).lw(0)
    pic.inputdata().GetPointData().GetScalars().SetName("RGBA")
    grid.inputdata().GetPointData().AddArray(pic.inputdata().GetPointData().GetScalars())
    grid.inputdata().GetPointData().SetActiveScalars("RGBA")
    grid.mapper().SetArrayName("RGBA")
    grid.mapper().ScalarVisibilityOn()
    grid.name = pic.name
    grid.filename = pic.filename
    return grid


def as_vedo_image_coords(np_image_coords: npt.NDArray, image_height: int, inplace: bool = False) -> npt.NDArray:
    """

    Args:
        np_image_coords: (2, N) array of coordinates
        image_height:
        inplace:

    Returns:
        (2, N) array of coordinates that set the bottom left (0, 0), x to right, y upwards
    """
    # vedo never flips its underlying coordinates, just makes the coordinates look like flipped
    if inplace:
        coords = np_image_coords
    else:
        coords = np_image_coords.copy()

    coords[1] = image_height - coords[1]
    return coords
