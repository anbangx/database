'''
Created on Dec 18, 2013

@author: anbangx
'''

class C:
    def __init__(self,los):
        self.los = los
        
    def __lt__(self,right):
        return self.los < right.los
    
    # test if it exists __eq__
    def __eq__(self,right):
        return self.los == right.los
if __name__ == '__main__':
    x = C(['1', '5'])
    y = C(['5', '1'])
    print(x < y)
    print(y > x)
    
    x=C(['1'])
    y=C(['1'])
    z=x
    
    print(x == y, x is y, x == z, x is z)
    print(x != y, x is not y, x != z, x is not z)
