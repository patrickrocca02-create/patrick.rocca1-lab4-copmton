import numpy as np
import matplotlib.pyplot as plt
import scipy.optimize as optimization

# Valori in UA
X_data=np.loadtxt("data.txt",usecols=(7),skiprows=1,unpack=True)
# Valori tabulati
Y=np.array([60.,511.,1275.])

index=[]
str="detector"
if str=="tagger":
    index=[0,1,2]
elif str=="scatterer":
    index=[3,4,5]
elif str=="detector":
    index=[6,7,8]

X=[]
for i in index:
    X.append(X_data[i])
X=np.array(X)

def func(x,a,b):
    return(a+b*x)

x0=np.array([-10,0.5])
a,b=optimization.curve_fit(func,X,Y,x0,None)[0][:]
sa,sb=np.sqrt(np.diag(optimization.curve_fit(func,X,Y,x0,None)[1]))
print("Intercetta: ",a," +/- ",sa)
print("Pendenza: ",b," +/- ",sb)

fig, ax1 = plt.subplots()

x=np.linspace(np.min(X),np.max(X),num=1000)
ax1.plot(X, Y, color="darkorange", marker="s", markersize=4, linestyle="", label="Data")
ax1.plot(x, func(x,a,b), color="cornflowerblue", marker="", linestyle="-", label="Fit lineare")
ax1.set_xlabel("Centroide []",fontsize=16)
ax1.set_ylabel("Energia [keV]",fontsize=16)
ax1.tick_params(axis='both', labelsize=13)
ax1.minorticks_on()
ax1.legend(fontsize=16)
ax1.grid(linewidth=0.4)

fig, ax2 = plt.subplots()

ax2.plot(X, Y-(a+b*X), color="darkorange", marker="s", markersize=4, linestyle="", label="Data")
ax2.axhline(0, color="cornflowerblue", linestyle="--")
ax2.set_xlabel("Centroide []",fontsize=16)
ax2.set_ylabel("Residui [keV]",fontsize=16)
ax2.tick_params(axis='both', labelsize=13)
ax2.minorticks_on()
ax2.legend(fontsize=16)
ax2.grid(linewidth=0.4)

plt.legend(fontsize=16)
plt.grid(linewidth=0.4)
plt.show()