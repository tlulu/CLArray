# library
import matplotlib.pyplot as plt
 
# Make fake dataset
decisions = ('2-bit Packing: OFF', '2-bit Packing: ON', 'Prefetching: OFF', 'Prefetching: ON', 'Row Major', 'Column Major')
time = [271.989, 22.474, 80.8592, 79.7133, 46.02, 45.5282]
y_pos = [0, 1, 3, 4, 6, 7]

fig, ax = plt.subplots(figsize=(12, 6))
 
# Create bars
barlist = ax.barh(y_pos, time, height=1.0, align='center')
ax.set_yticks(y_pos)
ax.set_yticklabels(decisions)
ax.invert_yaxis()
ax.set_xlabel('Time (ms)')
ax.set_title('Upload latencies')

for i in range(len(barlist)):
	if i%2 == 0:
		barlist[i].set_color('#00b7b7')
	else:
		barlist[i].set_color('b')
 
# Show graphic
plt.show()
