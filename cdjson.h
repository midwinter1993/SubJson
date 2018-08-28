#ifndef __CDJSON_H__
#define __CDJSON_H__

typedef struct Json Json;

Json* json_parse(const char*);
void json_dump(Json*);
void json_destroy(Json*);

int json_integer(Json *);
const char* json_string(Json *);
Json* json_map(Json*, const char*);
int json_map_size(Json*);
Json* json_array(Json*, int);
int json_array_size(Json*);


#endif /* ifndef __CDJSON_H__ */
