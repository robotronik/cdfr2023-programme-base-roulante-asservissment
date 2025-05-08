# pip install control matplotlib numpy
import control as ctl
import matplotlib.pyplot as plt

# Plant: 1 / (s^2 + 2s + 1)
num = [1]
den = [1, 2, 1]
plant = ctl.TransferFunction(num, den)

# PID Controller: Kp + Ki/s + Kd*s
Kp, Ki, Kd = 5, 1, 2
controller = ctl.TransferFunction([Kd, Kp, Ki], [1, 0])

# Closed-loop system
system = ctl.feedback(controller * plant)

# Time response
time, response = ctl.step_response(system)
plt.plot(time, response)
plt.title("Closed-loop Step Response")
plt.xlabel("Time (s)")
plt.ylabel("Output")
plt.grid(True)
plt.savefig("plot.png")