# Ronald Yu, :ab 1
# Roger Ding , Lab 1
# We certify that we worked cooperatively on this programming
#   assignment, according to the rules for pair programming

def read_fa(fa_file):
    '''Takes an fa_file and creates a dictionary representing the finite automaton'''
    fa_dict = dict()
    for line in fa_file:
        split_line_list = line.strip().split(';')
        for counter in range(1,len(split_line_list),2):
            fa_dict.setdefault(split_line_list[0],dict()).update({split_line_list[counter]:split_line_list[counter+1]})
    fa_file.close()
    return fa_dict
    
def print_fa(fa_dict):
    '''Prints an FA dictionary'''
    print('Finite Automaton Description')
    print(fa_dict)
    for key in fa_dict:
        print( '{} transitions: {}'.format(key,list(fa_dict[key].items())))    

def process(fa_dict, start_state, input_list):     
    '''Returns a list containing the start state and tuples that show the input and
    resulting state after each transition'''
    result=[start_state]
    state=start_state
    for i in input_list:
        #is_i_in_fa_dict checks if we have been given a legal input
        is_i_in_fa_dict = False
        for value in fa_dict[state]:
            if value == i:
                state = fa_dict[state][value]
                result.append((i,state))
                is_i_in_fa_dict = True
        if not is_i_in_fa_dict:
            result.append((i,None))
            return result
    return result 

def interpret(processed_list):      
    '''Prints out the result of processing an fa on an input'''
    print('Starting new simulation')
    print('Start state = {}'.format(processed_list[0]))
    for i in processed_list[1:]:
        print('\tInput = {}; new state = {}'.format(i[0],i[1] if i[1] != None else 'input error; terminated'))
    print('Stop State = {}'.format(processed_list[-1][-1]))
if __name__ == '__main__':
    
    while True:
        try:
            fa_file_name = input('Please enter the name of the FA file:')
            fa_dict = read_fa(open(fa_file_name,'r'))
        except:
            print('Please enter a valid FA file')
        else:
            print_fa(fa_dict)
            break
    while True:
        try:
            fa_input_file_name = input('Enter file with start-state and input:')
            fa_input_file = open(fa_input_file_name,'r')
            for line in fa_input_file:
                interpret(process(fa_dict, line.strip().split(';')[0], line.strip().split(';')[1:]))
            fa_input_file.close()
        except:
            print('Please enter a valid file')
        else:
            break