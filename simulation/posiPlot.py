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

def calculate_speed(data, time):
    # Vérifier que data et time ont la même taille
    if len(data) != len(time):
        raise ValueError("Les listes data et time doivent avoir la même taille")

    # Calculer la vitesse entre chaque point
    speeds = []
    speeds.append(0)
    for i in range(1, len(data)):
        distance_diff = data[i] - data[i - 1]  # Différence de distance (en mm)
        time_diff = (time[i] - time[i - 1])/1000      # Différence de temps (en ms)

        if time_diff == 0:
            speed = 0  # Éviter la division par zéro
        else:
            speed = distance_diff / time_diff  # Vitesse en mm/ms

        speeds.append(speed)

    return speeds

if __name__ == "__main__":
    data, time = get_values_from_cpp()
    print(data)
    speed = calculate_speed(data,time)
    fig, ax = plt.subplots()
    ax.plot(time, data)

    ax.set(xlabel='time (ms)')
    ax.set(ylabel='distance (mm)')
    ax.set(title='poistion control')
    ax.grid()


    ax2 = ax.twinx()
    ax2.set(ylabel='speed')
    ax.plot(time, speed)



    plt.show()
