'''
Created on Dec 18, 2013

@author: anbangx
'''

if __name__ == '__main__':
    print(zip((1,2,3),(10,20,30),(100,200,300)))
    
    [sum(x) for x in zip((1,2,3),(10,20,30),(100,200,300))]