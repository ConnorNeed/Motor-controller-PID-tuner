import matplotlib.pyplot as plt
from decimal import *
getcontext().prec = 7

pids_and_error = []
with open("data/data_log") as data:
  for line in data:
    if line.startswith("PID"):
      line = line.split(" ")
      kp, ki, kd, error = Decimal(line[1][3:-1]), Decimal(line[2][3:-1]), Decimal(line[3][3:-1]), Decimal(line[-1])
      pids_and_error.append((kp, ki, kd, error))

kp_error = [((val[0]), (val[-1])) for val in pids_and_error if val[-1] < 10000]
ki_error = [((val[1]), (val[-1])) for val in pids_and_error if val[-1] < 10000]
kd_error = [((val[2]), (val[-1])) for val in pids_and_error if val[-1] < 10000]

# Unpack data for plotting
kp_vals, kp_errors = zip(*kp_error)
ki_vals, ki_errors = zip(*ki_error)
kd_vals, kd_errors = zip(*kd_error)

# Create subplots
fig, axs = plt.subplots(3, 1, figsize=(8, 12))

# Plot kp vs error
axs[0].scatter(kp_vals, kp_errors, color='r', label='Kp vs Error')
axs[0].set_title('Kp vs Error')
axs[0].set_xlabel('Kp')
axs[0].set_ylabel('Error')
axs[0].legend()

# Plot ki vs error
axs[1].scatter(ki_vals, ki_errors, color='g', label='Ki vs Error')
axs[1].set_title('Ki vs Error')
axs[1].set_xlabel('Ki')
axs[1].set_ylabel('Error')
axs[1].legend()

# Plot kd vs error
axs[2].scatter(kd_vals, kd_errors, color='b', label='Kd vs Error')
axs[2].set_title('Kd vs Error')
axs[2].set_xlabel('Kd')
axs[2].set_ylabel('Error')
axs[2].legend()

# Adjust layout and show plot
plt.tight_layout()
plt.show()