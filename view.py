import csv
import math
import matplotlib.pyplot as plt

# Paramètres de base
NUMBERSTEPBYROTATION = 360.0
    #define DISTANCEWHEEL 254.828 // + = teat -
    #define DIAMETERWHEELD 41.12                // +1 roue = +25 distance
    #define DIAMETERWHEELG 41.410 // + = y -     // +1 roue = +25 distance

# Définition des trois configurations de roues
configurations = {
    "Config 1 ": {"DISTANCEWHEEL": 254.828, "DIAMETERWHEELD": 41.12, "DIAMETERWHEELG": 41.41},
    "Config 2 ": {"DISTANCEWHEEL": 266.0, "DIAMETERWHEELD": 42.56, "DIAMETERWHEELG": 42.56},
    "Config 3 ": {"DISTANCEWHEEL": 150.0, "DIAMETERWHEELD": 24.0, "DIAMETERWHEELG": 24.0},
    #"Config 4 ": {"DISTANCEWHEEL": 250.0, "DIAMETERWHEELD": 40.0, "DIAMETERWHEELG": 40.0},
    # "Config 4 ": {"DISTANCEWHEEL": 350.0, "DIAMETERWHEELD": 56.0, "DIAMETERWHEELG": 56.0},
    # "Config 5 ": {"DISTANCEWHEEL": 270.0, "DIAMETERWHEELD": 44.0, "DIAMETERWHEELG": 44.0},
    # "Config 6 ": {"DISTANCEWHEEL": 330.0, "DIAMETERWHEELD": 52.0, "DIAMETERWHEELG": 52.0},
    # "Config 7 ": {"DISTANCEWHEEL": 170.0, "DIAMETERWHEELD": 28.0, "DIAMETERWHEELG": 28.0},
    # "Config 8 ": {"DISTANCEWHEEL": 230.0, "DIAMETERWHEELD": 36.0, "DIAMETERWHEELG": 36.0},
    # "Config 9 ": {"DISTANCEWHEEL": 290.0, "DIAMETERWHEELD": 48.0, "DIAMETERWHEELG": 48.0},
    # "Config 10": {"DISTANCEWHEEL": 190.0, "DIAMETERWHEELD": 32.0, "DIAMETERWHEELG": 32.0},
    # "Config 11": {"DISTANCEWHEEL": 310.0, "DIAMETERWHEELD": 52.0, "DIAMETERWHEELG": 52.0},
}

# Liste des fichiers à traiter
files = {
    "Fichier 1": "../../automatic1.csv",
    "Fichier 2": "../../automatic2.csv",
    "Fichier 3": "../../rotationSimple.csv",
    "Fichier 4": "../../Line.csv",
    "Fichier 5": "../../rotationLine.csv"
}

# Couleurs définies pour chaque configuration
colors = ["blue", "red", "green"]

# Fonctions de calcul
def compute_step_angle(wheel, distance):
    return (((wheel) * 180.0) / (NUMBERSTEPBYROTATION * distance))

def compute_step_avance(wheel):
    return (((wheel) * math.pi) / (NUMBERSTEPBYROTATION * 2))

def odometrie_calc(position, type_trigger, param):
    coef = math.radians(position['teta'] + 90)
    if type_trigger == 0:  # forwardL
        position['y'] -= param['stepForrwardG'] * math.cos(coef)
        position['x'] -= param['stepForrwardG'] * math.sin(coef)
        position['teta'] -= param['stepAngleG']
    elif type_trigger == 1:  # backwardL
        position['y'] += param['stepForrwardG'] * math.cos(coef)
        position['x'] += param['stepForrwardG'] * math.sin(coef)
        position['teta'] += param['stepAngleG']
    elif type_trigger == 2:  # forwardR
        position['y'] -= param['stepForrwardD'] * math.cos(coef)
        position['x'] -= param['stepForrwardD'] * math.sin(coef)
        position['teta'] += param['stepAngleD']
    elif type_trigger == 3:  # backwardR
        position['y'] += param['stepForrwardD'] * math.cos(coef)
        position['x'] += param['stepForrwardD'] * math.sin(coef)
        position['teta'] -= param['stepAngleD']

# Tracer les trajectoires pour chaque configuration et chaque fichier
plt.figure(figsize=(8, 6))

for file_index, (file_label, file_path) in enumerate(files.items()):
    for config_index, (config_name, config) in enumerate(configurations.items()):
        position = {'teta': 0, 'x': 0, 'y': 0}
        x_values, y_values = [], []

        param = {
            'stepAngleD': compute_step_angle(config['DIAMETERWHEELD'], config['DISTANCEWHEEL']),
            'stepAngleG': compute_step_angle(config['DIAMETERWHEELG'], config['DISTANCEWHEEL']),
            'stepForrwardD': compute_step_avance(config['DIAMETERWHEELD']),
            'stepForrwardG': compute_step_avance(config['DIAMETERWHEELG'])
        }

        # Lire les données du fichier
        with open(file_path, newline='') as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                type_trigger = int(row[1])
                odometrie_calc(position, type_trigger, param)
                x_values.append(position['x'])
                y_values.append(position['y'])

        # Tracer la courbe avec une couleur unique pour chaque config
        plt.plot(x_values, y_values, marker='o', linestyle='-', color=colors[config_index], label=f"{config_name} - {file_label}")

# Configuration du graphique
plt.xlabel("X")
plt.ylabel("Y")
plt.title("Comparaison des trajectoires du robot pour deux fichiers")
plt.legend()
plt.grid()
plt.show()

