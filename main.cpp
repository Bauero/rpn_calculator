#include <iostream>
#include <string>
#include <stack>
#include <math.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::stack;

////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////////////
stack<char> rpn_equation;
stack<char> instructions;


////////////////////////////////////////////////////////////////////////////////
// FUNCITONS IN PROGRAM
////////////////////////////////////////////////////////////////////////////////
void cleanRPNEquationStack(){

    /// Remove all values from `rpn_equaiton` stack

    if (!rpn_equation.empty()){
        while (!rpn_equation.empty()){
            rpn_equation.pop();
        }
    }
}

void cleanInstructionsStack(){

    /// Remove all values from `instructions` stack

    while (!instructions.empty()){
        instructions.pop();
    }
}

void clearCurrentSetOfInstructions(){

    /* This function is responible adding all instruction which are stored in
    the instruction stack, to `rpn_equation` stack*/

    while (instructions.top() != '|'){
        rpn_equation.push(' ');
        rpn_equation.push(instructions.top());
        instructions.pop();
    }
}

void decode(string input){
    /**
     * This functions is responsible for decoding the initial equaiton and
     * rewriting it into Reverse Polish Notation
     * 
     * IMPORTANT! : the end result is stored upside-down (first char from the
     * top, is the last one from the 'left' if we were to rewrite it)
     * 
     * @return function doesn't return anything -> end result is stored inside
     * `rpn_equation` stack!
    */
    
    cleanRPNEquationStack();
    instructions.push('|');

    /*
    (+) (-)  level 1
    (*) (/)  level 2
    (^) (v)  level 3 (power, sqrt)
    ( '()' ) level 4
    */

    int level = 0; // each nested paranthesis is lvl up
    int lcal = 0;  // last character abstraction lvl

    bool lcwan = false; // last character was a number
    bool lcwas = false; // last character was a sign (+-/*^v)

    for (int i = 0; i < input.length(); i++){
        
        // Skip any unnecessary characters
        if (input[i] < 40) { continue; }
        if (input[i] > 58) {
            if (!(input[i] == 118 || input[i] == 94)) { continue; }
        }

        // if character is valid (wasn't discarded)
        switch (input[i]){
            case '(' : {
                level++;
                lcwan = false;
                lcwas = false;
                instructions.push('|');
                break;
            }
            case ')' : {
                level--;
                lcwan = false;
                lcwas = false;
                clearCurrentSetOfInstructions();
                instructions.pop();
                break;
            }
            case '+' : {
                if (lcal > 1) { clearCurrentSetOfInstructions(); }
                lcal = 1;
                lcwas = true;
                lcwan = false;
                instructions.push('+');
                break;
            }
            case '-' : {
                if (lcwas || i == 0) {
                    lcwan = true;
                    rpn_equation.push(' ');
                    rpn_equation.push('-');
                }
                else {
                    lcwas = true;
                    if (lcal > 1) { clearCurrentSetOfInstructions(); }
                    lcal = 1;
                    lcwan = false;
                    instructions.push('-');
                } 
                break;
            }
            case '*' : {
                if (lcal > 2) { clearCurrentSetOfInstructions(); }
                lcal = 2;
                lcwas = true;
                lcwan = false;
                instructions.push('*');
                break;
            }
            case '/' : {
                if (lcal > 2) { clearCurrentSetOfInstructions(); }
                lcal = 2;
                lcwas = true;
                lcwan = false;
                instructions.push('/');
                break;
            }
            case ':' : {
                if (lcal > 2) { clearCurrentSetOfInstructions(); }
                lcal = 2;
                lcwas = true;
                lcwan = false;
                instructions.push('/');
                break;
            }
            case '^' : {
                if (lcal > 3) { clearCurrentSetOfInstructions(); }
                lcal = 3;
                lcwas = true;
                lcwan = false;
                instructions.push('^');
                break;
            }
            case 'v' : {
                if (lcal > 3) { clearCurrentSetOfInstructions(); }
                lcal = 3;
                lcwas = true;
                lcwan = false;
                instructions.push('v');
                break;
            }
            default:
            {
                if (input[i] >= 48 || input[i] <= 57){
                    char str;
                    if (i > 0 && !lcwan) { rpn_equation.push(' '); }
                    if (input[i] == '.' || input[i] == ',') { str = '.'; }
                    else { str = input[i]; }
                    lcwan = true;
                    lcwas = false;
                    rpn_equation.push(str);
                    break;
                }
            }
        }
    }
    clearCurrentSetOfInstructions();
}

