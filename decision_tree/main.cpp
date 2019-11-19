#include <cassert>
#include <cstdio>
#include <cstring>

#include "decision_tree.h"

/*const size_t TEXT_SIZE = 256; /// size of text inside Node

const char* TREE_FILENAME = "../tree.txt";
const char* TREE_STORAGE_FILENAME = "../tree.txt";
const char* VIS_FILENAME = "../tree_vis.txt";

FILE* TREE_FILE = nullptr; /// file containing tree for loading
bool tree_file_open = false;

FILE* TREE_STORAGE_FILE = nullptr; /// file in which the tree would be stored
FILE* VIS_FILE = nullptr; /// file containing the tree for running the graphviz

char* GetString() {
  char* str = new char[TEXT_SIZE];
  size_t position = 0;
  int symbol = 0;

  symbol = getc(stdin);

  while (symbol == '\n') {
    symbol = getc(stdin);
  }

  while ((symbol != '\n') && (position < TEXT_SIZE)) {
    str[position++] = symbol;
    symbol = getc(stdin);
  }

  assert(str != nullptr);
  return str;
}

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


  explicit DecisionTree(const char* text) {
    root = new Node(text);
  }

  ~DecisionTree() {
    if (tree_file_open) {
      fclose(TREE_FILE);
      tree_file_open = false;
    }
  }

  void AddObject() {
    printf("Пожалуйста, укажите загаданный объект.\n");

    char* object_name = GetString();
    assert(object_name != nullptr);

    Node* object = new Node(object_name);
    object->is_object = true;

    if (is_left) {
      previous->left = object;
    } else {
      previous->right = object;
    }

  }

  void ResultCheck(Node* object) {
    assert(object != nullptr);
    assert(object->text != nullptr);
    assert(object->is_object);

    int result = 0;

    printf("Похоже Вы загадали %s.\n", object->text);
    printf("Верно ли предположение? [0/1]\n");

    scanf("%d", &result);
    if (result) {
      printf("Отлично! Возвращаюсь в начало.\n");

    } else {
      printf("Похоже нужна Ваша помощь... Придумайте, пожалуйста, признак,"
             "по которму отличаются %s и загаданный Вами ответ.\n",
             object->text);

      if (previous->left == object) {
        previous->left = new Node(GetString());
        previous = previous->left;
      } else {
        previous->right = new Node(GetString());
        previous = previous->right;
      }

      printf("Верно ли это для %s? [0/1]\n", object->text);
      scanf("%d", &result);

      if (result) {
        previous->left = object;
        is_left = false;
        AddObject();

      } else {
        previous->right = object;
        is_left = true;
        AddObject();
      }

    }
  }

  void Descent() {
    assert(root != nullptr);
    assert(root->text != nullptr);

    current = root;
    is_left = false;
    int result = 0;

    while (!current->is_object) {
      previous = current;
      printf("%s? [0/1]\n", current->text);
      scanf("%d", &result);

      if (result) {
        is_left = true;
        current = current->left;
      } else {
        is_left = false;
        current = current->right;
      }

      if (current == nullptr) {
        return;
      }
    }
  }

  void Run() {
    assert(root != nullptr);
    assert(root->text != nullptr);

    int result = 0;
    while (true) {
      Descent();

      if (current == nullptr) {
        printf("Объект с указанными признаками не найден... ");
        AddObject();
      } else {
        ResultCheck(current);
      }

      printf("Продолжить работу? [0/1]\n");
      scanf("%d", &result);
      if (result == 0) {
        return;
      }

    }
  }

////////////////////////////////////////////////////////////////////////////////

  void OpenFile() {
    assert(TREE_FILENAME != nullptr);

    tree_file_open = true;
    TREE_FILE = fopen(TREE_FILENAME, "r");
    assert(TREE_FILE != nullptr);

    fseek(TREE_FILE, 0, SEEK_END);

    size_t file_size = ftell(TREE_FILE);
    file.buffer = new char[file_size];
    file.position = file.buffer;

    fseek(TREE_FILE, 0, SEEK_SET);
    fread(file.buffer, sizeof(char), file_size, TREE_FILE);
  }

  char* GetTextFromBuffer() {
    assert(file.position != nullptr);
    assert(file.buffer != nullptr);

    char* node_delimiter = strpbrk(file.position + 1, node_delimiters);
    char* beginning = strpbrk(file.position + 1, str_delimiters);

    if ((beginning == nullptr) || (beginning > node_delimiter)) {
      file.position = node_delimiter;
      return nullptr;
    }

    file.position = strpbrk(beginning + 1, str_delimiters);
    size_t size = file.position - beginning - 1;
    char* text = new char[size];
    memcpy(text, beginning + 1, size);
    file.position = node_delimiter;

    assert(text != nullptr);
    return text;
  }

  void BuildSubTree(Node* previous, bool is_left) {
    assert(previous != nullptr);

    char* text = GetTextFromBuffer();
    Node* current = nullptr;

    assert(file.position != nullptr);
    assert(file.buffer != nullptr);

    if (text != nullptr) {
       current = new Node(text);

      if (is_left) {
        previous->left = current;
      } else {
        previous->right = current;
      }

    }


    if (file.position[0] == '{') {
      is_object = true;
      BuildSubTree(current, true);
    }

    if (file.position[0] == '}') {

      if (current != nullptr) {
        current->is_object = is_object;
      }

      is_object = false;
      file.position = strpbrk(file.position + 1, node_delimiters);
      if ((file.position == nullptr) || file.position[0] == '}') {
        return;
      }
      is_object = true;

      BuildSubTree(previous, false);
    }

  }

  DecisionTree() {

    OpenFile();

    root = new Node(nullptr);
    BuildSubTree(root, true);
    root = root->left;

  }

////////////////////////////////////////////////////////////////////////////////

  void StoreTree() {
    assert(TREE_STORAGE_FILENAME != nullptr);
    TREE_STORAGE_FILE = fopen(TREE_STORAGE_FILENAME, "w");
    assert(TREE_STORAGE_FILE != nullptr);

    fprintf(TREE_STORAGE_FILE, "{\n");
    StoreNode(root);
    fprintf(TREE_STORAGE_FILE, "}");

    fclose(TREE_STORAGE_FILE);
  }

  void StoreNode(Node* local) {
    assert(local != nullptr);

    fprintf(TREE_STORAGE_FILE, "\"%s\"\n", local->text);
    if (!local->is_object) {
      fprintf(TREE_STORAGE_FILE, "{\n");
      if (local->left != nullptr) {
        StoreNode(local->left);
      }
      fprintf(TREE_STORAGE_FILE, "}\n");
      fprintf(TREE_STORAGE_FILE, "{\n");
      if (local->right != nullptr) {
        StoreNode(local->right);
      }
      fprintf(TREE_STORAGE_FILE, "}\n");
    }
  }

////////////////////////////////////////////////////////////////////////////////

  void StoreTreeVis() {
    size_t node_index = 0;
    VIS_FILE = fopen(VIS_FILENAME, "w");

    fprintf(VIS_FILE, "digraph DecisionTree {\n");
    fprintf(VIS_FILE, "node [shape=record]\n");

    fprintf(VIS_FILE, "Node%zu [label=\"{%p | \\\"%s\\\"}\"]\n",
        node_index, root, root->text);

    StoreNodeVis(root, node_index);

    fprintf(VIS_FILE, "}");
    fclose(VIS_FILE);
  }

  void StoreNodeVis(Node* local, size_t& node_index) {
    assert(local != nullptr);
    assert(node_index >= 0);

    size_t local_previous = node_index;

    if (local->left != nullptr) {
      fprintf(VIS_FILE, "Node%zu [label=\"{%p | \\\"%s\\\"}\"]\n",
          ++node_index, local->left, local->left->text);

      fprintf(VIS_FILE, "Node%zu -> Node%zu [label=\"Yes\"]\n",
          local_previous, node_index);
    }

    if (local->left != nullptr) {
      StoreNodeVis(local->left, node_index);
    }

    if (local->right != nullptr) {
      fprintf(VIS_FILE, "Node%zu [label=\"{%p | \\\"%s\\\"}\"]\n",
          ++node_index, local->right, local->right->text);

      fprintf(VIS_FILE, "Node%zu -> Node%zu [label=\"No\"]\n",
          local_previous, node_index);
    }

    if (local->right != nullptr) {
      StoreNodeVis(local->right, node_index);
    }

  }

////////////////////////////////////////////////////////////////////////////////

  struct Path {
    size_t size = 100;
    size_t capacity = 0;
    bool* path = new bool[size];
    bool correct = false;

    ~Path() { delete[] path; }
  };

  void GetPath(Node* current, const char* object, Path* definition) {
    if (current->is_object) {
      if (strcmp(object, current->text) == 0) {
        definition->correct = true;
      }
      return;
    }

    size_t position = definition->capacity;

    if ((current->left != nullptr) && (!definition->correct)) {
      definition->path[definition->capacity++] = true;
      GetPath(current->left, object, definition);
    }

    if ((current->right != nullptr) && (!definition->correct)) {
      definition->capacity = position;
      definition->path[definition->capacity++] = false;
      GetPath(current->right, object, definition);
    }
  }

  void Definition() {
    Path definition;
    char* object_name = nullptr;
    printf("Какой объект интересует Вас сейчас?\n");
    object_name = GetString();
    printf("Сейчас посмотрим...\n");

    GetPath(root, object_name, &definition);

    if (!definition.correct) {
      printf("К сожалению, я и сам не знаю.\n");
      return;
    }

    current = root;
    printf("Согласно текущему дереву %s - это сущность, которая",
        object_name);
    for (size_t index = 0; index < definition.capacity; ++index) {
      if (definition.path[index]) {
        printf(", %s", current->text);
        current = current->left;
      } else {
        printf(", не %s", current->text);
        current = current->right;
      }
    }
    printf(".\n");
  }


};
*/

int main() {
  int result = 0;
  printf("Загружать дерево из фаила или создать с нуля? [0/1]\n");
  scanf("%d", &result);

  if (result) {
    printf("Введите первый признак, он обязателен.\n");
    char* root_text = GetString();
    assert(root_text != nullptr);
    DecisionTree tree(root_text);
    tree.Run();
    tree.StoreTree();
  }


  DecisionTree tree;
  while (true) {
    printf("Выберете задачу: отгадывание/добавление | определение | завершение [0/1/2]\n");
    scanf("%d", &result);
    switch (result) {
    case (0):
      tree.Run();
      break;
    case (1):
      tree.Definition();
      break;
    case (2):
      tree.StoreTree();
      tree.StoreTreeVis();
      return 0;
    }
  }
}