import csv
import math
import matplotlib.pyplot as plt

NUMBERSTEPBYROTATION = 360.0
DISTANCEWHEEL = 250.0
DIAMETERWHEELD = 40.0
DIAMETERWHEELG = 40.0

def compute_step_angle(wheel, distance):
    return (((wheel)*180.0)/(NUMBERSTEPBYROTATION*(distance)))

def compute_step_avance(wheel):
    return (((wheel)*math.pi)/(NUMBERSTEPBYROTATION*2))

def mod_angle(angle):
    return (angle + 180) % 360 - 180

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

def compute_calibration():
    best_wheel_g, best_wheel_d, best_distance = -1, -1, -1
    best_quality = float('inf')
    compromis = 1
    total_iteration = 0
    best_results = []
    paramMinMax = []
    graphData = []
    qualityGraphData = []
    division = 4

    for i in range(10):

        if best_results:
            qualityGraphData.append(best_results[0][0])

        min_wheel_g  = best_results[0][1] - (20/(division*pow(2,i-1))) if best_results else DIAMETERWHEELG - 20
        max_wheel_g  = best_results[0][1] + (20/(division*pow(2,i-1))) if best_results else DIAMETERWHEELG + 20
        min_wheel_d  = best_results[0][2] - (20/(division*pow(2,i-1))) if best_results else DIAMETERWHEELD - 20
        max_wheel_d  = best_results[0][2] + (20/(division*pow(2,i-1))) if best_results else DIAMETERWHEELD + 20
        min_distance = best_results[0][3] - (100/(division*pow(2,i-1))) if best_results else DISTANCEWHEEL - 100
        max_distance = best_results[0][3] + (100/(division*pow(2,i-1))) if best_results else DISTANCEWHEEL + 100

        paramMinMax.append((min_wheel_g, max_wheel_g, min_wheel_d, max_wheel_d, min_distance, max_distance))
        print("\nPram : ")
        print(min_wheel_g)
        print(max_wheel_g)
        print(min_wheel_d)
        print(max_wheel_d)
        print(min_distance)
        print(max_distance)
        print("\nTOP 10 BEST :")
        for i, (q, wg, wd, d, e, f) in enumerate(best_results):
            print(f"{i+1}. quality: {q}, wheelG: {wg}, wheelD: {wd}, distance: {d}")
        print("\n\n")


        best_results.clear()
        graphData.clear()
        for wheel_g in [min_wheel_g + (i+0.5) * ((max_wheel_g - min_wheel_g) / division) for i in range(division)]:
            for wheel_d in [min_wheel_d + (i+0.5) * ((max_wheel_d - min_wheel_d) / division) for i in range(division)]:
                for distance in [min_distance + (i+0.5) * ((max_distance - min_distance) / division) for i in range(division)]:
                    total_iteration += 1
                    param = {
                        'stepAngleD': compute_step_angle(wheel_d, distance),
                        'stepAngleG': compute_step_angle(wheel_g, distance),
                        'stepForrwardD': compute_step_avance(wheel_d),
                        'stepForrwardG': compute_step_avance(wheel_g)
                    }

                    position0 = {'teta': 0, 'x': 0, 'y': 0}
                    with open("../../automatic1.csv", newline='') as csvfile:
                        reader = csv.reader(csvfile)
                        for row in reader:
                            type_trigger = int(row[1])
                            odometrie_calc(position0, type_trigger, param)

                    position1 = {'teta': 0, 'x': 0, 'y': 0}
                    with open("../../automatic2.csv", newline='') as csvfile:
                        reader = csv.reader(csvfile)
                        for row in reader:
                            type_trigger = int(row[1])
                            odometrie_calc(position1, type_trigger, param)

                    position2 = {'teta': 0, 'x': 0, 'y': 0}
                    with open("../../rotationSimple.csv", newline='') as csvfile:
                        reader = csv.reader(csvfile)
                        for row in reader:
                            type_trigger = int(row[1])
                            odometrie_calc(position2, type_trigger, param)

                    position3 = {'teta': 0, 'x': 0, 'y': 0}
                    with open("../../Line.csv", newline='') as csvfile:
                        reader = csv.reader(csvfile)
                        for row in reader:
                            type_trigger = int(row[1])
                            odometrie_calc(position3, type_trigger, param)

                    position0['teta'] = mod_angle(position0['teta'])
                    position1['teta'] = mod_angle(position1['teta'])
                    position2['teta'] = mod_angle(position2['teta'])
                    position3['teta'] = mod_angle(position3['teta'])
                    #quality = abs(position1['x'])
                    qualityTeta = + abs(position1['x']) + abs(position0['x']) + abs(position3['x']+1544) #+ abs(position2['teta']) * compromis + abs(position3['teta']) * compromis
                    qualityX = abs(position1['teta']) +  abs(position0['teta']) + abs(position3['teta'])
                    quality = qualityTeta * compromis + qualityX

                    graphData.append((quality, abs(wheel_d-40), abs(distance-250)))

                    if len(best_results) < 10:
                        best_results.append((quality, wheel_g, wheel_d, distance, qualityTeta, qualityX))
                        best_results.sort(key=lambda x: x[0])
                    elif quality < best_results[-1][0]:
                        best_results[-1] = (quality, wheel_g, wheel_d, distance,qualityTeta, qualityX)
                        best_results.sort(key=lambda x: x[0])

                    print(f"{total_iteration},{wheel_g},{wheel_d},{distance}")

    print("\nTOP 10 BEST:")
    for i, (q, wg, wd, d, teta, x) in enumerate(best_results):
        print(f"{i+1}. quality: {q}, wheelG: {wg}, wheelD: {wd}, distance: {d}, teta: {teta}, x: {x}")

    print("\nPARAM:")
    for i, (wgmin, wgmax, wdmin, wdmax, dmin, dmax) in enumerate(paramMinMax):
        print(f"{i+1:<3}, {wgmin:<10.3f}, {wgmax:<10.3f}, {wdmin:<10.3f}, {wdmax:<10.3f}, {dmin:<10.3f}, {dmax:<10.3f}")

    graphData.sort(key=lambda x: x[0])
    qualities = [x[0] for x in graphData]
    wheel_d_values = [x[1] for x in graphData]
    distance_values = [x[2] for x in graphData]

    # Tracer les courbes
    plt.figure(figsize=(10, 5))
    plt.plot(qualities, wheel_d_values, label="Wheel Diameter", marker="o")
    plt.plot(qualities, distance_values, label="Distance", marker="s")

    # Personnalisation du graphique
    plt.xlabel("Quality")
    plt.ylabel("Values")
    plt.title("Wheel Diameter & Distance vs Quality")
    plt.legend()
    plt.grid(True)

    # Affichage
    plt.show()

    plt.figure(figsize=(8, 4))
    plt.plot(qualityGraphData, marker='x', linestyle='-', color='purple')
    plt.title("QualityGraphData over Iterations")
    plt.xlabel("Index")
    plt.ylabel("Quality")
    plt.grid(True)

    # Affichage de la deuxième figure
    plt.show()



    return True



# Exemple d'utilisation
compute_calibration()

        # wheelG: 40.61
        # wheelD: 40.919999999999995
        # distance: 253.828

        # wheelG: 42.209999999999994
        # wheelD: 41.32
        # distance: 255.028

        # BEST:
        # wheelG: 40.41
        # wheelD: 41.519999999999996
        # distance: 255.228