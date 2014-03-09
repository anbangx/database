'''
Created on Dec 18, 2013

@author: anbangx
'''

class C:
    def __init__(self):
        print('C object created')
        
if __name__ == '__main__':
    D = C    # C and D share the same class object
    x = C() # Use C to construct an instance of a class C object
    y = D() # Use D to construct an instance of a class C object
    print(type(x), type(y), type(C), type(type(x)))