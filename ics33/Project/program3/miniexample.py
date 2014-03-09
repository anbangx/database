import prompt
import traceback

#
def make_keep(filters,decorate):
    # prints string s as if it were a code listing: line number followed by code
    def show_listing(s):
        for i,l in enumerate(s.split('\n'),1):
            print('{num: >3} {text}'.format(num=i, text = l.rstrip()))
            
    # The keep function returns a string will all the characters not in filters
    # 1) make_keep replaces {init} by a set with all the characters in filters
    # 2) It appends a return statement (either normal or decorated)
    # 3) It calls exec to create the code for keep and returns the code
    function_template = '''\
def keep(word):
    filter = set({init})
    result = ''
    for c in word:
        if c not in filter:
            result += c
'''  
     
    function_definition = \
      function_template.format(init = '['+','.join(["'"+i+"'" for i in filter_letters])+']')

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
        
    if prompt.for_bool('print definition?',True):
        show_listing(function_definition)  # comment out
    return name_space['keep']
 

# Get information, make function, test it
filter_letters = prompt.for_string('Enter disallowed letters')
decorate_it    = prompt.for_bool('Decorate it?',True)
    
f = make_keep(filter_letters,decorate_it)

word = prompt.for_string('Enter word')
print('returned word =',f(word))
