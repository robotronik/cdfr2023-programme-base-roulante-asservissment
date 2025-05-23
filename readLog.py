import re
import plotly.graph_objs as go
import base64

# Chemin vers le fichier log
log_file = 'asserv_521.log'

# Expressions régulières
coord_pattern = re.compile(r'get_coordinates\s*:\s*x\s*(-?\d+),\s*y\s*(-?\d+),')
goto_pattern = re.compile(r'(go_to_point\s*:.*)')
stop_pattern = re.compile(r'stop')

# Variables pour stocker les données
x_coords = []
y_coords = []

hover_texts = []
goto_points = []
stop_points = []

last_position = None

# Lecture du log
with open(log_file, 'r') as f:
    for line in f:
        coord_match = coord_pattern.search(line)
        if coord_match:
            last_position = (int(coord_match.group(1)), int(coord_match.group(2)))
            x_coords.append(last_position[0])
            y_coords.append(last_position[1])

        elif goto_pattern.search(line) and last_position:
            match = goto_pattern.search(line)
            label = match.group(1).strip()
            goto_points.append((last_position[0], last_position[1], label))

        elif stop_pattern.search(line) and last_position:
            stop_points.append((last_position[0], last_position[1]))

# Trace principale du chemin
trace_path = go.Scatter(
    y=x_coords,
    x=y_coords,
    mode='lines+markers',
    name='Trajet',
    line=dict(color='blue')
)

# Points go_to avec hover
trace_goto = go.Scatter(
    y=[p[0] for p in goto_points],
    x=[p[1] for p in goto_points],
    mode='markers',
    name='go_to_point',
    marker=dict(color='green', size=10, symbol='cross'),
    text=[p[2] for p in goto_points],
    hoverinfo='text'
)

# Points stop
trace_stop = go.Scatter(
    y=[p[0] for p in stop_points],
    x=[p[1] for p in stop_points],
    mode='markers',
    name='stop',
    marker=dict(color='red', size=10, symbol='circle'),
    hoverinfo='text',
    text=["stop"] * len(stop_points)
)


image_path = "../../stategie/2025/CoupeRobotique.png"

# Charger et encoder l'image
with open(image_path, "rb") as image_file:
    encoded_image = base64.b64encode(image_file.read()).decode()

# Créer l'URL base64
image_base64 = "data:image/png;base64," + encoded_image

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
fig = go.Figure(data=[trace_path, trace_goto, trace_stop], layout=layout)
fig.show()