import prompt,traceback,math
from goody import irange, type_as_str
from pcollections import pnamedtuple

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



def batch_self_check(file_name,seperator='-->',show_exception_message=False):
    print('Starting batch_self_check')
    check_num, correct_count, wrong_count, wrong = 0, 0, 0, []
    for line in open(file_name):
        line = line.rstrip()
        check_num += 1
        try:
            # #
            if line[0] == '#':
                print(line)
                continue
            kind,to_compute,correct = line.split(seperator)
            # Command
            if kind =='c':
                exec(to_compute)
                correct_count += 1
            # Query/Expressions
            elif kind == 'e':
                computed = str(eval(to_compute))
                if correct != '':
                    if computed == correct:
                        correct_count += 1
                    else:
                        wrong_count += 1
                        wrong.append(check_num)
                        print(check_num,'Error:',to_compute,'->',computed,'but should ->',correct)
            # Exception expected
            elif kind == '^':
                try:
                    exec(to_compute)
                    wrong_count += 1
                    wrong.append(check_num)
                    print(check_num,'Error:',to_compute,'should have raised exception:', to_compute)
                except Exception as exc:
                    if any([isinstance(exc,eval(e)) for e in correct.split(',')]):
                        correct_count += 1
                        if show_exception_message:
                            print('  Correct exception('+type_as_str(exc)+'); message:',exc)
                    else:
                        wrong_count += 1
                        wrong.append(check_num)
                        print(check_num,'Error: Incorrect exception('+type_as_str(exc)+'); message:',exc)
                        
        except Exception:
            wrong_count += 1
            wrong.append(check_num)
            print(check_num,'Error:',to_compute,'raised exception')
            traceback.print_exc()
    print('\nDone batch_self_check:',correct_count,'correct;',wrong_count,'incorrect\n')
    print('Failed checks:',wrong)

# Driver: prompts and executes commands (including calling the methods above)
old = ''
while True:
    try:
        old = prompt.for_string('Command',default=old)
        exec(old)
    except Exception:
        traceback.print_exc()
     
