from goody import irange
from generator import sequence, transform, count, chunk_sum


for i in sequence('abcd','ef','ghij'):
    print(i,end='')
print()


def upper(x):
    return x.upper()
    
for i in transform('abCdeFg',str.upper):
    print(i,end='')
print()
   
    
for i in count('aBcDEfGhijK',lambda x : 'A'<=x<='Z'): # is upper-case
    print(i,end=' ')
print()


for i in chunk_sum(irange(1,20),5):
    print(i,end=' ')


for i in flatten([1,2,[3,4,(5,6,7,{'abc':1,'xyz':2}),8,9],10]):
    print(i,end=' ')