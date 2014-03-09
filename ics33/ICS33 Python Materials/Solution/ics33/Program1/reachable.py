# Ronald Yu, :ab 1
# Roger Ding , Lab 1
# We certify that we worked cooperatively on this programming
#   assignment, according to the rules for pair programming


def read_graph(file_path):
    '''Takes a file path of a graph and returns a dictionary where the keys are each starting
     node and the values are the destinations corresponding to each starting node'''
    graph_dict = dict()
    graph_file = open(file_path, 'r')
    for line in graph_file:
        starting_node = line[0]
        destination = line[2]
        if starting_node in graph_dict:
            graph_dict[starting_node].update({destination})
        elif starting_node not in graph_dict:
            graph_dict[starting_node] = ({destination})
    graph_file.close()
    return graph_dict
        
def print_graph(graph_dict):
    '''Prints a graph when given a dictionary representing the graph'''
    graph_list = sorted(graph_dict.items())
    print('Graph: source -> {destination} edges')
    for i in graph_list:
        print('  {} -> {}'.format(i[0],i[1]))
    
    
def reachable(starter, graph_dict):
    '''Returns a set of all possible nodes that can be reached from the starter node in
    the graph'''
    reached_nodes = set()
    exploring_list = [starter]
    while exploring_list != []:
        temp_node = exploring_list.pop(0)
        reached_nodes.update({temp_node})
        if temp_node not in graph_dict:
            continue
        for reachable_node in graph_dict[temp_node]:
            exploring_list.append(reachable_node)
    return reached_nodes
        
    
    

if __name__ == '__main__':
    while True:
        file_name = input( 'Enter the file of the graph:')
        #Try to make a dictionary out of the graph file
        try:
            graph_dict = read_graph(file_name)
        except:
            print('Not given a valid file name. Please try again')
        else:
            print_graph(graph_dict)
            break
    while True:
        starter = input('Enter starting node:')
        if starter == 'quit':
            break
        if starter not in graph_dict:
            print('''Entry Error: '{}';  Not a source node
    Please enter a legal String'''.format(starter))
            continue
        reached_nodes = reachable(starter,graph_dict)
        print('From', starter, 'the reachable nodes are', reached_nodes)