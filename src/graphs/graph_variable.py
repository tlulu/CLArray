# library
import matplotlib.pyplot as plt
 
# Make fake dataset
decisions = ('Row major', 'Column major', 'Offset array', 'Multi-page')
time = [250.824, 255.443, 148.359, 247.528]
y_pos = [0, 2, 4, 6]

fig, ax = plt.subplots(figsize=(10, 6))
 
# Create bars
barlist = ax.barh(y_pos, time, height=1.0, align='center')
ax.set_yticks(y_pos)
ax.set_yticklabels(decisions)
ax.invert_yaxis()
ax.set_xlabel('Time (ms)')
ax.set_title('Upload latencies')

barlist[0].set_color('orange')
barlist[1].set_color('b')
barlist[2].set_color('#00b7b7')
barlist[3].set_color('saddlebrown')
 
# Show graphic
plt.show()
