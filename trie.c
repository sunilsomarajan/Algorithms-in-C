#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/** ************************************************************  */
/** implement a trie data structure for fast word autocompletion  */
/** ************************************************************  */

#define debug_print(...)                                                       \
  {                                                                            \
    do {                                                                       \
      printf("[%s:%s %d]", __FILE__, __func__, __LINE__);                      \
      printf(__VA_ARGS__);                                                     \
    } while (0);                                                               \
  }

/* By C.B. Falconer.  Public domain 2002-06-22            */
/*    attribution appreciated.                            */

#define INITSIZE 50 /* power of 2 minus 16, helps malloc */
#define DELTASIZE (INITSIZE + 16)

enum { OK = 0, NOMEM };

int fggets(char **ln, FILE *f) {
  int cursize, ch, ix;
  char *buffer, *temp;

  *ln = NULL; /* default */
  if (NULL == (buffer = malloc(INITSIZE)))
    return NOMEM;
  cursize = INITSIZE;

  ix = 0;
  while ((EOF != (ch = getc(f))) && ('\n' != ch)) {
    if (ix >= (cursize - 1)) { /* extend buffer */
      cursize += DELTASIZE;
      if (NULL == (temp = realloc(buffer, (size_t)cursize))) {
        /* ran out of memory, return partial line */
        buffer[ix] = '\0';
        *ln = buffer;
        return NOMEM;
      }
      buffer = temp;
    }
    buffer[ix++] = ch;
  }
  if ((EOF == ch) && (0 == ix)) {
    free(buffer);
    return EOF;
  }

  buffer[ix] = '\0';
  if (NULL == (temp = realloc(buffer, (size_t)ix + 1))) {
    *ln = buffer; /* without reducing it */
  } else
    *ln = temp;
  return OK;
}

/* English alphabet only */
#define R_SIZE 26
typedef struct Trienode_T {
  struct Trienode_T *next[R_SIZE];
  int end_of_key;
} trie_node;

typedef struct TrieT {
  trie_node *root;
  int number_of_nodes;
} trie;

trie_node *newTrieNode() {
  trie_node *tr = NULL;
  tr = malloc(sizeof(trie_node));
  if (tr) {
    tr->end_of_key = 0;
    memset(tr->next, 0, sizeof(tr->next));
  }
  return tr;
}

/**
 * Insert a key into the trie. Note that we never store the key itself since
 * this algorithm only cares about prefix completion. The index of the next
 * pointer in t->next determines the character that is being referenced
 */
trie_node *trieInsert(trie *t, trie_node *node, char *key, int len) {
  if (node == NULL) {
    node = newTrieNode();
    if (node == NULL) {
      debug_print("Could not create node, abort !\n");
    }
  }
  /** ignore the new line as fgets includes the newline in the string hence the
   * strlen needs to be reduced by 1 as seen below
   */
  if (len == strlen(key) - 1) {
    if (node->end_of_key == 0)
      t->number_of_nodes++;
    /* we are done adding the entire string */
    node->end_of_key = 1;
    return node;
  }
  int index = (key[len] % 32) - 1;
  node->next[index] = trieInsert(t, node->next[index], key, len + 1);
  return node;
}

void deleteChar(char *str, unsigned int index) {
  char *src;
  for (src = str + index; *src != '\0'; *src = *(src + 1), ++src)
    ;
  *src = '\0';
}

trie_node *get_root_of_prefix(trie_node *node, char *prefix, int len) {
  if (node == NULL) {
    return NULL;
  }
  if (len == strlen(prefix)) {
    return node;
  }
  int index = (prefix[len] % 32) - 1;
  return get_root_of_prefix(node->next[index], prefix, len + 1);
}

void freeTrieNodes(trie_node *node) {
  if (node == NULL) {
    return;
  }
  for (char c = 'a'; c <= 'z'; c++) {
    int index = (c % 32) - 1;
    freeTrieNodes(node->next[index] == NULL ? NULL : node->next[index]);
  }
  free(node);
}

void get_all_prefixes(trie_node *node, char *prefix, char *prefix_array) {
  if (node == NULL) {
    return;
  }
  if (node->end_of_key == 1) {
    printf("                  . (%s)\n", prefix);
  }
  for (char c = 'a'; c <= 'z'; c++) {
    strncat(prefix, &c, 1);
    int index = (c % 32) - 1;
    get_all_prefixes(node->next[index] == NULL ? NULL : node->next[index],
                     prefix, prefix_array);
    deleteChar(prefix, strlen(prefix) - 1);
  }
}
trie_node *trieFindPrefixes(trie_node *node, char *prefix) {
  char prefix_array[256];
  char prefix_copy[256];
  strncpy(prefix_copy, prefix, sizeof(prefix));
  node = get_root_of_prefix(node, prefix, 0);
  get_all_prefixes(node, prefix_copy, prefix_array);
}

int main() {
  trie *t;
  trie_node *p;
  t = (trie *)malloc(sizeof(trie));
  if (t) {
    t->root = NULL;
    t->number_of_nodes = 0;
    // lets add some keys to the trie we just created
    printf("creating the 10000 word trie\n");
    printf("............................\n\n");

    char str[100];
    int mem = 0;
    FILE *f = fopen("10000.txt", "r");
    t->root = newTrieNode();
    if (f && t->root) {
      if (f) {
        /* fgets will return a partial line if length > 100 */
        while (fgets(str, 100, f)) {
          mem += strlen(str);
          trieInsert(t, t->root, str, 0);
        };
      }
      printf("total memory required for building the 10,000 word trie = [%d] "
             "bytes\n",
             mem);

      char *prefix;
      printf("enter an autocompletion prefix (empty string to dump all keys in "
             "the trie)\n");
      if (fggets(&prefix, stdin) != NOMEM) {
        printf("autocompletions for [%s]\n",
               strlen(prefix) > 1 ? prefix : "empty string");
        trieFindPrefixes(t->root, prefix);
        free(prefix);
      }
      printf("freeing the 10000 word trie\n");
      printf("...........................\n\n");
      freeTrieNodes(t->root);
      free(t);
      fclose(f);
    }
  }
}