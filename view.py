import csv
import math
import matplotlib.pyplot as plt

position0 = {'teta': 0, 'x': 0, 'y': 0}
x_values = []
y_values = []

NUMBERSTEPBYROTATION = 360.0
DISTANCEWHEEL = 266.0
DIAMETERWHEELD =  42.56
DIAMETERWHEELG =  42.56

DISTANCEWHEEL_1 = 150.0
DIAMETERWHEELD_1 =  24.0
DIAMETERWHEELG_1 =  24.0

DISTANCEWHEEL_2 = 250.0
DIAMETERWHEELD_2 =  40.0
DIAMETERWHEELG_2 =  40.0

def compute_step_angle(wheel, distance):
    return (((wheel)*180.0)/(NUMBERSTEPBYROTATION*(distance)))

def compute_step_avance(wheel):
    return (((wheel)*math.pi)/(NUMBERSTEPBYROTATION*2))

def mod_angle(angle):
    return (angle + 180) % 360 - 180

param = {
    'stepAngleD': compute_step_angle(DIAMETERWHEELD, DISTANCEWHEEL),
    'stepAngleG': compute_step_angle(DIAMETERWHEELG, DISTANCEWHEEL),
    'stepForrwardD': compute_step_avance(DIAMETERWHEELD),
    'stepForrwardG': compute_step_avance(DIAMETERWHEELG)
}

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


with open("../../automatic1.csv", newline='') as csvfile:
    reader = csv.reader(csvfile)
    for row in reader:
        type_trigger = int(row[1])
        odometrie_calc(position0, type_trigger, param)  # Remplace None par param si nécessaire
        x_values.append(position0['x'])
        y_values.append(position0['y'])
        print(position0['x'],position0['y'])

# Tracer le chemin
plt.plot(x_values, y_values, marker='o', linestyle='-')
plt.xlabel("X")
plt.ylabel("Y")
plt.title("Trajectoire du robot")
plt.grid()
plt.show()
