
import matplotlib.pyplot as plt
import numpy as np

class DesignationsDrawer:
    def __init__(self, designationsFilename):
        self.file = open(designationsFilename, 'r')
    
    def draw(self, withDiscontinuties):
        filedata = self.file.read().split('\n')
        
        data = {}

        def append(id, t,dist,azm,ang, index=-1):
            if index == -1:
                data[id]['time'].append(t)
                data[id]['dist'].append(dist)
                data[id]['azimuth'].append(azm)
                data[id]['angle'].append(ang)
            else:
                data[id]['time'].insert(index, t)
                data[id]['dist'].insert(index, dist)
                data[id]['azimuth'].insert(index, azm)
                data[id]['angle'].insert(index, ang)

        dt = 1e9

        for line in filedata[1:-1]:
            t,id,dist,azm,ang = [float(a) for a in line.split('\t')]
            id = int(id)

            if id not in data:
                data[id] = {'time': [], 'dist': [], 
                            'azimuth': [], 'angle': []}
            if data[id]['time']:
                dt = min(dt, t - data[id]['time'][-1])
            
            append(id, t,dist,azm,ang)
        
        # include discontinuties
        if withDiscontinuties:
            for id in data:
                t_idx = len(data[id]['time']) - 1
                while t_idx != 1:
                    if data[id]['time'][t_idx] - data[id]['time'][t_idx-1] != dt:
                        append(id, *[np.nan]*4, t_idx)
                    elif abs(data[id]['azimuth'][t_idx] - data[id]['azimuth'][t_idx-1]) > 18 / np.pi:
                        data[id]['azimuth'][t_idx] = np.nan
                    t_idx -= 1

        for id in data:
            t = np.array(data[id]['time']) / 3600.0 
            fig, axs = plt.subplots(3, sharex=True)
            fig.suptitle(f'Station {id} designations')
            axs[0].plot(t, data[id]['dist'])
            axs[0].set_title("Distance")
            axs[1].plot(t, data[id]['azimuth'])
            axs[1].set_title("Azimuth")
            axs[2].plot(t, data[id]['angle'])
            axs[2].set_title("Angle")

            axs[0].set(ylabel='km')
            axs[1].set(ylabel='degrees')
            axs[2].set(ylabel='degrees')

            axs[2].set(xlabel='time, hours')
