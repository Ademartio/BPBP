from module import bpbp
import numpy as np

def generate_random_matrix():
    N = np.random.randint(1, 100)  # Random number of rows between 1 and 10
    M = np.random.randint(1, 100)  # Random number of columns between 1 and 10

    # Generate a random NxM matrix with 0s and 1s
    matrix = np.random.randint(2, size=(N, M), dtype=np.uint8)
    return matrix

# Example usage:
mat = generate_random_matrix()

n, m = np.shape(mat)
print("Matrix shape in python is: {}x{}".format(n, m))

bpbp.koh_v2(mat)