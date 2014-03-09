def transform(iterable,f):
    for i in iterable:
        yield f(i)

def running_count(iterable,p):
    count = 0
    for i in iterable:
        if p(i):
            count += 1
        yield count
        
def n_with_pad(iterable,n,pad=None):
    it = iter(iterable) 
    count = 0
    while count < n:
        count += 1
        try:
            yield next(it)
        except StopIteration:
            yield pad
        
        
def sequence(*args):
    for arg in args:
        for i in arg:
            yield i

def alternate(*args):
    args = [iter(a) for a in args]
    c = 0
    while args:
        left = []
        print(len(args))
        for i in range(len(args)):
            try:
                yield next(args[i])
                left.append(args[i])
            except StopIteration:
                pass
        args = left
                
def flatten(iterable):
    for i in iterable:
        if type(i) is str:
            yield i
        else:
            try:
                for x in flatten(i):
                    yield x
            except:
                yield i

                
                
if __name__ == '__main__':
#     import driver
#     driver.driver()
    
    a = ' '.join([str(i) for i in transform('abCdeFg',str.upper)]) #-->A B C D E F G
    print(str(a))
    
    b = ' '.join([str(i) for i in running_count('bananastand',lambda x : x in 'aeiou')]) #-->0 1 1 2 2 3 3 3 4 4 4
    print(b)
    
    c = ' '.join([str(i) for i in n_with_pad('abcdefg',3)]) #-->a b c
    print(c)
    c = ' '.join([str(i) for i in n_with_pad('abcdefg',10,'?')]) #-->a b c d e f g ? ? ?
    print(c)
    c = ' '.join([str(i) for i in n_with_pad('abcdefg',10)]) #-->a b c d e f g None None None
    print(c)

    d = ' '.join([str(i) for i in sequence('abcde','fg','hijk')]) #-->a b c d e f g h i j k
    print(d)
    
    e = ' '.join([str(i) for i in alternate('abcde','fg','hijk')]) #-->a f h b g i c j d k e
    print('alternate: ' + e)
    
#     for i in flatten([1,2,[3,4,(5,6,7,{'abc':1,'xyz':2}),8,9],10]):
#         print(i,end=' ')