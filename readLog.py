import re
import matplotlib.pyplot as plt

# Chemin vers le fichier log (à adapter si nécessaire)
log_file_path = 'asserv_345.log'

# Expressions régulières pour extraire les coordonnées
coordinate_pattern = re.compile(r'get_coordinates\s*:\s*x\s*(-?\d+),\s*y\s*(-?\d+)')

# Listes pour stocker les points
x_coords = []
y_coords = []

# Lecture et extraction des coordonnées
with open(log_file_path, 'r') as file:
    for line in file:
        match = coordinate_pattern.search(line)
        if match:
            x = int(match.group(1))
            y = int(match.group(2))
            x_coords.append(x)
            y_coords.append(y)

# Vérification s'il y a des coordonnées
if not x_coords:
    print("Aucune coordonnée trouvée dans le log.")
else:
    # Affichage du trajet
    plt.figure(figsize=(8, 6))
    plt.plot(x_coords, y_coords, marker='o', linestyle='-', color='blue')
    plt.title('Trajet basé sur get_coordinates')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.grid(True)
    plt.axis('equal')  # Pour garder le bon ratio des distances
    plt.show()