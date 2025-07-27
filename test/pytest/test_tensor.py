import numpy as np
from erl_common.storage import TensorDouble2D
import unittest


class TestTensor(unittest.TestCase):

    def test_TensorDouble2D(self):
        shape = np.array([3, 4])
        value = 10
        tensor = TensorDouble2D(shape, value)
        self.assertEqual(tensor.dims, 2)  # check dims
        self.assertTrue(np.array_equal(tensor.shape, shape))  # check shape
        self.assertEqual(tensor.size, shape.prod())  # check size
        self.assertTrue(tensor.is_row_major)

        tensor.fill(1)
        array = np.array(tensor, copy=False)  # array and tensor does share the same memory with copy=False
        self.assertTrue(np.array_equal(array, np.ones(shape)))  # check fill and cast-to-numpy
        array[0, 0] = 10
        self.assertEqual(tensor[0], 10)
        tensor[0, 1] = 20
        self.assertEqual(array[0, 1], 20)
        tensor[3] = 5  # support linear indexing
        self.assertEqual(array[0, 3], 5)

        tensor_slice = tensor.get_slice([1], [2])
        self.assertTrue(np.array_equal(tensor_slice, array[:, 2]))


if __name__ == '__main__':
    unittest.main()
