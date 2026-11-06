// author: william noonan

#include <iostream>
#include <vector>
#include <cmath> // for std::pow
#include <iomanip>
#include <map>
#include <set>


static bool is_digit(const char ch) {
    return (ch >= '0' && ch <= '9');
}

static double calculate(const double num1, const double num2, const char op) {
    switch (op) {
        case '^': return std::pow(num1, num2);
        case '*': return num1 * num2;
        case '/': return num1 / num2;
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        default: throw std::runtime_error("Invalid operator.");
    }
}

template <typename T>
static void printVector(const std::vector<T>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}

static void reduceStack(std::vector<double>& number_stack, std::vector<char>& operator_stack) {
    const char op = operator_stack.back();
    operator_stack.pop_back();
    if (op == 'n') {
        number_stack.back() = -number_stack.back();
    } else {
        double num2 = number_stack.back();
        number_stack.pop_back();

        double num1 = number_stack.back();
        number_stack.pop_back();

        number_stack.push_back(calculate(num1, num2, op));
    }
}

static double roundToDecimalPlaces(double value, int places = 5) {
    double multiplier = std::pow(10.0, places);
    return std::round(value * multiplier) / multiplier;
}


static void computeExpression(const std::string &expression, const int places = 5) {

    const std::set operator_set = {'^', '*', '/', '+', '-'};
    const std::map<char, double> operatorRank = {
        {'^', 1.0},
        {'n', 1.5},
        {'*', 2.0},
        {'/', 2.0},
        {'+', 3.0},
        {'-', 3.0}
    };

    std::vector<double> number_stack;
    std::vector<char> operator_stack;

    bool expect_number = true;

    size_t i = 0;
    while (i < expression.length()) {
        char ch = expression[i];

        if (ch == ' ') {
            i++;
            continue;
        }

        if (is_digit(ch) || ch == '.') {
            size_t start = i;
            while (i < expression.length() && (is_digit(expression[i]) or expression[i] == '.')) {
                i++;
            }
            std::string sub = expression.substr(start, i - start);
            double number = std::stod(sub);
            number_stack.push_back(number);
            expect_number = false;  // just saw one
            continue;  // continue reading character at current index
        }

        if (ch == '(') {
            operator_stack.push_back(ch);
            expect_number = true;
        } else if (ch == ')') {
            while (operator_stack.back() != '(') {
                reduceStack(number_stack, operator_stack);
            }
            operator_stack.pop_back();  // pops the '(' at the top
            // just collapsed a (...) into a single number, so an operator is expected next
            expect_number = false;
        } else if (operator_set.count(ch)) {
            if (expect_number) {
                // expecting a number but see an operator means its the unary minus
                operator_stack.push_back('n');
            } else {
                while (!operator_stack.empty() && operator_stack.back() != '(' && (
                    operatorRank.at(operator_stack.back()) < operatorRank.at(ch) ||
                    (operatorRank.at(operator_stack.back()) == operatorRank.at(ch) && ch != '^')
                )) {
                    reduceStack(number_stack, operator_stack);
                }
                // push new operator to stack
                operator_stack.push_back(ch);
                expect_number = true;
            }
        }

        i++;
    }

    while (!operator_stack.empty())
        reduceStack(number_stack, operator_stack);

    double rounded = roundToDecimalPlaces(number_stack.back(), places);
    std::cout << rounded << std::endl;
}


int main() {

    std::vector<std::string> expressions = {
        "10 - 3 + 2",
        "2 * (3.14 + 4)",
        "100 / (2 * 3 + 4)",
        "1 + (-7.2) * 2",
        "3^6 / 117",
        "(2.16 - 48.34)^-1" ,
        "5 + -6 / 2",
        "6 / -2 + 1"
    };

    for (const std::string& e : expressions) {
        computeExpression(e);
    }

    return 0;
}