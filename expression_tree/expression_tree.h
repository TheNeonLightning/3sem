#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>

enum Operations {
  sum = 0,
  dif = 1,
  mul = 2,
  divide = 3,
  sin = 4,
  cos = 5,
  pow = 6,
  id = 7,
  exp = 8,
  ln = 9,
};

class ExpressionTree {
public:

  struct Node {
    long value = 0;
    bool constant = true;

    Node* left = nullptr;
    Node* right = nullptr;
    Node* prev = nullptr;

    Node() = default;

    Node(long value, bool constant): value(value), constant(constant) {}

    Node(Node* node);
  };


  struct Buffer {
    char* buffer = nullptr;
    size_t position = 0;
    size_t size = 0;

    Buffer() = default;

    char* GetToken(const char* delimiters = "()");

    ~Buffer() { delete[] buffer; }
  } file;


  Node* root = nullptr;

  ExpressionTree();

  explicit ExpressionTree(Node* node);

private:

  void OpenFile(const char* filename, FILE* file_to_open);

////////////////////////////////////////////////////////////////////////////////
/// Differentiating ExpressionTree
public:

  ExpressionTree* DiffTree();

private:

  Node* DiffNode(Node* current);

  Node* DiffSum(Node* current);

  Node* DiffDifference(Node* current);

  Node* DiffDivide(Node* current);

  Node* DiffSin(Node* current);

  Node* DiffCos(Node* current);

  Node* DiffLn(Node* current);

  Node* DiffMultiplication(Node* current);

  Node* DiffExponent(Node* current);

  Node* DiffPow(Node* current);

  Node* DiffId(Node* current);

////////////////////////////////////////////////////////////////////////////////
/// Storing DecisionTree for visual representation with Graphviz
public:

  void StoreTreeVis();

private:

  void StoreNodeVis(Node* node, size_t& node_index, size_t local_previous);

  void StoreSubTreeVis(Node* local, size_t& node_index);

};
