#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "KStream.h"

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "usage: mcrypt key-file in-file [ out-file | - ]\n");
        return EXIT_FAILURE;
    }

    const char *key_file = argv[1];
    const char *in_file = argv[2];
    const char *out_spec = argv[3];

    printf("Arguments OK. Key: %s, In: %s, Out: %s\n", 
           key_file, in_file, out_spec);
           
    return EXIT_SUCCESS;
}
