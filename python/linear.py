import struct
import torch
import torch.nn as nn

def binToPt():
    path = "../data/Features.bin"
    file = open(path, 'rb')
    content = file.read()

    sz, realsz = struct.unpack('ii', content[:8])
    assert sz % 50 == 0
    print(sz, realsz)
    data = torch.empty((sz, 4))
    labels = torch.empty((sz, 1))

    P = 4*realsz+2*4
    OFF = 8
    for i in range(0, sz, 1):
        x = struct.unpack('4dii', content[OFF + i*P:OFF + (i+1)*P])
        data[i] = torch.Tensor(x[:4])
        labels[i, 0] = x[4]
        #empty squares not used
    torch.save(data, 'data.pt')
    torch.save(labels, 'labels.pt')

def loadFromPt():
    dpath = 'data_shuffled.pt'
    lpath = 'labels_shuffled.pt'
    return torch.load(dpath), torch.load(lpath)

def saveData(d, l):
    dpath = 'data_shuffled.pt'
    lpath = 'labels_shuffled.pt'
    torch.save(d, dpath)
    torch.save(l, lpath)

def shuffleData(d, l):
    r = torch.randperm(d.shape[0])
    return d[r], l[r]


data, labels = loadFromPt()
#sd, sl = shuffleData(data, labels)
#saveData(sd, sl)


#####################
# Optimization
BATCH_SZ = 50
NBATCHES = data.shape[0] // BATCH_SZ

IN_SZ, OUT_SZ = 4, 1
#linear combination of features : 
#net = nn.Sequential(nn.Linear(IN_SZ, OUT_SZ))

#small nn
net = nn.Sequential(nn.Linear(IN_SZ, 6), nn.ReLU(), nn.Linear(6, OUT_SZ))

criterion = nn.MSELoss()
optimizer = torch.optim.SGD(net.parameters(), lr=1e-9)
#test with all ones
data = data.view((NBATCHES, BATCH_SZ, -1))
labels = labels.view((NBATCHES, BATCH_SZ, -1))

data = data.view((NBATCHES, BATCH_SZ, -1))
labels = labels.view((NBATCHES, BATCH_SZ, -1))

for ep in range(20):
    for bi in range(data.shape[0]):
        y = net(data[bi])
        loss = criterion(y, labels[bi])
        loss.backward()
        optimizer.step()
        
        if bi % 50 == 0:
            print(f'Loss : {loss.item()}')
            if bi % 500 == 0:
                print(f'Weights : {net[0].weight}, Bias : {net[0].bias}')
                if bi % 50000 == 0:
                    print("Example :")
                    for i in range(BATCH_SZ):
                        print(f'Data {data[bi, i]} Y {y[i]} T {labels[bi, i]}')
                    input("Continue ? ")

#code to try with a simple sum of all features
'''w = torch.zeros_like(net[0].weight)+1
for bi in range(data.shape[0]):
    y = data[bi] * w
    loss = criterion(y, labels[bi])
    print(loss)
'''