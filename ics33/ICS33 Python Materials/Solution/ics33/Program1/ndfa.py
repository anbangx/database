# Ronald Yu, :ab 1
# Roger Ding , Lab 1
# We certify that we worked cooperatively on this programming
#   assignment, according to the rules for pair programming

def read_ndfa(ndfa_file):
    '''Takes an fa_file and creates a dictionary representing the finite automaton'''
    ndfa_dict = dict()
    for line in ndfa_file:
        split_line_list = line.strip().split(';')
        ndfa_dict[split_line_list[0]]=dict()
        for counter in range(1,len(split_line_list),2):
            if split_line_list[counter] not in ndfa_dict[split_line_list[0]]:
                ndfa_dict[split_line_list[0]][split_line_list[counter]]={split_line_list[counter+1]}
            else:
                ndfa_dict[split_line_list[0]][split_line_list[counter]].add(split_line_list[counter+1])
    ndfa_file.close()
    return ndfa_dict

def print_ndfa(ndfa_dict):
    '''Takes an fa_dict and prints out the dictionary'''
    print('Non-deterministic Finite Automaton')
    for key in ndfa_dict:
        print( '{} transitions: {}'.format(key,list(ndfa_dict[key].items())))
        
def process(ndfa_dict, start_state, input_list):     
    '''Returns a list containing the start state and tuples that show the input and
    resulting state after each transition'''
    result = [start_state]
    possible_states = {start_state}
    temp_states=set()
    for i in input_list:
        #is_i_in_fa_dict checks if we have been given a legal input
        is_i_in_ndfa_dict = False
        for state in possible_states:
            if i in ndfa_dict[state]:
                temp_states.update(ndfa_dict[state][i])
                is_i_in_ndfa_dict = True
        if not is_i_in_ndfa_dict:
            result.append(i,None)
            return result
        result.append((i,set(temp_states)))
        possible_states.clear()
        possible_states.update(temp_states)
        temp_states.clear()
    return result

def interpret(processed_list):      
    '''Prints out the result of processing an ndfa on an input'''
    print('Starting new simulation:')
    print('Start state = {}'.format(processed_list[0]))
    for i in processed_list[1:]:
        print('\tInput = {}; new states = {}'.format(i[0],i[1] if i[1] != None else 'input error; terminated'))
    print('Stop State(s) = {}'.format(processed_list[-1][-1]))
        
if __name__ == '__main__':
    
    while True:
        try:
            ndfa_file_name = input('Please enter the name of the FA file:')
            ndfa_dict = read_ndfa(open(ndfa_file_name,'r'))
            print_ndfa(ndfa_dict)
        except:
            print('Please enter a valid FA file')
        else:
            break
    while True:
        try:
            ndfa_input_file_name = input('Enter file with start-state and input:')
            ndfa_input_file = open(ndfa_input_file_name,'r')
            for line in ndfa_input_file:
                print()
                interpret(process(ndfa_dict, line.strip().split(';')[0], line.strip().split(';')[1:]))
            ndfa_input_file.close()
        except:
            print('Please enter a valid file')
        else:
            break