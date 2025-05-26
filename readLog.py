import os
import re
import plotly.graph_objs as go
import base64
import subprocess

# === CONFIGURATION ===
ROBOT_HOST = "raspitronik.local"
ROBOT_USER = "robotronik"
REMOTE_DIR = "~/LOG_CDFR2025/"
LOCAL_CACHE_DIR = ".logCache"

# Crée le dossier local si nécessaire
os.makedirs(LOCAL_CACHE_DIR, exist_ok=True)

def run_command(command, **kwargs):
    print(f"\n[CMD] {' '.join(command)}")
    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            **kwargs
        )
        if result.returncode != 0:
            print(f"[ERREUR] Code de retour : {result.returncode}")
            print(f"[STDERR] {result.stderr.strip()}")
            print(f"[STDOUT] {result.stdout.strip()}")
            raise subprocess.CalledProcessError(result.returncode, command)
        return result
    except subprocess.CalledProcessError as e:
        print(f"[EXCEPTION] Commande échouée : {e}")
        return None

# === Liste les fichiers locaux ===
def list_local_logs():
    asservFilePatern = re.compile(r'asserv_(-?\d+).log')
    finalList = []
    for f in os.listdir(LOCAL_CACHE_DIR):
        name = os.path.basename(f)
        file_match = asservFilePatern.search(name)
        if file_match:
            num = int(file_match.group(1))
            finalList.append((num, name))
    finalList.sort(key=lambda x: x[0])
    return finalList

# === Liste les fichiers distants via SSH ===
def list_remote_logs():
    asservFilePatern = re.compile(r'asserv_(-?\d+).log')
    finalList = []
    try:
        result = run_command(
            ["ssh", f"{ROBOT_USER}@{ROBOT_HOST}", f"ls {REMOTE_DIR}*.log"],
            timeout=5
        )
        if result.returncode != 0:
            raise Exception(result.stderr.strip())
        for f in result.stdout.strip().splitlines():
            name = os.path.basename(f)
            file_match = asservFilePatern.search(name)
            if file_match:
                num = int(file_match.group(1))
                finalList.append((num, name))
        finalList.sort(key=lambda x: x[0])
        return finalList
    except Exception as e:
        print(f"[INFO] Impossible de se connecter au robot ({ROBOT_HOST}).\nMotif : {e}")
        return []

# === Copier un fichier depuis le robot ===
def fetch_remote_file(filename):
    remote_path = f"{ROBOT_USER}@{ROBOT_HOST}:{REMOTE_DIR}{filename}"
    local_path = os.path.join(LOCAL_CACHE_DIR, filename)
    try:
        run_command(["scp", remote_path, local_path], check=True)
        return local_path
    except Exception as e:
        print(f"[ERREUR] Échec du transfert SCP : {e}")
        return []


# === INTERACTION UTILISATEUR ===

print("Fichiers distants disponibles sur le robot :")
remote_files = list_remote_logs()
for num, fname in remote_files:
    print(f"[R{num}] {fname}")

print("\nFichiers locaux disponibles dans .logCache :")
local_files = list_local_logs()
for num, fname in local_files:
    print(f"[L{num}] {fname}")

choice = input("\nChoisissez un fichier ([L<num>] ou [R<num>]) : ")
selected_file = "asserv_" + choice[1:] + ".log"


if choice.startswith("L"):
    log_file_path = os.path.join(LOCAL_CACHE_DIR, selected_file)
elif choice.startswith("R"):
    log_file_path = fetch_remote_file(selected_file)
    print(f"Fichier '{selected_file}' copié depuis le robot.")
else:
    raise ValueError("Choix invalide")

# === Traitement du fichier choisi ===
coord_pattern = re.compile(r'get_coordinates\s*:\s*x\s*(-?\d+),\s*y\s*(-?\d+),')

event_patterns = [
    {
        "name": "go_to_point",
        "pattern": re.compile(r'(go_to_point.*)'),
        "color": "green",
        "symbol": "cross"
    },
    {
        "name": "stop",
        "pattern": re.compile(r'(stop.*)'),
        "color": "red",
        "symbol": "circle"
    },
    {
        "name": "pause",
        "pattern": re.compile(r'(pause.*)'),
        "color": "orange",
        "symbol": "diamond"
    },
    {
        "name": "resume",
        "pattern": re.compile(r'(resume.*)'),
        "color": "purple",
        "symbol": "triangle-up"
    },
    # Ajoute ici d'autres patterns si besoin
]

# Variables pour stocker les données

x_coords = []
y_coords = []

event_points = {event["name"]: [] for event in event_patterns}

last_position = None

# Lecture du log
with open(log_file_path, 'r') as f:
    for line in f:
        coord_match = coord_pattern.search(line)
        if coord_match:
            last_position = (int(coord_match.group(1)), int(coord_match.group(2)))
            x_coords.append(last_position[0])
            y_coords.append(last_position[1])
            continue

        for event in event_patterns:
            match = event["pattern"].search(line)
            if match and last_position:
                label = match.group(1).strip()
                event_points[event["name"]].append((last_position[0], last_position[1], label))
                break  # évite qu'une ligne match plusieurs events

# === Traces Plotly ===

# Trace principale du chemin
trace_path = go.Scatter(
    y=x_coords,
    x=y_coords,
    mode='lines+markers',
    name='Trajet',
    line=dict(color='blue')
)

# Traces dynamiques pour les événements
event_traces = []
for event in event_patterns:
    points = event_points[event["name"]]
    if not points:
        continue
    trace = go.Scatter(
        y=[p[0] for p in points],
        x=[p[1] for p in points],
        mode='markers',
        name=event["name"],
        marker=dict(color=event["color"], size=10, symbol=event["symbol"]),
        text=[p[2] for p in points],
        hoverinfo='text'
    )
    event_traces.append(trace)

# Liste finale des traces à passer à go.Figure(data=...)
all_traces = [trace_path] + event_traces


image_path = "../../stategie/2025/Vinyl.svg"

# Charger et encoder le fichier SVG
with open(image_path, "r", encoding="utf-8") as svg_file:
    svg_content = svg_file.read()
    encoded_svg = base64.b64encode(svg_content.encode("utf-8")).decode()

# Créer l'URL base64 pour image SVG
image_base64 = "data:image/svg+xml;base64," + encoded_svg

# Layout avec l'image encodée
layout = go.Layout(
    title='Trajet avec commandes',
    xaxis=dict(title='Y', range=[-1500, 1500], scaleanchor='y'),
    yaxis=dict(title='X', range=[ 1000,-1000]),
    showlegend=True,
    images=[
        dict(
            source=image_base64,
            xref="x",
            yref="y",
            x=-1500,         # En bas à gauche
            y=-1000,          # En haut à gauche (Y croissant vers le haut)
            sizex=3000,
            sizey=2000,
            sizing="stretch",
            opacity=1,
            layer="below"
        )
    ]
)
fig = go.Figure(data=all_traces, layout=layout)
fig.show()