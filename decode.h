#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>  // Add this for FILE type
#include "types.h"  // Contains user-defined types

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;  // For stego image
    char out_file_name[50];  // Output file name
    char extn_secret_file[10];
    int extn_size;
    long size_secret_file;
    FILE *fptr_out;          // For output file
} DecodeInfo;

// Function prototypes
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status open_files_dec(DecodeInfo *decInfo);
Status decode_image_to_data(char *str, int size, FILE *fptr_stego_image);
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);
Status decode_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(int size, DecodeInfo *decInfo);
int decode_size_to_lsb(char *buffer);
Status do_decoding(char *argv[], DecodeInfo *decInfo);  // Ensure this is declared

#endif
