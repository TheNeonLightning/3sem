#include <cassert>
#include <cstdio>
#include <cstring>

#include "decision_tree.h"

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
    printf("Выберете задачу: отгадывание/добавление (0) | определение (1) | разница (2) | завершение (3)\n");
    scanf("%d", &result);
    switch (result) {
    case (0):
      tree.Run();
      break;
    case (1):
      tree.Definition();
      break;
    case (2):
      tree.Difference();
      break;
    case (3):
      tree.StoreTree();
      tree.StoreTreeVis();
      return 0;
    default:
      printf("Не распознал команду, пожалуйста, "
             "введите номер команды (указана рядом в скобках)");
    }
  }
}