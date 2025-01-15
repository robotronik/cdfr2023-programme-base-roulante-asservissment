import ctypes
import matplotlib.pyplot as plt
import numpy as np

# Charger la bibliothèque partagée
lib = ctypes.CDLL('./positionControlTB.so')

# Définir les prototypes des fonctions C++
lib.get_values.restype = ctypes.c_int  # get_values retourne un pointeur sur int
lib.get_values.argtypes = [ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]  # Prend un pointeur sur int comme argument

lib.free_values.argtypes = [ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]  # free_values prend un pointeur sur int

# # Appeler la fonction `get_values`
# size = ctypes.c_int()  # Créer une variable pour stocker la taille du tableau
# values_ptr = lib.get_values(ctypes.byref(size))  # Obtenir le pointeur et la taille

data_ptr = ctypes.POINTER(ctypes.c_int)()
time_ptr = ctypes.POINTER(ctypes.c_int)()
size = lib.get_values(ctypes.byref(data_ptr), ctypes.byref(time_ptr))
print(size)

if size <= 0:
    raise ValueError("get_values a retourné une taille invalide ou nulle")


# Convertir les pointeurs en tableaux Python accessibles
data_array = ctypes.cast(data_ptr, ctypes.POINTER(ctypes.c_int * size)).contents
time_array = ctypes.cast(time_ptr, ctypes.POINTER(ctypes.c_int * size)).contents

# Convertir le pointeur en une liste Python
values = [data_ptr[i] for i in range(size)]  # Lire les valeurs

# Afficher les valeurs
print("Valeurs retournées par la fonction C++ :", values)


t = np.arange(0.0, 1000, 1)

fig, ax = plt.subplots()
ax.plot(t, values)

ax.set(xlabel='time (s)', ylabel='voltage (mV)',
       title='About as simple as it gets, folks')
ax.grid()

plt.show()

# Libérer la mémoire
lib.free_values(data_ptr,time_ptr)
