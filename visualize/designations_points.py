import numpy as np
import matplotlib.pyplot as plt

class DesignationsPoints:
    def __init__(self, measurementsFN, guessFN, resultFN, timeFN):
        m = open(measurementsFN)
        g = open(guessFN)
        r = open(resultFN)
        t = open(timeFN)

        self.meas = []
        self.guess = []
        self.res = []
        self.t = []
        for line in m:
            self.meas.append([float(i) for i in line[1:-1].strip('[] ').split()])
        for line in g:
            self.guess.append([float(i) for i in line[1:-1].strip('[] ').split()])
        for line in r:
            self.res.append([float(i) for i in line[1:-1].strip('[] ').split()])
        for line in t:
            self.t.append(float(line))
            
        self.meas = np.array(self.meas)
        self.guess = np.array(self.guess)
        self.res = np.array(self.res)

    def draw(self):
        # plt.scatter(self.t, self.meas[:, 0], c='red', label="Measurements", alpha=.5)
        # plt.scatter(self.t, self.guess[:, 0], c='blue', label="Initial Guess", alpha=.5)
        # plt.scatter(self.t, self.res[:, 0], c='green', label="Result Guess", alpha=.5)
        
        fig, axs = plt.subplots(3, sharex=True)
        fig.suptitle(f'Station designations')
        for i in range(3):
            axs[i].scatter(self.t, self.meas[:, i], c='red', label="Measurements", alpha=.5, marker='X')
            axs[i].scatter(self.t, self.guess[:, i], c='blue', label="Initial Guess", alpha=.5, marker='.')
            axs[i].scatter(self.t, self.res[:, i], c='green', label="Result", marker='.')
        # axs[0].plot(t, data['dist'])
        axs[0].set_title("Distance")
        # axs[1].plot(t, data['azimuth'])
        axs[1].set_title("Azimuth")
        # axs[2].plot(t, data['angle'])
        axs[2].set_title("Angle")

        axs[0].set(ylabel='km')
        axs[1].set(ylabel='degrees')
        axs[2].set(ylabel='degrees')

        axs[2].set(xlabel='time, hours')
        axs[0].legend()