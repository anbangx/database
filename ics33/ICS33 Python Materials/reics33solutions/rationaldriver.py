import prompt, math
from goody import irange
from rational import Rational
from itertools import accumulate
from operator import mul

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


for i in irange(1,20):
    print(compute_e(i))
# Driver: prompts and executes commands (can call compute_e/computer_pi)
while True:
    try:
        exec(prompt.for_string('Command'))
    except Exception as report:
        import traceback
        traceback.print_exc()
     
    