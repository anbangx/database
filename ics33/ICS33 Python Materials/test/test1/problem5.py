# Ronald Yu, :ab 1
# Roger Ding , Lab 1
# We certify that we worked cooperatively on this programming
#   assignment, according to the rules for pair programming
import goody
import random

def read_corpus(order_stat, infile):
    corpus=dict()
    word_list = list(goody.read_file_values(infile))
    start = 0
    while (start+order_stat) < len(word_list):
        word_tuple = tuple(word_list[start:start+order_stat])
        corpus.setdefault(word_tuple,{word_list[start+order_stat]}).add(word_list[start+order_stat])
        start+=1
    #Convert the set to a list
    for i in corpus:
        corpus[i]=list(corpus[i])
    infile.close()
    return corpus

def print_corpus(corpus):
    print("Corpus")
    min,max=1,1
    for i in corpus:
        print('\t',i, 'can be followed by any of', corpus[i])
        if len(corpus[i]) < min:
            min = len(corpus[i])
        if len(corpus[i]) > max:
            max = len(corpus[i])        
    print('min/max={}/{}'.format(min, max))
    
def produce_text(corpus, word_list, num_to_gen):
    order_stat = len(word_list)
    for i in range(num_to_gen):
        if tuple(word_list[i:order_stat+i]) not in corpus:
            word_list.append(None)
            return word_list
        value_list = corpus[tuple(word_list[i:order_stat+i])]
        word_list.append(value_list[random.randint(0,len(value_list)-1)])
    return word_list
if __name__ == '__main__':
    while True:
        try:
            order_stat = eval(input('Enter Order Statistic:'))
            infile = input('Enter file to process:')
            corpus=read_corpus(order_stat,open(infile))
            print_corpus(corpus)
        except:
            print('Please enter valid parameters')
        else:
            break
    while True:
        try:
            print("Enter {} words to start with".format(order_stat))
            word_list=[]
            for i in range(order_stat):
                word_list.append(input('Enter word {}:'.format(i+1)))
            num_to_gen = eval(input("Enter # of words to generate:"))
        except:
            print('Please enter valid parameters')
        else:
            print(produce_text(corpus, word_list, num_to_gen))
            break