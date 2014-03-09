import prompt,traceback,math
from goody import irange
from rational import Rational
from bag import Bag

def compute_e(n):
    answer = Rational(1)
    for i in irange(1,n):
        answer += Rational(1,math.factorial(i))
    return answer

# Newton: pi = 6*arcsin(1/2); see the arcsin series at http://mathforum.org/library/drmath/view/54137.html
# Check your results at http://www.numberworld.org/misc_runs/pi-5t/details.html
def compute_pi(n):
    def prod(r):
        answer = 1
        for i in r:
            answer *= i
        return answer
    
    answer = Rational(1,2)
    x      = Rational(1,2)
    for i in irange(1,n):
        big = 2*i+1
        answer += Rational(prod(range(1,big,2)),prod(range(2,big,2)))*x**big/big       
    return 6*answer

def batch_test(file_name,confirm=False):
    print('Starting batch_test')
    for command in open(file_name,'r'):
        try:
            command = command.rstrip()
            print('\nCommand:',command)
            if confirm:
                prompt.for_string('Press enter to do command','')
            exec(command)
        except Exception:
            traceback.print_exc()
    print('\nDone batch_test\n')



def batch_self_check(file_name,seperator):
    print('Starting batch_self_check')
    check_num, correct_count, wrong_count, wrong = 0, 0, 0, []
    for line in open(file_name):
        check_num += 1
        try:
            kind,to_compute,correct = line.rstrip().split(seperator)
            if kind =='c':
                exec(to_compute)
                correct_count += 1
            elif kind == 'e':
                computed = str(eval(to_compute))
                if correct != '':
                    if computed == correct:
                        correct_count += 1
                    else:
                        wrong_count += 1
                        wrong.append(check_num)
                        print(check_num,'Error:',to_compute,'->',computed,'but should ->',correct)
            elif kind == '^':
                try:
                    exec(to_compute)
                    wrong_count += 1
                    wrong.append(check_num)
                    print(check_num,'Error:',to_compute,'should have raised exception:', to_compute)
                   
                except Exception:
                    correct_count += 1
        except Exception:
            wrong_count += 1
            wrong.append(check_num)
            print(check_num,'Error:',to_compute,'raised exception')
            traceback.print_exc()
    print('\nDone batch_self_check:',correct_count,'correct;',wrong_count,'incorrect\n')
    print('Failed checks:',wrong)

# Driver: prompts and executes commands (including calling the methods above)

while True:
    try:
        exec(prompt.for_string('Command'))
    except Exception:
        traceback.print_exc()
     
