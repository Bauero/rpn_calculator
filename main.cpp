#include <iostream>
#include <string>
#include <stack>
#include <math.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::stack;


// GLOBAL VARIABLES
stack<char> rpn_equation;
stack<char> instructions;


// Funckcje w programie
void czyscRownanie(){
    while (!rpn_equation.empty()){
        rpn_equation.pop();
    }
}

void wpiszZnaki(){
    while (instructions.top() != '|'){
        rpn_equation.push(' ');
        rpn_equation.push(instructions.top());
        instructions.pop();
    }
}

void dekoduj(string input){
    czyscRownanie();

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
                instructions.push('|');
                break;
            }
            case ')' : {
                level--;
                lcwan = false;
                wpiszZnaki();
                instructions.pop();
                break;
            }
            case '+' : {
                if (lcal > 1) { wpiszZnaki(); }
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
                    if (lcal > 1) { wpiszZnaki(); }
                    lcal = 1;
                    lcwan = false;
                    instructions.push('-');
                } 
                break;
            }
            case '*' : {
                if (lcal > 2) { wpiszZnaki(); }
                lcal = 2;
                lcwan = false;
                instructions.push('*');
                break;
            }
            case '/' : {
                if (lcal > 2) { wpiszZnaki(); }
                lcal = 2;
                lcwan = false;
                instructions.push('/');
                break;
            }
            case ':' : {
                if (lcal > 2) { wpiszZnaki(); }
                lcal = 2;
                lcwan = false;
                instructions.push('/');
                break;
            }
            case '^' : {
                if (lcal > 3) { wpiszZnaki(); }
                lcal = 3;
                lcwas = true;
                lcwan = false;
                instructions.push('^');
                break;
            }
            case 'v' : {
                if (lcal > 3) { wpiszZnaki(); }
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
    wpiszZnaki();
}

int oblicz(){
    stack<float> values;

    float var_l = 0;
    float var_r = 0;
    int no_args = 0;  // amount of arguments available at the moment (used to
                        // determine which operation could be performed)
    bool pobieranie_liczby = false;
    bool coma_value = false;
    bool negative_number = false;
    int dividor = 10;

    while (!rpn_equation.empty()){

        char v = rpn_equation.top();
        char i = instructions.top();

        if (v == ' ') {
            if (pobieranie_liczby) { 
               pobieranie_liczby = false;
               coma_value = false;
               negative_number = false;
               no_args++;
            }
            if (i == 'v') {
                var_r = sqrtf(values.top());
                instructions.pop();
                values.pop();
                values.push(var_r);
                var_r = 0;
            }
            else if (no_args > 1){
                var_l = values.top();
                values.pop();
                var_r = values.top();
                values.pop();
                switch (i) {
                    case '+' : { values.push(var_l + var_r); break; }
                    case '-' : { values.push(var_l - var_r); break; }
                }
                var_l = var_r = 0;
            }
        }
        else {
            if (v == 45 || v == 46 || (v >= 48 && v <= 57)) {
                signed int tmp = 0;
                if (v == 45) {
                    tmp = -1;
                    negative_number = true;
                    pobieranie_liczby = true;
                }
                else if (v == 46) {
                    coma_value = true;
                }
                else{
                    if (pobieranie_liczby) {
                        tmp = values.top();
                        values.pop();
                    }
                    pobieranie_liczby = true;
                    if (coma_value) {
                        if (!negative_number)   tmp += (v - 48)/dividor;
                        else                    tmp -= (v - 48)/dividor;
                        dividor *= 10;
                    }
                    else {
                        tmp *= 10;
                        if (!negative_number)   tmp += v - 48;
                        else                    tmp -= v - 48;
                    }
                }
                values.push(tmp);
            }
            else { instructions.push(v); }
        }
    }

    return var_r;
}


// PROGRAM GŁÓWNY
int main(){
    instructions.push('|');

    string s;
    cout << "Write equation: ";
    cin >> s;
    dekoduj(s);

    stack<char> tmpStack;

    cout  << "RPN equation: '";
    while (!rpn_equation.empty()){
        tmpStack.push(rpn_equation.top());
        rpn_equation.pop();
    }

    while (!tmpStack.empty()){
        cout << tmpStack.top();
        tmpStack.pop();
    }
    
    cout << "'" << endl;

    return 0;
}
