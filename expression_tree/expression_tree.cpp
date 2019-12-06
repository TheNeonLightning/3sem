#include "expression_tree.h"
#include "constants.h"

void ExpressionTree::OpenFile(const char* filename, FILE* file_to_open) {
  assert(filename != nullptr);

  file_to_open = fopen(filename, "r");
  assert(file_to_open != nullptr);

  fseek(file_to_open, 0, SEEK_END);

  size_t file_size = ftell(file_to_open);
  file.size = file_size;
  file.buffer = new char[file_size];

  fseek(file_to_open, 0, SEEK_SET);
  fread(file.buffer, sizeof(char), file_size, file_to_open);
  fclose(file_to_open);
}


ExpressionTree::ExpressionTree() {
  assert(EXPR_FILENAME != nullptr);

  OpenFile(EXPR_FILENAME, EXPR_FILE);

  root = new Node();
  Node* current = root;

  char* token = nullptr;

  while (file.position < file.size) {

    if (file.buffer[file.position] == '(') {
      ++file.position;

      if (current->left == nullptr) {
        current->left = new Node();
        current->left->prev = current;
        current = current->left;

      } else {

        current->right = new Node();
        current->right->prev = current;
        current = current->right;
      }

      continue;
    }

    if (file.buffer[file.position] == ')') {
      ++file.position;
      current = current->prev;
      continue;
    }

    token = file.GetToken();

    for (size_t command = 0; command < OPERATIONS_NUM; ++command) {
      if (strcmp(token, OPERATIONS[command]) == 0) {
        current->value = command;
        current->constant = false;
        break;
      }
    }

    if (current->constant) {
      current->value = strtol(token, nullptr, 10);
      continue;
    }

    delete[] token;
  }
}

ExpressionTree::ExpressionTree(Node* node) {
assert(node != nullptr);
root = new Node(node);
}


////////////////////////////////////////////////////////////////////////////////
/// Storing DecisionTree for visual representation with Graphviz

void ExpressionTree::StoreTreeVis() {
  size_t node_index = 0;

  VIS_FILE = fopen(VIS_FILENAME, "w");

  fprintf(VIS_FILE, "digraph DecisionTree {\n");
  fprintf(VIS_FILE, "node [shape=record]\n");

  fprintf(VIS_FILE, "Node%zu [label=\"{%p | %s}\"]\n", node_index,
          root, OPERATIONS[root->value]);

  StoreSubTreeVis(root, node_index);

  fprintf(VIS_FILE, "}");
  fclose(VIS_FILE);
}

void ExpressionTree::StoreNodeVis(Node* node, size_t& node_index,
                                  size_t local_previous) {
  assert(node != nullptr);

  if (node->constant) {

    fprintf(VIS_FILE, "Node%zu [label=\"{%p | %ld}\"]\n", ++node_index, node,
            node->value);

  } else {

    fprintf(VIS_FILE, "Node%zu [label=\"{%p | %s}\"]\n", ++node_index, node,
            OPERATIONS[node->value]);
  }

  fprintf(VIS_FILE, "Node%zu -> Node%zu\n", local_previous, node_index);
}

void ExpressionTree::StoreSubTreeVis(Node* local, size_t& node_index) {
  assert(local != nullptr);
  assert(node_index >= 0);

  size_t local_previous = node_index;


  if (local->left != nullptr) {
    StoreNodeVis(local->left, node_index, local_previous);
    StoreSubTreeVis(local->left, node_index);
  }


  if (local->right != nullptr) {
    StoreNodeVis(local->right, node_index, local_previous);
    StoreSubTreeVis(local->right, node_index);
  }

}

////////////////////////////////////////////////////////////////////////////////
/// Differentiating ExpressionTree

ExpressionTree* ExpressionTree::DiffTree() {
  assert(root != nullptr);

  ExpressionTree* diff_tree = new ExpressionTree(DiffNode(root));

  return diff_tree;

}

