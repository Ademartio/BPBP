import numpy as np

from module import bpbp

def kruskal_on_hypergraph(Hog):
    """
    We iterate over all the columns of the matrix of ordered llr columns and keep the linearly independent ones.
    We do so by choosing ones which do not produce loops.

    Args:
        Hog (np.array): Ordered pcm.

    Returns:
        matrix_with_empty_column (np.array): Matrix of linear independent columns plus an empty column.
        column_to_square (np.array): Vector containing the positions of the columns that have been chosen as l.i.
    """
     
    rows, columns = Hog.shape
    column_to_square = np.zeros(rows, dtype = int)
    
    zeros_rows = np.zeros((2, columns))
    H =  np.vstack((Hog, zeros_rows))
    
    # We add virtual checks so all columns have at least two non-trivial elements.
    for i in range(columns):
        ones_in_col = np.where(H[:,i] == 1)[0]
        if len(ones_in_col) == 1:
            if ones_in_col[0] < rows//2:
                H[-2,i] = 1
            else:
                H[-1,i] = 1
           
    # cluster_array indicates in which tree each node is. Tree 0 means they are in no  t ree.     
    cluster_array = np.zeros(H.shape[0], dtype = int)
    cluster_number = 1
    column_number = 1
    
    # First column is always l.i.
    cluster_array[np.where(H[:,0] == 1)[0]] = cluster_number
    cluster_number += 1
    
    for i in range(1,columns):
        
        cluster_values = cluster_array[np.where(H[:,i] == 1)[0]]
        non_zero_cluster_values = cluster_values[np.nonzero(cluster_values)]
        if np.any(np.bincount(non_zero_cluster_values) > 1):
            # Loop, omit column.
            continue

        if len(non_zero_cluster_values) == 0:
            # New tree
            cluster_array[(np.where(H[:,i] == 1)[0])] = cluster_number
            cluster_number += 1
        
        elif len(non_zero_cluster_values) == 1:
            # Absortion of the node into a former tree
            cluster_array[(np.where(H[:,i] == 1)[0])] = non_zero_cluster_values[0]

        else:
            # UNION of several trees through the new node
            cluster_array[(np.where(H[:,i] == 1)[0])] = cluster_number
            cluster_array[np.where(np.isin(cluster_array, non_zero_cluster_values))] = cluster_number
            cluster_number += 1

        column_to_square[column_number] = i
        column_number += 1
        if column_number == rows:
            break
    

    assert column_to_square[-1] != 0, " Ha habido un error, no ha encontrado n-k columnas independientes"
    
    H_square = H[:,column_to_square.astype(int)]

    print("H_square is:")
    print(H_square.astype(int))
    
    empty_column = np.zeros((H_square.shape[0], 1))  # Create an empty column filled with zeros
    matrix_with_empty_column = np.hstack((H_square, empty_column))

    return matrix_with_empty_column[:-2,:], column_to_square

# Define a matrix
matrix = [  [False,  True, False, False, False,  True, False, False,  True, False, False],
            [False, False, False, False, False, False, False, False, False, False, True],
            [ True,  True, False, False, False, False, False, False, False, False, False],
            [False, False, False,  True, False, False, False, False,  True, False, False],
            [False, False, False, False, False, False, False, False, False, False, True],
            [False, False, False, False, False, False, False, False, False, False, False],
            [False, False, False, False, False, False, False,  True, False, False, False],
            [False, False, False, False, False, False, False, False, False, False, True],
            [False, False,  True, False, False,  True, False, False, False, False, False],
            [False, False, False, False, False, False, False, False, False, False, False]
        ]

np_mat = np.array(matrix)

#print("Before bpbp:")
#bpbp.print_matrix(matrix)

H, col = bpbp.kruskal_on_hypergraph(np_mat)

print("\nAfter bpbp:")
bpbp.print_matrix(H)
print("\nThe column of the tupple:")
#col = [col] # Make a list of only one list to be able to print it hehe
print(col)


H_orig, col_orig = kruskal_on_hypergraph(np_mat)

print("\nThe original values are:\n Matrix:\n")
print(H_orig.astype(int))
print(" Column:")
print(col_orig)

assert np.allclose(H, H_orig.astype(bool)) == True, "Error! Las matrices resusltantes no son iguales..."

print("Las matrices obtenidas son iguales! Zorionak!!")

assert np.allclose(col, col_orig) == True, "Error! Las columnas obtenidas no son iguales..."

print("Las columnas obtenidas también son iguales! Zorionak^2!!")