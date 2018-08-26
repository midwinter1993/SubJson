#ifndef __PARSSER_H__
#define __PARSSER_H__

typedef struct Tokenizer Tokenizer;

Tokenizer* tk_create(const char *str);
char tk_look_ahead(Tokenizer*);
void tk_assert_and_eat(Tokenizer*, char);
char* tk_next_string(Tokenizer*);
int tk_next_integer(Tokenizer*);


#endif /* ifndef __PARSSER_H__ */
