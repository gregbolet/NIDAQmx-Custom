import numpy as np
import matplotlib.pyplot as plt
import os
import glob
import pandas as pd
from scipy.interpolate import *

plt.rcParams['figure.figsize'] = [12, 5]
plt.rcParams['figure.dpi'] = 100

#os.chdir("/content/drive/MyDrive/Advanced_Systems_Final_Project/")

df = pd.read_csv('testout1.csv')

lines = df.columns[1:]
print(lines)

# Average out the lines
df['mean'] = df[lines].mean(axis=1)

print(df.tail(10))
print(df.shape)
print(df.columns)

fig, ax = plt.subplots(figsize=(15,10))

for line in lines:
  ax.plot(df.time, df[line], '--', linewidth=2, label=line)


ax.set_title('CPU Power Line Consumption Over Time')
ax.set_ylabel('Power (watts)')
ax.set_xlabel('Time (seconds)')

ax.legend()

plt.show()

x = df['time']
y = df['mean']

fig, ax = plt.subplots(figsize=(15,10))

ax.plot(df.time, df['mean'], '--', linewidth=2, c='black')
ax.scatter(x, y, c='red', label='Raw Data')

# Make interpolation spline as summary data
cs = CubicSpline(x, y)
xnew = np.linspace(x.min(), x.max(), 15)
ynew = cs(xnew)

ax.plot(xnew, ynew, label='Interpolated Data', linewidth=3)

ax.legend()

ax.set_title('CPU Lines -- Mean Power Consumption Over Time')
ax.set_ylabel('Mean Power (watts) From All Lines')
ax.set_xlabel('Time (seconds)')

plt.savefig('image.jpg')
#plt.show()
