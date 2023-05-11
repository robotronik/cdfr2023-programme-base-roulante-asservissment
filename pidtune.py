import tkinter as tk
import random

import numpy as np
from tkinter import ttk, messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

import struct
import serial
import math

comport:serial.Serial = None
rcvbuff:bytes = b""
posX=0; posY=0; rot=0
asservState = None

def ConnectCOM(port:str):
	global comport
	comport = serial.Serial(port, baudrate=115200, bytesize=8, stopbits=1)
	print(f"comport open ? :{comport.is_open}")
	
#0: linear pos
#1: linear vel
#2: angular pos
#3: angular vel
def SendPID(pidindex, kP, kI, kD):
	global comport
	if comport is None:
		return
	sendstr = "!I2CS32:" + str(pidindex+40) + ","
	paramsbin = struct.pack("<fff", kP, kI, kD)
	bink = struct.unpack("<iii", paramsbin)
	print(paramsbin, bink, hex(0))
	for i in range(len(bink)):
		sendstr += hex(bink[i])[2:] + ("\n" if i == len(bink)-1 else ",")
	print(sendstr.encode("ascii"))
	comport.write(sendstr.encode("ascii"))

def ResetPos():
	if comport is None:
		return
	comport.write("!I2CSend:21,0,0,0\n".encode("ascii"))

def RequestPos():
	if comport is None:
		return
	sendstr = "!I2CRequest:6\n!I2CSend:20\n"
	#print(sendstr.encode("ascii"))
	comport.write(sendstr.encode("ascii"))

def StopRobot():
	if comport is None:
		return
	comport.write("!I2CSend:33\n".encode("ascii"))

def SetTargetPos(tx, ty, bwd:bool):
	global asservState
	if comport is None:
		return
	sendstr = "!I2CSend:30,"+str(int(tx))+","+str(int(ty))+","+str(int(bwd))+"\n"
	comport.write(sendstr.encode("ascii"))
	asservState = (1, tx, ty, bwd)

def SetTargetRot(rot):
	global asservState
	if comport is None:
		return
	sendstr = "!I2CSend:31,"+str(int(rot))+"\n"
	comport.write(sendstr.encode("ascii"))
	asservState = (2, rot)

def computeErr():
	if asservState is None:
		return 0
	if asservState[0] == 1:
		dx = asservState[1]-posX
		dy = asservState[2]-posY
		dist = math.sqrt(dx*dx+dy*dy)
		angletotarget = math.atan2(dy, dx)
		if angletotarget > math.pi/2:
			return -dist
		else:
			return dist
	if asservState[1] == 2:
		drot = (asservState[1]-rot) %360
		if drot > 180:
			drot -= 360
		return drot


def processReceived():
	global comport
	global rcvbuff
	global posX, posY, rot
	if comport is None:
		rcvbuff = b""
		return
	
	rcvbuff += comport.read_all()
	commandstartidx = 0
	for i in range(len(rcvbuff)):
		if rcvbuff[i] == '\n':
			commandstring = (rcvbuff[commandstartidx:i+1]).decode("ascii")
			commandstring = commandstring.rstrip("\r\n\0").lstrip("!")
			print(commandstring)
			commandstartidx = i+1
			commandparts = commandstring.split(":")
			if commandparts[0] == "I2C":
				if len(commandparts) != 2:
					continue
				positions = commandparts[1].split(",")
				if len(positions) != 3:
					continue
				posX = int(positions[0])
				posY = int(positions[1])
				rot = int(positions[2])
	rcvbuff = rcvbuff[commandstartidx:]


# Créer une instance de la classe Tk
root = tk.Tk()

# Définir les propriétés de la fenêtre principale
root.title("Zerbib PID Auto-tunner 3000 (made by Victor RS)")
root.geometry("830x550")

# Section 1 : COM
section1 = tk.Frame(root)
section1.pack(side="top", fill="x")

label1 = tk.Label(section1, text="COM")
label1.pack(side="left")

entry1 = tk.Entry(section1)
entry1.pack(side="left", padx=5)


def connect_com(port):
	ConnectCOM(port)
	print(port)
	#messagebox.showinfo(f"Opening com port: {port}")


