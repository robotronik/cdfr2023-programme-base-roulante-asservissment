import ctypes
import matplotlib.pyplot as plt
import numpy as np



lib = ctypes.CDLL('./positionControlTB.so')

# Définir les prototypes des fonctions
lib.get_values.restype = ctypes.c_int
lib.get_values.argtypes = [ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]
lib.free_values.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]

# Fonction pour appeler get_values
def get_values_from_cpp():
    data_ptr = ctypes.POINTER(ctypes.c_int)()
    time_ptr = ctypes.POINTER(ctypes.c_int)()

    size = lib.get_values(ctypes.byref(data_ptr), ctypes.byref(time_ptr))

    if size <= 0:
        raise ValueError("get_values a retourné une taille invalide ou nulle")

    data_array = ctypes.cast(data_ptr, ctypes.POINTER(ctypes.c_int * size)).contents
    time_array = ctypes.cast(time_ptr, ctypes.POINTER(ctypes.c_int * size)).contents

    data_list = list(data_array)
    time_list = list(time_array)

    lib.free_values(data_ptr, time_ptr)

    return data_list, time_list


if __name__ == "__main__":
    data, time = get_values_from_cpp()
    fig, ax = plt.subplots()
    ax.plot(time, data)

    ax.set(xlabel='time (ms)')
    ax.set(ylabel='distance (mm)')
    ax.set(title='poistion control')
    ax.grid()

    plt.show()

