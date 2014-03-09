'''
Created on Dec 20, 2013

@author: anbangx
'''

if __name__ == '__main__':
    def f(x):
        return x**2
    print(f(8))
    
    g = lambda x : x**2
    print(g(8))
    
    def make_incrementor (n): return lambda x: x + n
    
    f = make_incrementor(2)
    g = make_incrementor(8)
    
    print(f(42), g(42))
    
    print(make_incrementor(22)(33))