button1 = tk.Button(section1, text="Connexion")
button1.pack(side="left", padx=5)
button1.bind("<Button-1>", lambda event: connect_com(entry1.get()))

# Section 2 : Onglets avec champs KP, KI, KD et bouton Envoyer
section2 = tk.Frame(root)

notebook = ttk.Notebook(section2)

figsection = tk.Frame(section2)
figsection.pack(side="right")

section2.pack(side="top", fill="both", expand=True)

linpos = tk.Frame(notebook)
linvel = tk.Frame(notebook)
angpos = tk.Frame(notebook)
angvel = tk.Frame(notebook)

notebook.add(linpos, text="LinPos")
notebook.add(linvel, text="LinVel")
notebook.add(angpos, text="AngPos")
notebook.add(angvel, text="AngVel")

notebook.pack(side="left", fill="y", expand=True)

contexttoindex = {"linpos":0, "linvel":1, "angpos":2, "angvel":3}

def apply_pid_tunning(context: str, kp_str: str, ki_str: str, kd_str: str):
	#try:
		kp, ki, kd = float(kp_str), float(ki_str), float(kd_str)
		messagebox.showinfo("Apply PID tunning", f"Application de KP={kp}, KI={ki}, KD={kd} on {context}")
		SendPID(contexttoindex[context], kp, ki, kd)
	#except:
	#	messagebox.showinfo("Apply PID tunning", f"Invalid params type for {context}")


# Onglet InPost
middle_frame = tk.Frame(linpos)
inpost_ki = tk.Frame(linpos)
inpost_kd = tk.Frame(linpos)

label2_1 = tk.Label(middle_frame, text="KP")
label2_1.pack(side="left", padx=5)
entry2_1 = tk.Entry(middle_frame)
entry2_1.pack(side="left", padx=5)
middle_frame.pack(side="top")

label2_2 = tk.Label(inpost_ki, text="KI")
label2_2.pack(side="left", padx=5)
entry2_2 = tk.Entry(inpost_ki)
entry2_2.pack(side="left", padx=5)
inpost_ki.pack(side="top")

label2_3 = tk.Label(inpost_kd, text="KD")
label2_3.pack(side="left", padx=5)
entry2_3 = tk.Entry(inpost_kd)
entry2_3.pack(side="left", padx=5)
inpost_kd.pack(side="top")

button2 = tk.Button(linpos, text="Envoyer")
button2.pack(side="top", padx=5)
button2.bind("<Button-1>", lambda event: apply_pid_tunning("linpos", entry2_1.get(), entry2_2.get(), entry2_3.get()))

# Onglet InVel
invel_kp = tk.Frame(linvel)
invel_ki = tk.Frame(linvel)
invel_kd = tk.Frame(linvel)
label3_1 = tk.Label(invel_kp, text="KP")
label3_1.pack(side="left", padx=5)
entry3_1 = tk.Entry(invel_kp)
entry3_1.pack(side="left", padx=5)
invel_kp.pack(side="top")

label3_2 = tk.Label(invel_ki, text="KI")
label3_2.pack(side="left", padx=5)
entry3_2 = tk.Entry(invel_ki)
entry3_2.pack(side="left", padx=5)
invel_ki.pack(side="top")

label3_3 = tk.Label(invel_kd, text="KD")
label3_3.pack(side="left", padx=5)
entry3_3 = tk.Entry(invel_kd)
entry3_3.pack(side="left", padx=5)
invel_kd.pack(side="top")

button3 = tk.Button(linvel, text="Envoyer")
button3.pack(side="top", padx=5)
button3.bind("<Button-1>", lambda event: apply_pid_tunning("linvel", entry3_1.get(), entry3_2.get(), entry3_3.get()))

# Onglet AngPos
angpos_kp = tk.Frame(angpos)
angpos_ki = tk.Frame(angpos)
angpos_kd = tk.Frame(angpos)
label4_1 = tk.Label(angpos_kp, text="KP")
label4_1.pack(side="left", padx=5)
entry4_1 = tk.Entry(angpos_kp)
entry4_1.pack(side="left", padx=5)
angpos_kp.pack(side="top")

label4_2 = tk.Label(angpos_ki, text="KI")
label4_2.pack(side="left", padx=5)
entry4_2 = tk.Entry(angpos_ki)
entry4_2.pack(side="left", padx=5)
angpos_ki.pack(side="top")

