#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
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

    KStream kstream = NULL;
    FILE *in = NULL;
    FILE *out = NULL;
    int exit_status = EXIT_FAILURE; 

    unsigned char in_buffer[BUFFER_SIZE];
    unsigned char out_buffer[BUFFER_SIZE];

    kstream = ks_create(key_file);
    if (kstream == NULL) {
        fprintf(stderr, "Error: Failed to create KStream from key file '%s'\n",
                key_file);
        goto cleanup;
    }

    in = fopen(in_file, "rb");
    if (in == NULL) {
        fprintf(stderr, "Error: Cannot open input file '%s': %s\n",
                in_file, strerror(errno));
        goto cleanup;
    }

    int output_to_stdout = (strcmp(out_spec, "-") == 0);
    
    if (output_to_stdout) {
        out = stdout;
    } else {
        out = fopen(out_spec, "wb");
        if (out == NULL) {
            fprintf(stderr, "Error: Cannot open output file '%s': %s\n",
                    out_spec, strerror(errno));
            goto cleanup;
        }
    }

    size_t bytes_read;
    while ((bytes_read = fread(in_buffer, 1, BUFFER_SIZE, in)) > 0) {
        
        ks_translate(kstream, in_buffer, out_buffer, bytes_read);

        if (output_to_stdout) {
            for (size_t i = 0; i < bytes_read; i++) {
                if (isascii(out_buffer[i])) {
                    printf("%c", out_buffer[i]);
                } else {
                    printf("%02x", out_buffer[i]);
                }
            }
        } else {
            size_t bytes_written = fwrite(out_buffer, 1, bytes_read, out);
            if (bytes_written != bytes_read) {
                fprintf(stderr, "Error: Failed to write to output file\n");
                goto cleanup;
            }
        }
    }

    if (ferror(in)) {
        fprintf(stderr, "Error: Failed while reading from input file\n");
        goto cleanup;
    }

    exit_status = EXIT_SUCCESS;

cleanup:
    if (kstream) {
        ks_destroy(kstream);
    }
    if (in) {
        fclose(in);
    }
    if (out && out != stdout) {
        fclose(out);
    }

    return exit_status;
}
