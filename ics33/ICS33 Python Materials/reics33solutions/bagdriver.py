import prompt
from bag import Bag


# Driver: prompts and executes commands (can all compute_e)
while True:
    try:
        exec(prompt.for_string('Command'))
    except Exception as report:
        import traceback
        traceback.print_exc()
     
