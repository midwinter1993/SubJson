#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "common.h"
#include "cdjson.h"
#include "token.h"


/* typedef struct Json Json; */ // define in cdjson.h
typedef struct Map Map;
typedef struct Array Array;
typedef struct String String;
typedef struct Integer Integer;

typedef enum Type {
    MAP,
    STRING,
    ARRAY,
    INTEGER
} Type;

Json* json_as_json(void*);
Map* json_as_map(Json*);
Array* json_as_array(Json*);
String* json_as_string(Json*);
Integer* json_as_integer(Json*);

Json* json_as_json(void *obj) {
    assert(obj);
    return (Json*)obj;
}

Type json_type(Json *obj) {
    assert(obj);
    return *(Type*)obj;
}

Map* json_as_map(Json *obj) {
    assert(json_type(obj) == MAP);
    return (Map*)obj;
}

Array* json_as_array(Json *obj) {
    assert(json_type(obj) == ARRAY);
    return (Array*)obj;
}

String* json_as_string(Json *obj) {
    assert(json_type(obj) == STRING);
    return (String*)obj;
}

Integer* json_as_integer(Json *obj) {
    assert(json_type(obj) == INTEGER);
    return (Integer*)obj;
}

// ===============================================

typedef struct Map {
    Type type_;
    uint32_t nr_;
    uint32_t cap_;
    char **keys_;
    Json **values_;
} Map;

static Map* map_create() {
    Map *map = NULL;
    NEW_INSTANCE(map, Map);
    map->type_ = MAP;
    map->nr_ = 0;
    map->cap_ = 10;
    NEW_ARRAY(map->keys_, char*, map->cap_);
    NEW_ARRAY(map->values_, Json*, map->cap_);
    return map;
}

static void map_destroy(Map *map) {
    assert(map);

    for (size_t i = 0; i < map->nr_; ++i) {
        free(map->keys_[i]);
        json_destroy(map->values_[i]);
    }
    free(map->keys_);
    free(map->values_);
}

static void map_put(Map *map, char *key, Json *value) {
    assert(map->nr_ < map->cap_);
    map->keys_[map->nr_] = key;
    map->values_[map->nr_] = value;
    map->nr_ += 1;
}

static Json* map_get(Map *map, const char *key) {
    for (uint32_t i = 0; i < map->nr_; ++i) {
        if (strcmp(key, map->keys_[i]) == 0) {
            return map->values_[i];
        }
    }
    return NULL;
}

// -----------------------------------------------

typedef struct Array {
    Type type_;
    uint32_t nr_;
    uint32_t cap_;
    Json **values_;
} Array;

static Array* array_create() {
    Array *arr = NULL;
    NEW_INSTANCE(arr, Array);
    arr->type_ = ARRAY;
    arr->nr_ = 0;
    arr->cap_ = 10;
    NEW_ARRAY(arr->values_, Json*, arr->cap_);
    return arr;
}

static void array_destroy(Array *arr) {
    assert(arr);

    for (size_t i = 0; i < arr->nr_; ++i) {
        json_destroy(arr->values_[i]);
    }
    free(arr->values_);
}

static void array_put(Array *arr, Json *value) {
    assert(arr->nr_ < arr->cap_);
    arr->values_[arr->nr_] = value;
    arr->nr_ += 1;
}

static Json* array_get(Array *arr, int pos) {
    assert(pos < arr->nr_);
    return arr->values_[pos];
}

// -----------------------------------------------

typedef struct String {
    Type type_;
    char *value_;
} String;

static String* string_create(char *value) {
    String *str = NULL;
    NEW_INSTANCE(str, String);
    str->type_ = STRING;
    str->value_ = value;
    return str;
}

static void string_destroy(String *str) {
    assert(str);

    free(str->value_);
}

// -----------------------------------------------

typedef struct Integer {
    Type type_;
    int value_;
} Integer;


static Integer* integer_create(int value) {
    Integer *i = NULL;
    NEW_INSTANCE(i, Integer);
    i->type_ = INTEGER;
    i->value_ = value;
    return i;
}

// ===============================================

static Json* json_parse_json(Tokenizer *tker);
static Map* json_parse_map(Tokenizer *tker);
static Array* json_parse_array(Tokenizer *tker);
static String* json_parse_string(Tokenizer *tker);
static Integer* json_parse_integer(Tokenizer *tker);

