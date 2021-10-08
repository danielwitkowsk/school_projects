#Tests for TRNG
import math
import numpy as np
from scipy.spatial.distance import pdist, squareform
from scipy.stats import expon
import matplotlib.pyplot as plt
import scipy.stats

def phi(x):
    return (1.0 + math.erf(x / math.sqrt(2.0))) / 2.0

def cars(random):
    sth, p = scipy.stats.kstest(arr,'uniform')
    print(p)
    lenn=len(random)
    rezs=[]
    for k in range(10,20):
        print(k)
        vals=[]
        for j in range (0,100):
            setpointsx = []
            setpointsy = []
            parkedcars = []
            for i in range (0,12000):
                setpointsx.append(random[(k*2400000+ j*24000+i*2)%lenn] * 100)
                setpointsy.append(random[(k*2400000+ j*24000+i*2+1)%lenn] * 100)
            for i in range (0,len(setpointsx)):
                if issafe(setpointsx[i],setpointsy[i],parkedcars)==1:
                    parkedcars.append(setpointsx[i])
                    parkedcars.append(setpointsy[i])
            vals.append((((len(parkedcars)/2)-3523.0)/21.9))
            print(j)
        maxx=max(vals)
        minn=min(vals)
        for i in range(0,len(vals)):
            vals[i]=(vals[i]-minn)/(maxx-minn)
        #arr, xx = np.histogram(vals)
        mu = np.mean(vals)
        sigma = np.std(vals)
        sth, p = scipy.stats.kstest(vals,'norm',args=(mu,sigma))
        rezs.append(p)
        print(p)
    arr, xx = np.histogram(rezs)
    mu = np.mean(arr)
    sigma = np.std(arr)
    sth, p = scipy.stats.kstest(arr,'norm',args=(mu,sigma))
    print(arr,xx,p)
           
def issafe(x,y,parkedcars):
    for i in range(0,int(len(parkedcars)/2)):
        if max(abs(x-parkedcars[i*2]),abs(y-parkedcars[i*2+1])) < 1:
            return 0
    return 1
            
        
    
def countones(random):
    propability={
    'A':37/256,
    'B':56/256,
    'C':70/256,
    'D':56/256,
    'E':37/256
    }
    letters=[]
    for i in range(0,math.floor(len(random)/8)):
        count=0
        if random[i*8]==1:
            count+=1
        if random[i*8+1]==1:
            count+=1
        if random[i*8+2]==1:
            count+=1
        if random[i*8+3]==1:
            count+=1
        if random[i*8+4]==1:
            count+=1
        if random[i*8+5]==1:
            count+=1
        if random[i*8+6]==1:
            count+=1
        if random[i*8+7]==1:
            count+=1
        if count<3:
            letters.append("A")
        elif count==3:
            letters.append("B")
        elif count==4:
            letters.append("C")
        elif count==5:
            letters.append("D")
        elif count>5:
            letters.append("E")
    rezs=[]
    for j in range (0,100):
        omit=j*256000
        wordsTypesFive={'AAAAA':0}
        Q5=0
        fexp=[]
        for i in range(0,256000):
            word=''
            word+=letters[omit+i]
            word+=letters[omit+i+1]
            word+=letters[omit+i+2]
            word+=letters[omit+i+3]
            word+=letters[omit+i+4]
            if(word in wordsTypesFive.keys()):
                    wordsTypesFive[word]+=1
            elif(word not in wordsTypesFive.keys()):
                    wordsTypesFive[word]=1
        
        for v,k in wordsTypesFive.items():
            mean = np.prod([propability[i] for i in v ]) * 256000
            fexp.append(mean)
            observ = k
            Q5 += ((k-mean)**2)/mean
        t=[]
        for i,j in wordsTypesFive.items():
            t.append(j)
        chisq5, p5 = scipy.stats.chisquare( t ,f_exp = fexp)

        wordsTypesFour={'AAAA':0}
        Q4=0
        fexp = []
        for i in range(0,256000):
            word=''
            word+=letters[omit+i]
            word+=letters[omit+i+1]
            word+=letters[omit+i+2]
            word+=letters[omit+i+3]
            if(word in wordsTypesFour.keys()):
                    wordsTypesFour[word]+=1
            elif(word not in wordsTypesFour.keys()):
                    wordsTypesFour[word]=1
        for v,k in wordsTypesFour.items():
            mean = np.prod([propability[i] for i in v ]) * 256000
            fexp.append(mean)
            Q4 += ((k-mean)**2)/mean
        t=[]
        for i,j in wordsTypesFour.items():
            t.append(j)
        chisq4, p4 = scipy.stats.chisquare( t ,f_exp = fexp)
        rezs.append(chisq5-chisq4)
    print(rezs)
    z=[]
    phiz=[]
    for i in range(0,len(rezs)):
        z.append((rezs[i]-2500)/math.sqrt(5000))
        phiz.append((1-phi(z[i])))
    print(phiz)
    sth, p = scipy.stats.kstest(phiz,'uniform')
    arr, xx = np.histogram(phiz)
    print(p,arr,xx)

random = []
file1 = open("sample.txt", "r")#otwieranie bitów
for line in file1:
    for character in line:
        random.append(int(character))
#file2 = open("nums.txt", "r") #otwieranie liczb, każda z 32 bitów
#for line in file2:
#    random.append(float(line))
countones(random)
#cars(random)

