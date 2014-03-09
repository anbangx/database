'''
Created on Dec 20, 2013

@author: anbangx
'''

if __name__ == '__main__':
    print( list(map(lambda x,y: x*y, 'abcde', range(1,5))) )
    
    y = map(lambda x : x**2, [i for i in range(0,5)])
    print(y)
    
    print([i for i in y])
    print(list(y))