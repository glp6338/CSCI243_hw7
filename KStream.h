#ifndef KSTREAM_H
#define KSTREAM_H

#include <stdio.h>
#include <stdlib.h>

typedef struct KStream_s *KStream;

KStream ks_create(const char *key_filename);

void ks_destroy(KStream kstream);

void ks_translate(KStream kstream, const unsigned char *in_buffer, 
                  unsigned char *out_buffer, size_t num_bytes);

#endif // KSTREAM_H

