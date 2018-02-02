
# coding: utf-8

#Ukol 1
# from pylab import*
from scipy.io import wavfile
# Nacteni souboru
sampFreq, snd = wavfile.read('xendry02.wav')

# Vzorkovaci frekvence
sampFreq

# Delka ve vzorcich
len(snd)

# Delka v sekundach
len(snd)/ sampFreq

#Ukol 2
from matplotlib.pyplot import *
from numpy import *
import matplotlib.pyplot as plt
interval = int(len(snd)/2)
# Vytvoreni casove osy 
timeArray = arange(interval)
# Diskretni furierova transformace na signal
dft = fft.fft(snd[:])
#Nastaveni nazvu os
plt.xlabel("Frekvence [Hz]")
plt.ylabel("Modulo spektra")
#Nastaveni grafu
plot(timeArray, abs(dft[:interval]), color='k')
show()
dft[:interval]

# Ukol 3
# Maximum
i = 0
maxim = None
maximIndex = -1
for n in dft:
    if(maxim == None or maxim < abs(dft[i])):
        maxim = abs(dft[i])
        maximIndex = i
    i += 1;
print(maximIndex)
print(maxim)

# Ukol 4
from scipy.signal import tf2zpk
import matplotlib.patches as mpatches

#Koeficienty filtru
filterB = [0.2324,-0.4112,0.2324]
filterA = [1,0.2289,0.4662]
# Ziskani nul, polu
z,p,k = tf2zpk(filterB,filterA)

plot(real(z),imag(z),"bo")
plot(real(p),imag(p),"ro")
#Zobrazeni jednotkove kruznice
circle = plt.Circle((0, 0), radius=1,fill=False)
#legenda grafu
red_patch = mpatches.Patch(color='blue', label='Nuly')
blue_patch = mpatches.Patch(color='red', label='Póly')
plt.xlabel("Reálná osa")
plt.ylabel("Imaginární osa")
plt.legend(handles=[blue_patch,red_patch])
plt.gca().add_patch(circle)
plt.axis('scaled')
show()

#Ukol 5
from scipy.signal import freqz
w, h = freqz(filterB,filterA,8000)
plt.plot(timeArray, abs(h), 'b')
plt.xlabel("Frekvence [Hz]")
plt.ylabel("Modul kmitočtové charakteristiky")
show()
# Horni propust

#Ukol 6
from scipy.signal import lfilter
#filtrace
filtered1 = lfilter(filterB,filterA, snd)

#Generovani casove osy
timeArrayFull = arange(len(snd))
#Diskretni fourierova transformace
filterDft = fft.fft(filtered1)
plot(timeArray,abs(filterDft[:8000]))
plt.xlabel("Frekvence [Hz]")
plt.ylabel("Modulo spektra")
show()

# Ukol 7
# Maximum
j = 0
maxim1 = None
maximIndex1 = -1
for n in filterDft:
    if(maxim1 == None or maxim1 < abs(filterDft[j])):
        maxim1 = filterDft[j]
        maximIndex1 = j
    j += 1;
    if(j >= 8000):
        break
print(maximIndex1)
print(maxim1)

#Ukol 9
autocorel = []
for x in range(0, len(snd)):
    sum = 0
    n = 0
    for y in range(-50,51):
        if(x + y >= 0 and x + y < len(snd)):
            sum += snd[x] * snd[x + y]
            n += 1
    autocorel.append(sum/n)

plot(timeArrayFull, autocorel,color="k")
plt.xlabel("Frekvence [Hz]")
plt.ylabel("Autokorelační koeficienty")
show()

#Ukol 10
autocorel[10]

