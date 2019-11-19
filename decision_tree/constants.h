const size_t TEXT_SIZE = 256; /// size of text inside Node

const char* TREE_FILENAME = "../tree.txt";
const char* TREE_STORAGE_FILENAME = "../tree.txt";
const char* VIS_FILENAME = "../tree_vis.txt";

FILE* TREE_FILE = nullptr; /// file containing tree for loading
bool tree_file_open = false;

FILE* TREE_STORAGE_FILE = nullptr; /// file in which the tree would be stored
FILE* VIS_FILE = nullptr; /// file containing the tree for running the graphviz
