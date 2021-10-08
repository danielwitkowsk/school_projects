#Tests for TRNG
import math
import scipy.stats
import numpy as np

def ost(random,isPrinting):
    m=100
    mean = 0.5*m;
    std = math.sqrt(12.0);
    x=[0]*m
    y=[0]*m
    for t in range(0,m):
        y[0]+=random[t]
    for t in range(1,m):
        y[t]=y[t-1]-random[t-1]+random[m+t]
        y[t-1] = (y[t-1] - mean)*std;

    y[m-1] = (y[m-1] - mean)*std
    x[0]=y[0]/math.sqrt(1.0*m)
    x[1]= -x[0]*(m-1)/math.sqrt(2.0*m - 1.0) + y[1]*math.sqrt(m/(2.0*m - 1.0));
    x[0]=scipy.stats.norm(scale=1).cdf(x[0])
    x[1]=scipy.stats.norm(scale=1).cdf(x[1])
    for t in range(2,m):
        a = 2.0*m + 1.0 - t;
        b = 2.0*a - 2.0;
        x[t] = y[t-2]/math.sqrt(a*b) - y[t-1]*math.sqrt((a-1.0)/(b+2.0)) + y[t]*math.sqrt(a/b);
        x[t] = scipy.stats.norm(scale=1).cdf(x[t])
    if isPrinting==1:
        print(x)
    sth,p=scipy.stats.kstest(x,'uniform')
    return p

random=[]
file2 = open("nums.txt", "r")
for line in file2:
    random.append(float(line))
j=3
p=[0]*100
for i in range(0,100):
    if i==3:
        p[i]=ost(random[j*20000+i*200:j*20000+(i+1)*200],1)
    else:
        p[i]=ost(random[j*20000+i*200:j*20000+(i+1)*200],0)
sth,ip=scipy.stats.kstest(p,'uniform') #nie udaje sie niestety uzyskać lepszej wartości p-value
print(p,ip)
