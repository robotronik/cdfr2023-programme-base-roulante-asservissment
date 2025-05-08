# pip install control matplotlib numpy
import control as ctl
import matplotlib.pyplot as plt
import numpy as np

# Motor:
num = [1]
den = [1, 1]
motor = ctl.TransferFunction(num, den)

# PID Controller parameters
Kp_values = [0.2, 0.3, 0.4, 0.6, 1]  # List of Kp values to test
Kd_values = [0, 0.1, 0.3, 0.5, 1]  # List of Kd values to test

# Pure integrator representing the wheels
wheels = ctl.TransferFunction([1], [1, 0])

time_scale = np.linspace(0, 15, 1000)  # From 0 to 10 seconds, 1000 points

# Plot responses for different Kp values
plt.figure()
for Kp in Kp_values:
    controller = ctl.TransferFunction([0, Kp, 0], [1, 0])
    system = ctl.feedback(controller * motor * wheels)
    time, response = ctl.step_response(system, T=time_scale)
    plt.plot(time, response, label=f"Kp={Kp}")

# Add plot details
plt.title("Closed-loop Step Response for Different Kp Values")
plt.xlabel("Time (s)")
plt.ylabel("Output")
plt.legend()
plt.grid(True)
plt.savefig("plot_multiple_kp.png")

# Plot responses for different Kp values
plt.figure()
for Kd in Kd_values:
    controller = ctl.TransferFunction([Kd, 1, 0], [1, 0])
    system = ctl.feedback(controller * motor * wheels)
    time, response = ctl.step_response(system, T=time_scale)
    plt.plot(time, response, label=f"Kd={Kd}")

# Add plot details
plt.title("Closed-loop Step Response for Different Kd Values")
plt.xlabel("Time (s)")
plt.ylabel("Output")
plt.legend()
plt.grid(True)
plt.savefig("plot_multiple_kd.png")