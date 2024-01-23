import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import numpy as np
import unittest
from erl_common.storage import GridMapInfo2D


class TestGridMapInfo(unittest.TestCase):
    def test_ray_casting_2d(self):
        grid_map_info = GridMapInfo2D(map_shape=np.array([11, 11]), min=np.array([0.0, 0.0]), max=np.array([1.0, 1.0]))
        print(grid_map_info.resolution)
        start = np.array([0.1, 0.2])
        end = np.array([0.9, 0.7])
        points = grid_map_info.ray_casting(start, end)
        print(points)
        points = grid_map_info.grid_to_meter_for_points(points)
        print(points)
        res = grid_map_info.resolution_at(0)

        for p in np.linspace(0.0, 1.0, 10):
            plt.vlines(p, 0.0, 1.0, colors="gray", linestyles="dashed")
            plt.hlines(p, 0.0, 1.0, colors="gray", linestyles="dashed")
        plt.plot([start[0], end[0]], [start[1], end[1]], "b")
        for p in points.T:
            rect = Rectangle((p[0] - res / 2, p[1] - res / 2), res, res, fill=True, facecolor="k", edgecolor="g")
            plt.gca().add_patch(rect)
        plt.scatter(points[0], points[1], c="r", s=10)
        plt.axis("equal")
        plt.show()


if __name__ == "__main__":
    unittest.main()
