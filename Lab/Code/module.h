#ifndef __MODULE_H__
#define __MODULE_H__

#define MODULE(mod)                                                            \
  typedef struct mod_##mod##_t mod_##mod##_t;                                  \
  extern mod_##mod##_t *mod;                                                   \
  struct mod_##mod##_t

#define MODULE_DEF(mod)                                                        \
  extern mod_##mod##_t __##mod##_obj;                                          \
  mod_##mod##_t *mod = &__##mod##_obj;                                         \
  mod_##mod##_t __##mod##_obj

typedef union Attribute attribute_t;
typedef struct Ast ast_t;
MODULE(parser) {
  attribute_t (*get_attribute)(size_t);
  void (*print_ast_tree)();
  ast_t *(*get_ast_root)();
  void (*restart)(FILE *);
  int (*parse)();
  int (*lex_destroy)();
};

MODULE(analyzer) {
  void (*semantic_analysis)(ast_t *);
  void (*print_symbol_table)();
};

MODULE(mm) {
  void *(*log_malloc)(size_t);
  void (*clear_malloc)();
};

#endif
