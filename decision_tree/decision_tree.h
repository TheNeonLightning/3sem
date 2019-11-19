#pragma once

#include <cassert>
#include <cstdio>
#include <cstring>

char* GetString();

class DecisionTree {
public:

  struct Node {
    const char*  text = nullptr;

    bool is_object = false;

    Node* left = nullptr;
    Node* right = nullptr;

    Node() = default;

    Node(const char* text) : text(text) {}

    ~Node() { delete[] text; }

  };

  struct Buffer {
    char* buffer = nullptr;
    char* position = nullptr;

    Buffer() = default;

    ~Buffer() { delete[] buffer; }
  } file;

  const char* str_delimiters = "\"";
  const char* node_delimiters = "{}";

  Node* root = nullptr;


  Node* current = nullptr;
  Node* previous = nullptr;
  bool is_left = false;
  bool is_object = false;


  explicit DecisionTree(const char* text);

  ~DecisionTree();

  void AddObject();

  void ResultCheck(Node* object);

  void Descent();

  void Run();

////////////////////////////////////////////////////////////////////////////////

  void OpenFile();

  char* GetTextFromBuffer();

  void BuildSubTree(Node* previous, bool is_left);

  DecisionTree();

////////////////////////////////////////////////////////////////////////////////

  void StoreTree();

  void StoreNode(Node* local);

////////////////////////////////////////////////////////////////////////////////

  void StoreTreeVis();

  void StoreNodeVis(Node* local, size_t& node_index);

////////////////////////////////////////////////////////////////////////////////

  struct Path {
    size_t size = 100;
    size_t capacity = 0;
    bool* path = new bool[size];
    bool correct = false;

    ~Path() { delete[] path; }
  };

  void GetPath(Node* current, const char* object, Path* definition);

  void Definition();

};
