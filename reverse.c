#include <malloc.h>
#include <stdio.h>

#define LIST_SIZE 10
typedef struct Node {
  struct Node *next;
  int data;
} node;

node *reverseIterative(node *f);
node *createList(int num);
void dumpList(node *p);
void freeList(node *f);

int main() {
  node *head, *new_head;
  head = createList(LIST_SIZE);
  printf("ORIGINAL\n");
  dumpList(head);
  new_head = reverseIterative(head);
  /* reset original head for termination */
  head->next = NULL;
  printf("REVERSED\n");
  dumpList(new_head);
  freeList(new_head);
}

node *createList(int num) {
  int i = 0;
  node *p, *h, *prev;
  h = p = malloc(sizeof(node));
  if (h == NULL)
    return h;
  p->data = i;
  p->next = NULL;
  printf("HEAD %p\n", p);
  for (i = 1; i < num; i++) {
    prev = p;
    p = (node *)malloc(sizeof(node));
    if (p == NULL)
      return h;
    p->data = i;
    prev->next = p;
  }
  p->next = NULL;
  return (h);
}

void dumpList(node *f) {
  while (f != NULL) {
    printf("%s: %p:%d\n", __FUNCTION__, f, f->data);
    f = f->next;
  }
}

void freeList(node *f) {
  while (f != NULL) {
    free(f);
    f = f->next;
  }
}

node *reverseIterative(node *p) {
  node *cur, *left, *right;
  cur = p;
  left = p->next;
  right = left->next;
  while (cur != left) {
    left->next = cur;
    cur = left;
    if (right != NULL) {
      left = right;
      right = left->next;
    }
  }
  return (cur);
}