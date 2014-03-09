# Ronald Yu, :ab 1
# Roger Ding , Lab 1
# We certify that we worked cooperatively on this programming
#   assignment, according to the rules for pair programming


def read_voter_preferences(voter_file):
    '''Takes the file and returns a dictionary where the keys are the voters and the values
    are lists of the voters' preferred candidates'''
    voter_dict = dict()
    for line in voter_file:
        voter = line.split(';')[0]
        votes=line.strip().split(';')[1:]
        voter_dict[voter] = votes
    voter_file.close()
    return voter_dict

def print_dict(title, dictionary, function = None, b=False):
    '''Prints a dictionary containing the votes of each voter'''
    items_to_print_list = sorted(dictionary.items())
    print(title)
    for i in items_to_print_list:
        print('  {} -> {}'.format(i[0], i[1]))
        
        
def evaluate_ballot(vote_dict:dict, remaining_candidates:set):
    '''Evaluates the ballots and returns a dictionary containing the results of the ballots'''
    result_dict=dict()
    ballot_number=0
    for candidate in remaining_candidates:
        for voter in vote_dict:
            if vote_dict[voter][ballot_number] not in result_dict:
                result_dict[vote_dict[candidate][ballot_number]] = 1
            else:
                result_dict[vote_dict[candidate][ballot_number]]+=1

    return result_dict
            
        
def remaining_candidates(candidate_votes_dict):
    '''Takes dictionary of the the votes that each candidate received and returns
    the remaining candidates'''
    remaining_candidates=set()
    list_of_votes = sorted(candidate_votes_dict.values())
    if min(list_of_votes)==max(list_of_votes):
        return {}
    else:
        for i in candidate_votes_dict:
            if max(list_of_votes) == candidate_votes_dict[i]:
                remaining_candidates.update({candidate_votes_dict[i]})
    return remaining_candidates


if __name__ == '__main__':
    while True:
        voter_file_name = input('Enter file with voter preferences:')
        #Try to make a dictionary out of the voter file
        try:
            vote_dict = read_voter_preferences(voter_file_name)
        #If file name crashes the graph_dict() function for some other reason
        except:
            print('Not a valid file. Try again')
        else:
            print_dict('Voter Preferences', vote_dict)
            break
    '''remaining_candidates={}
    while True:
        ballot_result = evaluate_ballot(vote_dict,remaining_candidates)
        remaining_candidates = remaining_candidates()'''