label4_3 = tk.Label(angpos_kd, text="KD")
label4_3.pack(side="left", padx=5)
entry4_3 = tk.Entry(angpos_kd)
entry4_3.pack(side="left", padx=5)
angpos_kd.pack(side="top")

button4 = tk.Button(angpos, text="Envoyer")
button4.pack(side="top", padx=5)
button4.bind("<Button-1>", lambda event: apply_pid_tunning("angpos", entry4_1.get(), entry4_2.get(), entry4_3.get()))

# Onglet AngVel
angvel_kp = tk.Frame(angvel)
angvel_ki = tk.Frame(angvel)
angvel_kd = tk.Frame(angvel)
label5_1 = tk.Label(angvel_kp, text="KP")
label5_1.pack(side="left", padx=5)
entry5_1 = tk.Entry(angvel_kp)
entry5_1.pack(side="left", padx=5)
angvel_kp.pack(side="top")

label5_2 = tk.Label(angvel_ki, text="KI")
label5_2.pack(side="left", padx=5)
entry5_2 = tk.Entry(angvel_ki)
entry5_2.pack(side="left", padx=5)
angvel_ki.pack(side="top")

label5_3 = tk.Label(angvel_kd, text="KD")
label5_3.pack(side="left", padx=5)
entry5_3 = tk.Entry(angvel_kd)
entry5_3.pack(side="left", padx=5)
angvel_kd.pack(side="top")

button5 = tk.Button(angvel, text="Envoyer")
button5.pack(side="top", padx=5)
button5.bind("<Button-1>", lambda event: apply_pid_tunning("angvel", entry5_1.get(), entry5_2.get(), entry5_3.get()))


def stop():
	StopRobot()
	#messagebox.showinfo("Control", f"Stoup ! On nous bouge plou")


def test_angle():
	ResetPos()
	global line 
	line.set_xdata([0])
	line.set_ydata([0])
	SetTargetRot(90)
	#messagebox.showinfo("Control", f"Zig zoug")


def test_pos():
	ResetPos()
	global line 
	line.set_xdata([0])
	line.set_ydata([0])
	SetTargetPos(100, 0, 0)
	#messagebox.showinfo("Control", f"Zig zag")


# Section 3 : Boutons
section3 = tk.Frame(root)
section3.pack(side="top", fill="x")

button6 = tk.Button(section3, text="Stop")
button6.pack(side="left", padx=5)
button6.bind("<Button-1>", lambda event: stop())

button7 = tk.Button(section3, text="Test angle")
button7.pack(side="left", padx=5)
button7.bind("<Button-1>", lambda event: test_angle())

button8 = tk.Button(section3, text="Test pos")
button8.pack(side="left", padx=5)
button8.bind("<Button-1>", lambda event: test_pos())

# Créer une figure matplotlib
fig = Figure(figsize=(5, 4), dpi=100)
ax = fig.add_subplot(111)
line, = ax.plot([0], [0], 'b-')


# Créer une fonction qui met à jour les données
def update_data():
	xdata = line.get_xdata()
	ydata = line.get_ydata()
	RequestPos()
	processReceived()
	if asservState is not None :
		xdata = np.append(xdata, xdata[-1] + 1)
		error = computeErr()
		if asservState[0] == 1:
			#prevent the robot from turning around
			if error > 0 and asservState[3]: #positive error and going backwards
				SetTargetPos(asservState[1], asservState[2], False)
			if error < 0 and not asservState[3]: #negative error and going forwards
				SetTargetPos(asservState[1], asservState[2], True)
		ydata = np.append(ydata, error)
	line.set_data(xdata, ydata)
	ax.relim()
	ax.autoscale_view()
	canvas.draw()


# Créer une fonction de rappel pour mettre à jour les données toutes les secondes
def update_graph():
	update_data()
	root.after(20, update_graph)


# Ajouter la figure à la fenêtre Tkinter
canvas = FigureCanvasTkAgg(fig, master=figsection)
canvas.draw()
canvas.get_tk_widget().pack()

# Planifier la première mise à jour des données
root.after(20, update_graph)

# Lancer la boucle principale de la fenêtre
root.mainloop()