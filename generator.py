# Generator based on:
# Xuan Li, Guoji Zhang, Yuliang Liao “Chaos-based true random number generator
# using image”, 2011 International Conference on Computer Science and Service System
# (CSSS), 2011-06, p.2145-2147.
import numpy as np
from PIL import Image
import cv2 
from matplotlib import pyplot as plt 
from scipy.stats import entropy
import math

N=512
M=512
img = cv2.imread('lena1.png',0)  
ret, bw_img = cv2.threshold(img,127,255,cv2.THRESH_BINARY)
tiles = [bw_img[x:x+4,y:y+4] for x in range(0,bw_img.shape[0],4) for y in range(0,bw_img.shape[1],4)]

count = []
for i in range(0,16384):
    count.append(cv2.countNonZero(tiles[i])%2)

lam=4
L=250
x1=[0.361]
x2=[0.362]
x3=[0.363]
x4=[0.364]
x5=[0.365]
x6=[0.366]
x7=[0.367]
x8=[0.368]
x9=[0.369]
for i in range(1,262394):
    x1.append(lam*x1[i-1]*(1-x1[i-1]))
    x2.append(lam*x2[i-1]*(1-x2[i-1]))
    x3.append(lam*x3[i-1]*(1-x3[i-1]))
    x4.append(lam*x4[i-1]*(1-x4[i-1]))
    x5.append(lam*x5[i-1]*(1-x5[i-1]))
    x6.append(lam*x6[i-1]*(1-x6[i-1]))
    x7.append(lam*x7[i-1]*(1-x7[i-1]))
    x8.append(lam*x8[i-1]*(1-x8[i-1]))
    x9.append(lam*x9[i-1]*(1-x9[i-1]))
x1=x1[L:]
x2=x2[L:]
x3=x3[L:]
x4=x4[L:]
x5=x5[L:]
x6=x6[L:]
x7=x7[L:]
x8=x8[L:]
x9=x9[L:]
indexes = np.argsort(x1)
newimg = np.zeros((512,512, 3), dtype=np.uint8)
for i in range(0,M-1):
    for j in range(0,N-1):
        newimg[i][j]=img[indexes[i*N+j]%N][indexes[i*N+j]%N]
c=0.5
for i in range(0,262144):
    if x2[i]<c:
        x2[i]=0
    else:
        x2[i]=1
    if x3[i]<c:
        x3[i]=0
    else:
        x3[i]=1
    if x4[i]<c:
        x4[i]=0
    else:
        x4[i]=1
    if x5[i]<c:
        x5[i]=0
    else:
        x5[i]=1
    if x6[i]<c:
        x6[i]=0
    else:
        x6[i]=1
    if x7[i]<c:
        x7[i]=0
    else:
        x7[i]=1
    if x8[i]<c:
        x8[i]=0
    else:
        x8[i]=1
    if x9[i]<c:
        x9[i]=0
    else:
        x9[i]=1
newimg = newimg[:,:,2]
plane1 =[]
plane2 =[]
plane3 =[]
plane4 =[]
plane5 =[]
plane6 =[]
plane7 =[]
plane8 =[]
for i in range(0,M-1):
    for j in range(0,N-1):
        plane1.append(int(np.binary_repr(newimg[i][j], width=8)[0]))
        plane2.append(int(np.binary_repr(newimg[i][j], width=8)[1]))
        plane3.append(int(np.binary_repr(newimg[i][j], width=8)[2]))
        plane4.append(int(np.binary_repr(newimg[i][j], width=8)[3]))
        plane5.append(int(np.binary_repr(newimg[i][j], width=8)[4]))
        plane6.append(int(np.binary_repr(newimg[i][j], width=8)[5]))
        plane7.append(int(np.binary_repr(newimg[i][j], width=8)[6]))
        plane8.append(int(np.binary_repr(newimg[i][j], width=8)[7]))
result1=[]
for i in range(len(plane1)-1):
    result1.append((plane1[i]+x2[i])%2)
   
result=np.array_split(np.array(result1),32640)

for i in range(len(result)):
    out=0
    for bit in result[i]:
        out = (out << 1) | bit
    result[i]=out

lol=img.ravel()
values = np.unique(lol)
valsnum =[0]*256
valls=[0]*256
print(len(lol))
print(len(values))
for j in range(len(values)):
    for i in range(len(lol)):
        if lol[i]==values[j]:
            valsnum[j]+=1
            valls[lol[i]]+=1
for i in range(len(valls)):
    valls[i]/=len(lol)
ent = 0.0
values = values.astype(float)
for i in range(len(values)):
    values[i] = valsnum[i]/len(lol)
for i in range(len(values)):
    ent += values[i]*np.log2(values[i])
ent = -ent
print('Entropia: ',ent)
res=np.unique(result)
valsnum =[0]*256
for j in range(len(res)):
    for i in range(len(result)):
        if result[i]==res[j]:
            valsnum[j]+=1
ent = 0.0
res = res.astype(float)
for i in range(len(res)):
    res[i] = valsnum[i]/len(result)
for i in range(len(res)):
    if res[i]>0.0:
        ent += res[i]*np.log2(res[i])
ent = -ent
print('Entropia: ',ent)
#plt.bar(range(0,256), valls)

plt.bar(range(0,256), res)
#plt.hist(result,256) 
plt.xlabel('Wartość $(x_{i})$')
plt.ylabel('Częstość występowania $(p_{i})$')
plt.title("Empiryczny rozkład zmiennych losowych po post-processingu") 
plt.show()