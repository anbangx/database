def sequence(*args):
    for a in args:
        for i in a:
            yield i

def transform(iterable,f):
    for i in iterable:
        yield f(i)

def count(iterable,p):
    answer = 0
    for i in iterable:
        if p(i):
            answer += 1
        yield answer
        
def chunk_sum(iterable,n):
    i = iter(iterable)
    while True:
        sum = 0
        for x in range(n):
            sum += next(i)
        yield sum 