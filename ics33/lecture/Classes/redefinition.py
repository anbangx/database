'''
Created on Dec 18, 2013

@author: anbangx
'''

if __name__ == '__main__':
    def f(): return 0
    def g(): return 1
    f = g
    print(f())