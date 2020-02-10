'''

'''

f = range(-20, 20)
t = list(map(lambda x: 5*x-3, f))

w = 0.2
b = 0
eta = 0.01

for ite in range(100):
    for x, z in zip(f, t):
        y = x * w + b
        l = 0.5*(y-z)**2
        dldy = y-z
        dldw = dldy * x
        w -= eta * dldw
        b -= eta * dldy
        print(f'I {ite}, loss {l}, w {w}, b {b}.')
        

