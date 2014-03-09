def transform(iterable,f):
    for i in iterable:
        yield f(i)

def running_count(iterable,p):
    answer = 0
    for i in iterable:
        if p(i):
            answer += 1
        yield answer
        
def n_with_pad(iterable,n,pad=None):
    it = iter(iterable)
    for i in range(n):
        try:
            yield(next(it))
        except StopIteration:
            yield pad
        
def sequence(*args):
    for a in args:
        for i in a:
            yield i

def alternate(*args):
    args = [iter(a) for a in args]
    while args:
        left = []
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
    import driver
    driver.driver()
