import ctypes
import matplotlib.pyplot as plt
import numpy as np



lib = ctypes.CDLL('./positionControlTB.so')

# Définir les prototypes des fonctions
lib.get_values.restype = ctypes.c_int
lib.get_values.argtypes = [ctypes.POINTER(ctypes.POINTER(ctypes.c_double)), ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]
lib.free_values.argtypes = [ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_int)]

# Fonction pour appeler get_values
def get_values_from_cpp():
    data_ptr = ctypes.POINTER(ctypes.c_double)()
    time_ptr = ctypes.POINTER(ctypes.c_int)()

    size = lib.get_values(ctypes.byref(data_ptr), ctypes.byref(time_ptr))

    if size <= 0:
        raise ValueError("get_values a retourné une taille invalide ou nulle")

    data_array = ctypes.cast(data_ptr, ctypes.POINTER(ctypes.c_double * size)).contents
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
    accelerations = []
    accelerations.append(0)
    speeds.append(0)
    lastSpeed=0
    for i in range(1, len(data)):
        distance_diff = data[i] - data[i - 1]  # Différence de distance (en mm)
        time_diff = (time[i] - time[i - 1])/1000      # Différence de temps (en ms)

        if time_diff == 0:
            speed = 0  # Éviter la division par zéro
        else:
            speed = distance_diff / time_diff  # Vitesse en mm/ms
            speed = max(-1000, min(1000, speed))

        if(i>=2):
            if time_diff == 0:
                acceleration = 0
            else:
                acceleration = (speed - lastSpeed) / time_diff  # Vitesse en mm/ms
                acceleration = max(-1000, min(1000, acceleration))
        else:
            acceleration = 0
        lastSpeed = speed

        accelerations.append(acceleration)
        speeds.append(speed)

    return speeds, accelerations

if __name__ == "__main__":
    data, time = get_values_from_cpp()
    print(data)
    speed, acceleration = calculate_speed(data,time)
    print(speed)
    fig, ax = plt.subplots()
    ax.plot(time, data,marker = 'o')

    ax.set(xlabel='time (ms)')
    ax.set(ylabel='distance (mm)')
    ax.set(title='position control')
    ax.grid()


    ax2 = ax.twinx()
    ax2.set(ylabel='speed')
    ax2.plot(time, speed, marker = 'o', color="orange")
    ax2.plot(time, acceleration, marker = 'o', color="red")



    plt.show()

