'''
Created on Dec 18, 2013

@author: anbangx
'''
class C:
    def __init__(self,x):
        assert x > 0, 'C.__init__: x('+str(x)+') must be positive'
      
if __name__ == '__main__':
    x = C(-1)