ExpressionTree::Node* ExpressionTree::DiffNode(Node* current) {
  assert(current != nullptr);

  Node* node = nullptr;

  if (current->constant) {
    node = new Node(0, true);

  } else {

    switch (current->value) {
    case (sum):
      node = DiffSum(current);
      break;

    case (dif):
      node = DiffDifference(current);
      break;

    case (mul):
      node = DiffMultiplication(current);
      break;

    case (divide):
      node = DiffDivide(current);
      break;

    case (sin):
      node = DiffSin(current);
      break;

    case (cos):
      node = DiffCos(current);
      break;

    case (ln):
      node = DiffLn(current);
      break;

    case (exp):
      node = DiffExponent(current);
      break;

    case (pow):
      node = DiffPow(current);
      break;

    case (id):
      node = DiffId(current);
      break;

    }
  }
  return node;
}

ExpressionTree::Node* ExpressionTree::DiffSum(Node* current) {
  Node* node = new Node(sum, false);
  node->left = DiffNode(current->left);
  node->right = DiffNode(current->right);
  return node;
}

ExpressionTree::Node* ExpressionTree::DiffDifference(Node* current) {
  Node* node = new Node(dif, false);
  node->left = DiffNode(current->left);
  node->right = DiffNode(current->right);
  return node;
}

ExpressionTree::Node* ExpressionTree::DiffDivide(Node* current) {
  Node* node = new Node(divide, false);

  node->left = DiffMultiplication(current);
  node->left->value = dif;

  node->right = new Node(pow, false);
  node->right->left = new Node(current->right);
  node->right->right = new Node(2, true);
  return node;
}

ExpressionTree::Node* ExpressionTree::DiffSin(Node* current) {
  Node* node = new Node(mul, false);

  node->left = new Node(cos, false);
  node->left->left = new Node(current->left);

  node->right = DiffNode(current->left);
  return node;
}

ExpressionTree::Node* ExpressionTree::DiffCos(Node* current) {
  Node* node = new Node(mul, false);

  node->left = new Node(mul, false);

  node->left->left = new Node(sin, false);
  node->left->right = new Node(-1, true);

  node->left->left->left = new Node(current->left);

  node->right = DiffNode(current->left);

  return node;
}

ExpressionTree::Node* ExpressionTree::DiffLn(Node* current) {
  Node* node = new Node(mul, false);

  node->left = new Node(divide, false);
  node->left->left = new Node(1, true);
  node->left->right = new Node(current->left);

  node->right = DiffNode(current->left);

  return node;
}

ExpressionTree::Node* ExpressionTree::DiffMultiplication(Node* current) {
  Node* node = new Node(sum, false);
  node->left = new Node(mul, false);
  node->right = new Node(mul, false);

  node->left->left = DiffNode(current->left);
  node->left->right = new Node(current->right);

  node->right->left = new Node(current->left);
  node->right->right = DiffNode(current->right);
  return node;
}

ExpressionTree::Node* ExpressionTree::DiffExponent(Node* current) {
  Node* node = new Node(mul, false);
  node->right = DiffNode(current->left);
  node->left = new Node(current);
  return node;
}

ExpressionTree::Node* ExpressionTree::DiffPow(Node* current) {
  Node* local = new Node(exp, false);

  local->left = new Node(mul, false);
  local->left->left = new Node(ln, false);
  local->left->left->left = new Node(current->left);

  local->left->right = new Node(current->right);

  Node* node = new Node(mul, false);
  node->right = DiffNode(local->left);
  node->left = local;

  return node;
}

ExpressionTree::Node* ExpressionTree::DiffId(Node* current) {
  Node* node = new Node(1, true);
  return node;
}

////////////////////////////////////////////////////////////////////////////////
/// ExpressionTree::Buffer and ExpressionTree::Node

char* ExpressionTree::Buffer::GetToken(const char* delimiters) {
  size_t length = 0;

  while ((buffer[position + length] != delimiters[0]) &&
         (buffer[position + length] != delimiters[1])) {
    ++length;
  }

  char* token = new char[length + 1];
  memcpy(token, buffer + position, length);
  position += length;
  token[length] = '\0';
  return token;
}

ExpressionTree::Node::Node(Node* node) {
  value = node->value;
  constant = node->constant;
  if (node->left != nullptr) {
    left = new Node(node->left);
  }
  if (node->right != nullptr) {
    right = new Node(node->right);
  }
}
