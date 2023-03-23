import numpy as np

k = .011

def sigmoid(E):
    return 1 / (1 + np.exp(-k*E))

print(sigmoid(100))
