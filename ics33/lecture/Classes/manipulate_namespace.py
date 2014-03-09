'''
Created on Dec 18, 2013

@author: anbangx
'''

class C:
    def __init__(self):
        print('C object created')
        
if __name__ == '__main__':
    x = C()
    print(x.__dict__)
    x.x = 1
    x.y = 'two'
    x.__dict__['z'] = 3
    print(x.__dict__)
    
    for(k,v) in C.__dict__.items():
        print(k,'->',v)