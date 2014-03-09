import prompt
import traceback

def make_keep(filters,decorate):
    def show_listing(s):
        i = 1
        for l in s.split('\n'):
            print('{num: >3} {text}'.format(num=i, text = l.rstrip()))
            i += 1
            

    function_template = '''\
def keep(word):
    filter = set({init})
    result = ''
    for c in word:
        if c not in filter:
            result += c
'''  
     
    function_definition = \
      function_template.format(init = '['+','.join(["'"+i+"'" for i in filters])+']')

    if decorate:
        function_definition += "    return '-->'+result+'<--'"
    else:
        function_definition += "    return result"

    # A bit of magic (same in pnamedtuple)
    name_space = dict(__name__='make_keep')
    try:
        exec(function_definition, name_space)
    except SyntaxError:
        show_listing(function_definition)
        traceback.print_exc()
        
    if prompt.for_bool('print definition?'):
        show_listing(function_definition)  # comment out
    return name_space['keep']
 

# Get information, make function, test it
filter_letters = prompt.for_string('Enter disallowed letters')
decorate_it    = prompt.for_bool('Decorate it?')
    
f = make_keep(filter_letters,decorate_it)

word = prompt.for_string('Enter word')
print('returned word =',f(word))


# kargs example
print('Demonstrating kargs')
def f(**kargs):
    # prints dict
    print ('\nkargs =',kargs)
    
    # raises exception
    # equivalent to calling print(a=1,b=2) order of arguments based on dict
    d = dict(a=1,b=2)
    print (**d)

#Call successfully (watch what is printed)
f(a=1,b=True,c=['a','b','c'],d=f)

