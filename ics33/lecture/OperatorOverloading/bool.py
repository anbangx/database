'''
Created on Dec 18, 2013

@author: anbangx
'''

class C:
    def __init__(self,los):
        self.los = los
        
    def __len__(self):
        print('Calling len')
        return sum((len(i) for i in self.los))
      
    def __bool__(self):
        return ''.join(self.los) != 'False'
    
if __name__ == '__main__':
    x = C(['ab','cd'])
    if x:
        print('pass')
    x = C(['Fa', 'l', 'se'])
    
    if x:
        print('pass')