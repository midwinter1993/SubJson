#include <stdio.h>
#include <string.h>

#include "common.h"
#include "token.h"
#include "cdjson.h"

static char* load_file(const char *filename) {
    FILE *fp = fopen(filename, "r");

    if (fp) {
        fseek (fp, 0, SEEK_END);
        long length = ftell (fp);
        fseek (fp, 0, SEEK_SET);

        char *buf = NULL;
        NEW_STRING(buf, length);
        fread(buf, 1, length, fp);

        fclose(fp);
        return buf;
    } else {
        DBG_PRINTF("Open file failure [ %s ]\n", filename);
        exit(EXIT_FAILURE);
    }
    return NULL;
}

int main() {
    char *str = load_file("./data.json");
    /* printf("%s\n", str); */

    Json *json = json_parse(str);
    json_dump(json);

    const char *s = json_string(json_array(json_map(json, "dddd"), 1));
    printf("%s\n", s);

    json_destroy(json);

    return 0;
}
