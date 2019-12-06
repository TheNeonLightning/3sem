#include "expression_tree.h"

int main() {
    ExpressionTree tree;
    ExpressionTree* diff_tree = tree.DiffTree();
    diff_tree->StoreTreeVis();
    return 0;
}
