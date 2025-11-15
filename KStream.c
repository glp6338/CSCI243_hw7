#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "KStream.h"

#define KEY_LENGTH 8
#define STATE_SIZE 256
#define PRIME_DISCARD 1024

struct KStream_s {
    unsigned char S[STATE_SIZE];
    unsigned char i;
    unsigned char j;
};

static void swap(unsigned char *a, unsigned char *b) {
    unsigned char temp = *a;
    *a = *b;
    *b = temp;
}

static unsigned char next_byte(KStream kstream) {
    kstream->i = (kstream->i + 1);
    kstream->j = (kstream->j + kstream->S[kstream->i]);

    swap(&kstream->S[kstream->i], &kstream->S[kstream->j]);
    
    unsigned char B = kstream->S[(kstream->S[kstream->i] + 
                                  kstream->S[kstream->j])];
    return B;
}

KStream ks_create(const char *key_filename) {
    unsigned char key[KEY_LENGTH];
    FILE *key_file = fopen(key_filename, "rb");
    if (key_file == NULL) {
        return NULL;
    }

    size_t bytes_read = fread(key, 1, KEY_LENGTH, key_file);
    
    (void)fgetc(key_file); 
    
    if (bytes_read != KEY_LENGTH || !feof(key_file)) {
        fprintf(stderr, "Error: Key file '%s' must be exactly %d bytes.\n",
                key_filename, KEY_LENGTH);
        fclose(key_file);
        return NULL;
    }
    fclose(key_file);

    KStream kstream = malloc(sizeof(struct KStream_s));
    if (kstream == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for KStream.\n");
        return NULL;
    }

    kstream->i = 0;
    kstream->j = 0;
    for (int i = 0; i < STATE_SIZE; i++) {
        kstream->S[i] = i;
    }

    unsigned char j = 0;
    for (int i = 0; i < STATE_SIZE; i++) {
        j = (j + kstream->S[i] + key[i % KEY_LENGTH]);
        swap(&kstream->S[i], &kstream->S[j]);
    }
    
    for (int n = 0; n < PRIME_DISCARD; n++) {
        (void)next_byte(kstream);
    }

    return kstream;
}

void ks_destroy(KStream kstream) {
    free(kstream);
}

void ks_translate(KStream kstream, const unsigned char *in_buffer, 
                  unsigned char *out_buffer, size_t num_bytes) {
    
    for (size_t n = 0; n < num_bytes; n++) {
        unsigned char key_byte = next_byte(kstream);
        out_buffer[n] = in_buffer[n] ^ key_byte;
    }
}
