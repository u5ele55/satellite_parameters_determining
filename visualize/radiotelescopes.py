
import matplotlib.pyplot as plt
import numpy as np

class TelescopesDrawer:
    def __init__(self, telescopeFilename):
        self.fileTelescope = open(telescopeFilename, 'r')
    
    def draw(self, ax):
        dataTelescopes = self.fileTelescope.read().split('\n')
        xr,yr,zr = [], [], []
        N = int(dataTelescopes[0])

        for i in range(1,N+1):
            x,y,z = [float(a) for a in dataTelescopes[i][1:-1].split()]
            xr.append(x)
            yr.append(y)
            zr.append(z)
        
        ax.scatter(xr, yr, zr, c='#FF0000', s=35, marker='o', alpha=1)
