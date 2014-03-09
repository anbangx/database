'''
Created on Dec 19, 2013

@author: anbangx
'''
from collections import defaultdict

from goody import type_as_str

class Bag:
    def __init__(self, values=[]):
        self.counts = defaultdict(int)
        for v in values:
            self.counts[v] += 1
    
    def __str__(self):
        return 'Bag(' + ",".join(str(k) + '[' + str(v) + ']' for k,v in self.counts.items()) + ')'
    
    def __len__(self):
        return sum(self.counts.values())
    
    def unique(self):
        return len(self.counts.keys())
    
    @staticmethod
    def __contains__(self, v):
        return v in self.counts
    
    def count(self, v):
        return self.counts[v] if v in self.counts else 0
        
    def add(self, v):
        self.counts[v] += 1
        
    def remove(self, v):
        if v in self.counts:
            self.counts[v] = self.counts[v] - 1;
            if self.counts[v] == 0:
                del self.counts[v]
        else: 
            raise ValueError('Bag.remove fails, you can not remove ' + str(v) + '.')
    
    def _validate_bags(self,right,op):
        if type(right) is not Bag:
            raise TypeError('unsupported operand type(s) for '+op+\
                            ': \''+type_as_str(self)+'\' and \''+type_as_str(right)+'\'') 
    
    def _same(self,right):
        if len(self) != len(right):
            return False
        else:
            for i in self.counts:
                # check not it to avoid creating count of 0 via defaultdict
                if i not in right or self.counts[i] != right.counts[i]:
                    return False
            return True
                
    def __eq__(self, right):
        self._validate_bags(self, right, '==')
        return self._same(right)
            
    def __ne__(self, right):
         self._validate_bags(self, right, '!=')
         return not self._same(right)
     
    @staticmethod
    def _gen(x):
        for k,v in x.items():
            for i in range(v):
                yield k  
                
    def __iter__(self):
        return Bag._gen(self.counts)
    
if __name__ == '__main__':
#     import driver
#     driver.driver()   
    x = Bag(['d','a','b','d','c','b','d'])
    print(str(x))
    print(x.unique())
    x.add('d')
    print(x)
    
    # Test iterator
    a = [i for i in sorted(x)] #-->['a', 'b', 'b', 'c', 'd', 'd', 'd']
    print(str(a))
    i = iter(x)
    x.add('d')
    x.remove('a')
    b = [i for i in sorted(x)] #-->['b', 'b', 'c', 'd', 'd', 'd', 'd']
    print(str(b))