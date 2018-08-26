#include "token.h"

#include <ctype.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"

typedef struct Tokenizer {
    uint32_t curr_pos_;
    uint32_t buf_len_;
    char *buf_;
} Tokenizer;

static bool tk_is_eof(Tokenizer *tker) {
    return tker->curr_pos_ >= tker->buf_len_;
}

static char tk_curr_char(Tokenizer *tker) {
    assert(!tk_is_eof(tker));
    return tker->buf_[tker->curr_pos_];
}

static void tk_move_pos(Tokenizer *tker) {
    assert(!tk_is_eof(tker));
    tker->curr_pos_ += 1;
}

static void tk_skip_space(Tokenizer *tker) {
    while (!tk_is_eof(tker) && isspace(tk_curr_char(tker))) {
        tk_move_pos(tker);
    }
}

// ===============================================

Tokenizer* tk_create(const char *str) {
    Tokenizer *tker = NULL;
    NEW_INSTANCE(tker, Tokenizer);
    tker->curr_pos_ = 0;
    tker->buf_len_ = strlen(str);
    tker->buf_ = strdup(str);
    return tker;
}

char tk_look_ahead(Tokenizer *tker) {
    tk_skip_space(tker);

    if (tk_is_eof(tker)) {
        return 0;
    }
    return tk_curr_char(tker);
}

void tk_assert_and_eat(Tokenizer *tker, char ch) {
    tk_skip_space(tker);
    if (tk_curr_char(tker) != ch) {
        fprintf(stderr, "Expect: %c <=> Current: %c\n", ch, tk_curr_char(tker));
        assert(0);
    }
    tk_move_pos(tker);
}

char* tk_next_string(Tokenizer *tker) {
    tk_skip_space(tker);
    char *string_start = &(tker->buf_[tker->curr_pos_]);
    while (!tk_is_eof(tker) && tk_curr_char(tker) != '"') {
        tk_move_pos(tker);
    }
    char *string_end = &(tker->buf_[tker->curr_pos_]);

    uint32_t len = string_end - string_start;
    char *copy = (char*)malloc(len + 1);
    strncpy(copy, string_start, len);
    copy[len] = '\0';
    return copy;
}

int tk_next_integer(Tokenizer *tker) {
    tk_skip_space(tker);
    char *string_start = &(tker->buf_[tker->curr_pos_]);
    while (!tk_is_eof(tker) && isdigit(tk_curr_char(tker))) {
        tk_move_pos(tker);
    }
    char *string_end = &(tker->buf_[tker->curr_pos_]);

    uint32_t len = string_end - string_start;
    char *copy = (char*)malloc(len + 1);
    strncpy(copy, string_start, len);
    copy[len] = '\0';

    int ret = atoi(copy);
    free(copy);
    return ret;
}
