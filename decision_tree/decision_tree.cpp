#include <cstdlib>
#include "decision_tree.h"
#include "constants.h"

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

  void* str_reallocated = realloc(str, position);
  assert(str_reallocated != nullptr);

  str = static_cast<char*>(str_reallocated);

  assert(str != nullptr);
  return str;
}

////////////////////////////////////////////////////////////////////////////////
///Decision tree operating functions

DecisionTree::DecisionTree(const char* text) {
  root = new Node(text);
}

DecisionTree::~DecisionTree() {
  if (tree_file_open) {
    fclose(TREE_FILE);
    tree_file_open = false;
  }
}

void DecisionTree::AddObject() {
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

void DecisionTree::ResultCheck(Node* object) {
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

void DecisionTree::Descent() {
  assert(root != nullptr);
  assert(root->text != nullptr);

  current = root;
  is_left = false;
  int result = 0;

  while (!current->is_object) {
    previous = current;
    printf("%s? [0/1]\n", current->text);
    scanf("%d", &result);

    if (result == 1) {
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

void DecisionTree::Run() {
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
///Loading DecisionTree from the file

void DecisionTree::OpenFile() {
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

char* DecisionTree::GetTextFromBuffer() {
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

void DecisionTree::BuildSubTree(Node* previous, bool is_left) {
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

DecisionTree::DecisionTree() {

  OpenFile();

  root = new Node(nullptr);
  BuildSubTree(root, true);
  root = root->left;

}

////////////////////////////////////////////////////////////////////////////////
///Storing DecisionTree in file

void DecisionTree::StoreTree() {
  assert(TREE_STORAGE_FILENAME != nullptr);
  TREE_STORAGE_FILE = fopen(TREE_STORAGE_FILENAME, "w");
  assert(TREE_STORAGE_FILE != nullptr);

  fprintf(TREE_STORAGE_FILE, "{\n");
  StoreNode(root);
  fprintf(TREE_STORAGE_FILE, "}");

  fclose(TREE_STORAGE_FILE);
}

void DecisionTree::StoreNode(Node* local) {
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
///Storing DecisionTree for visual representation with Graphviz

void DecisionTree::StoreTreeVis() {
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

void DecisionTree::StoreNodeVis(Node* local, size_t& node_index) {
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
///Getting definitions of the objects stored in DecisionTree

void DecisionTree::GetPath(Node* current, const char* object, Path* definition) {
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

void DecisionTree::Definition() {
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

////////////////////////////////////////////////////////////////////////////////
///Difference between object in DecisionTree

void DecisionTree::FeaturesOutput(size_t position, Path* definition) {
  assert(definition != nullptr);
  assert(position >= 0);
  assert(position <= definition->capacity);

  if (definition->path[position]) {
    printf(" %s", current->text);
    current = current->left;
  } else {
    printf(" не %s", current->text);
    current = current->right;
  }

  if (position != definition->capacity - 1) {
    printf(",");
  }
}


void DecisionTree::Difference() {
  assert(root != nullptr);

  Path first_def, second_def;
  char* first_name = nullptr;
  char* second_name = nullptr;

  printf("Укажите первый объект.\n");
  first_name = GetString();
  assert(first_name != nullptr);

  printf("Теперь второй объект.\n");
  second_name = GetString();
  assert(second_name != nullptr);

  printf("Сейчас посмотрим...\n");

  GetPath(root, first_name, &first_def);

  if (!first_def.correct) {
    printf("К сожалению, я не нашёл первый объект.\n");
    return;
  }

  GetPath(root, second_name, &second_def);

  if (!second_def.correct) {
    printf("К сожалению, я не нашёл второй объект.\n");
    return;
  }

  current = root;
  size_t size = 0, index = 0;

  if (first_def.capacity < second_def.capacity) {
    size = second_def.capacity;
  } else {
    size = first_def.capacity;
  }

  if (first_def.path[0] == second_def.path[0]) {
    printf("И для %s, и для %s верно, что объект", first_name,
           second_name);
  } else {
    printf("%s и %s не имеют никаких общих признаков", first_name,
           second_name);
  }

  while((index < size) && (first_def.path[index] == second_def.path[index])) {
    FeaturesOutput(index++, &first_def);
  }

  Node *difference_point = current;
  size_t difference_index = index;

  if (index < first_def.capacity) {
      printf("\nОднако для %s важно:", first_name);

      while (index < first_def.capacity) {
          FeaturesOutput(index++, &first_def);
      }
  }

  index = difference_index;

  if (index < second_def.capacity) {
      printf(".\nВ то время как для %s справедливо следующее:", second_name);
      current = difference_point;

      while (index < second_def.capacity) {
          FeaturesOutput(index++, &second_def);
      }
  }

  printf(".\n\n");
}