float calculate(){
    /**
     * This function is executed when one want to calculate values which are
     * stored in 'rpn_equation' stack - it is assumed that the funciton is run
     * after `decode` function, therefore calculation is done after decoding
     * the initial string
     * 
     * @return float value, being the end result of calculation
    */

    stack<float> values;

    float var_l = 0;
    float var_r = 0;
    int no_args = 0;  // amount of arguments available at the moment (used to
                        // determine which operation could be performed)
    bool reading_number = false;
    bool coma_value = false;
    int multiplier = 1;

    while (!rpn_equation.empty()){

        char v = rpn_equation.top();
        char i = instructions.top();

        if (v == ' ') {
            if (reading_number) { 
               reading_number = false;
               coma_value = false;
               multiplier = 1;
               no_args++;
            }
            math_operand:
            if (i == 'v' && no_args >= 1) {
                var_r = sqrtf(values.top());
                instructions.pop();
                values.pop();
                values.push(var_r);
                var_r = 0;
            }
            else if (no_args >= 2){
                var_l = values.top();
                values.pop();
                var_r = values.top();
                values.pop();
                switch (i) {
                    case '+' : { values.push(var_l + var_r); break; }
                    case '-' : { values.push(var_l - var_r); break; }
                    case '*' : { values.push(var_l * var_r); break; }
                    case '/' : { values.push(var_l / var_r); break; }
                    case '^' : { values.push(pow(var_l, var_r)); break; }
                }
                instructions.pop();
                no_args--;
                var_l = var_r = 0;
                if (no_args >=1 ) goto math_operand;
            }
        }
        else {
            if ((v == 45 && reading_number) || 
                 v == 46 ||
                (v >= 48 && v <= 57)) {
                signed int tmp = 0;
                if (reading_number){
                    if (v == 46) {
                        tmp = values.top() / multiplier;
                        values.pop();
                        coma_value = true;
                        multiplier = 1;
                    }
                    else if (v == 45) {
                        tmp = -1 * values.top();
                        values.pop();
                    }
                    else {
                        tmp = values.top() + (v - 48) * multiplier;
                        values.pop();
                        multiplier *= 10;
                    }
                }
                else{
                    reading_number = true;
                    if (v == 46) { continue;}
                    tmp = v - 48;
                    multiplier *= 10;
                }
                values.push(tmp);
            }
            else { instructions.push(v); }
        }

        rpn_equation.pop();
    }

    return values.top();
}


////////////////////////////////////////////////////////////////////////////////
// MAIN PROGRAM
////////////////////////////////////////////////////////////////////////////////
int main(){
    

    string s;
    cout << "Write equation: ";
    cin >> s;
    decode(s);

    stack<char> tmpStack;

    cout  << "RPN equation: '";
    while (!rpn_equation.empty()){
        tmpStack.push(rpn_equation.top());
        rpn_equation.pop();
    }


    if (!rpn_equation.empty()){
        while (!rpn_equation.empty()){
            rpn_equation.pop();
        }
    }

    rpn_equation.push(' '); // added, so algorytm can perform last equation
                            // -> equations are perfomed when space is found

    while (!tmpStack.empty()){
        cout << tmpStack.top();
        rpn_equation.push(tmpStack.top());
        tmpStack.pop();
    }
    
    cout << "' = " << calculate() << endl;
    
    cleanInstructionsStack();

    return 0;
}
