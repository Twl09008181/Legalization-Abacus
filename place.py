import matplotlib.pyplot
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from numpy import floor, trim_zeros, true_divide
from numpy.core.fromnumeric import size





def drawLayout(ax,block,fill,text,color,linewidth):
    #define Matplotlib figure and axis

    max_y = 0
    max_x = 0
    for i in range(len(block)):
        x1,x2,y1,y2 = block[i]
        ax.add_patch(Rectangle((x1,y1),x2-x1,y2-y1,edgecolor=color,color=color,fill=fill,linewidth=linewidth))
        if text:
            ax.text(floor(x1+x2)/2,floor(y1+y2)/2,i,size=5)
        if(x2 > max_x):
            max_x = x2
        if(y2 > max_y):
            max_y = y2





layout = open("Layout","r")
rowNum = int(layout.readline())
termianlNum = int(layout.readline())
nodesNum = int(layout.readline())

Rows = []
terminals = []
nodes = []

layout.readline()
for i in range(rowNum):
    x1,x2,y1,y2 = layout.readline().split()   
    Rows.append([int(x1),int(x2),int(y1),int(y2)])
layout.readline()
for i in range(termianlNum):
    x1,x2,y1,y2 = layout.readline().split()   
    terminals.append([int(x1),int(x2),int(y1),int(y2)])
layout.readline()
for i in range(nodesNum):
    x1,x2,y1,y2 = layout.readline().split()   
    nodes.append([int(x1),int(x2),int(y1),int(y2)])



layout.close()


fig, ax = plt.subplots()
drawLayout(ax,Rows,False,False,"black",1)
drawLayout(ax,terminals,True,False,"red",1)
drawLayout(ax,nodes,False,True,"green",1)


ax.plot()


#display plot
plt.show()