static Json* json_parse_json(Tokenizer *tker) {
    char ch = tk_look_ahead(tker);
    if (ch == '{') {
        return json_as_json(json_parse_map(tker));
    } else if (ch == '"') {
        return json_as_json(json_parse_string(tker));
    } else if (ch == '[') {
        return json_as_json(json_parse_array(tker));
    } else {
        assert(isdigit(ch));
        return json_as_json(json_parse_integer(tker));
    }
    NOT_REACH_HERE();
    return NULL;
}

static Map* json_parse_map(Tokenizer *tker) {
    tk_assert_and_eat(tker, '{');

    Map *obj = map_create();

    while (tk_look_ahead(tker) != '}') {
        tk_assert_and_eat(tker, '"');
        char *key = tk_next_string(tker);
        tk_assert_and_eat(tker, '"');

        tk_assert_and_eat(tker, ':');

        Json *value = json_parse_json(tker);
        map_put(obj, key, value);

        // NEED: fix for json spec
        if (tk_look_ahead(tker) == ',') {
            tk_assert_and_eat(tker, ',');
        }
    }

    tk_assert_and_eat(tker, '}');

    return obj;
}

static Array* json_parse_array(Tokenizer *tker) {
    tk_assert_and_eat(tker, '[');

    Array *obj = array_create();

    while (tk_look_ahead(tker) != ']') {
        Json *value = json_parse_json(tker);
        array_put(obj, value);

        // NEED: fix for json spec
        if (tk_look_ahead(tker) == ',') {
            tk_assert_and_eat(tker, ',');
        }
    }

    tk_assert_and_eat(tker, ']');

    return obj;

}

static String* json_parse_string(Tokenizer *tker) {
    tk_assert_and_eat(tker, '"');
    char *value = tk_next_string(tker);
    tk_assert_and_eat(tker, '"');
    return string_create(value);
}

static Integer* json_parse_integer(Tokenizer *tker) {
    int value = tk_next_integer(tker);
    Integer *integer = integer_create(value);
    return integer;
}

// ===============================================

Json* json_parse(const char *str) {
    Tokenizer *tker = tk_create(str);
    assert(tker);
    Json *json = json_parse_json(tker);
    assert(json);
    return json;
}

void json_dump(Json *obj) {
    switch (json_type(obj)) {
        case MAP: {
        Map *map = json_as_map(obj);
            fprintf(stderr, "{ ");
            for (uint32_t i = 0; i < map->nr_; ++i) {
                fprintf(stderr, "\"%s\": ", map->keys_[i]);
                json_dump(map->values_[i]);
                fprintf(stderr, ",\n");
            }
            fprintf(stderr, " }");
            break;
        }
        case ARRAY: {
            fprintf(stderr, "[");
            Array *arr = json_as_array(obj);
            for (uint32_t i = 0; i < arr->nr_; ++i) {
                json_dump(arr->values_[i]);
                fprintf(stderr, ", ");
            }
            fprintf(stderr, " ]");
            break;
        }
        case STRING:
            fprintf(stderr, "\"%s\"", json_as_string(obj)->value_);
            break;
        case INTEGER:
            fprintf(stderr, "%d", json_as_integer(obj)->value_);
            break;
        default:
            NOT_REACH_HERE();
    }
}

void json_destroy(Json *obj) {
    assert(obj);

    switch (json_type(obj)) {
        case MAP: {
            map_destroy(json_as_map(obj));
            break;
        }
        case ARRAY: {
            array_destroy(json_as_array(obj));
            break;
        }
        case STRING:
            string_destroy(json_as_string(obj));
            break;
        case INTEGER:
            break;
        default:
            NOT_REACH_HERE();
    }
    free(obj);
}

// ===============================================

int json_ingeger(Json *obj) {
    return json_as_integer(obj)->value_;
}

const char* json_string(Json *obj) {
    return json_as_string(obj)->value_;
}

Json* json_map(Json *obj, const char* key) {
    return map_get(json_as_map(obj), key);
}

int json_map_size(Json *obj) {
    return json_as_map(obj)->nr_;
}

Json* json_array(Json *obj, int pos) {
    return array_get(json_as_array(obj), pos);
}

int json_array_size(Json *obj) {
    return json_as_array(obj)->nr_;
}
