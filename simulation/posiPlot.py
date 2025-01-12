import ctypes

# Charger la bibliothèque partagée
lib = ctypes.CDLL('./positionControlTB.so')

# Définir les prototypes des fonctions C++
lib.get_values.restype = ctypes.POINTER(ctypes.c_int)  # get_values retourne un pointeur sur int
lib.get_values.argtypes = [ctypes.POINTER(ctypes.c_int)]  # Prend un pointeur sur int comme argument

lib.free_values.argtypes = [ctypes.POINTER(ctypes.c_int)]  # free_values prend un pointeur sur int

# Appeler la fonction `get_values`
size = ctypes.c_int()  # Créer une variable pour stocker la taille du tableau
values_ptr = lib.get_values(ctypes.byref(size))  # Obtenir le pointeur et la taille

# Convertir le pointeur en une liste Python
values = [values_ptr[i] for i in range(size.value)]  # Lire les valeurs

# Afficher les valeurs
print("Valeurs retournées par la fonction C++ :", values)

# Libérer la mémoire
lib.free_values(values_ptr)
