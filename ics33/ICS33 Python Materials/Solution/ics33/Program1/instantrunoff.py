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
    '''Prints a dictionary based on the function given'''
    items_to_print_list = sorted(dictionary.items(), key = function, reverse = b) 
    print(title)
    for i in items_to_print_list:
        print('  {} -> {}'.format(i[0], i[1]))
        
        
def evaluate_ballot(vote_dict:dict, remaining_candidates:set):
    '''Evaluates the ballots and returns a dictionary containing the results of the ballots'''
    result_dict={candidate : 0 for candidate in remaining_candidates}
    for voter in vote_dict:
        ballot_number=0
        while True:
            if vote_dict[voter][ballot_number] in remaining_candidates:
                result_dict[vote_dict[voter][ballot_number]]+=1
                break
            else:
                ballot_number+=1
    return result_dict
            
        
def remaining_candidates(candidate_votes_dict):
    '''Takes dictionary of the the votes that each candidate received and returns
    the remaining candidates'''
    least_votes = min(candidate_votes_dict.values())
    remaining_candidates = set(candidate for candidate in candidate_votes_dict if candidate_votes_dict[candidate]!=least_votes)
    return remaining_candidates


if __name__ == '__main__':
    while True:
        voter_file_name = input('Enter file with voter preferences:')
        #Try to make a dictionary out of the voter file
        try:
            vote_dict = read_voter_preferences(open(voter_file_name))
        #If file name crashes the graph_dict() function
        except:
            print('Not a valid file. Try again')
        else:
            print_dict('Voter Preferences', vote_dict, lambda preference: preference[1])
            break
    #candidates_left is initally a set of all the candidates on an arbitrary voter preference
    candidates_left = set(candidate for all_candidates in vote_dict.values() for candidate in all_candidates)
    ballot_number=1
    while True:
        print_dict( 'Vote count on ballot #{} with candidates (alphabetically) = {}:'.format(ballot_number,candidates_left),
                    evaluate_ballot(vote_dict, candidates_left),
                    lambda preference: preference[0])
        print_dict( 'Vote count on ballot #{} with candidates (numerically) = {}:'.format(ballot_number,candidates_left),
                    evaluate_ballot(vote_dict, candidates_left),
                    lambda preference: preference[1], True)
        candidates_left = remaining_candidates(evaluate_ballot(vote_dict, candidates_left))
        ballot_number+=1
        if len(candidates_left) == 1:
            print('The winner is: {} '.format(candidates_left))
            break
        elif len(candidates_left) == 0:
            print(' No winner: election is a tie among candidate remaining on the last ballot')
            break
    
