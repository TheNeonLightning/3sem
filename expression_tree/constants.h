const size_t OPERATIONS_NUM = 10;
const char* OPERATIONS[OPERATIONS_NUM]  =
    {"+", "-", "*", "/", "sin", "cos", "^", "x", "exp" , "ln"};

const char* VIS_FILENAME = "../expression_vis.txt";
const char* EXPR_FILENAME = "../expression.txt";

FILE* EXPR_FILE = nullptr;
FILE* VIS_FILE = nullptr;
