/*
Name: Sakthivel A
Reg : 25012B_093
Date: 01-09-2025
Desc: LSB image steganography in C
*/

#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h"

// Structure to store information required for decoding
typedef struct _DecodeInfo
{
    // Stego Image Info
    char *stego_image_fname; // To store the stego image name
    FILE *fptr_stego_image;  // To store the address of stego image

    // Secret File Info
    char *secret_fname;       // To store the secret file name (output)
    FILE *fptr_secret;        // To store the secret file address
    long secret_extn_size;    // To store secret extn size
    char secret_file_name[50];   // To store secret file base name
    char extn_secret_file[7]; // To store the Secret file extension
    long size_secret_file;    // To store the size of the secret data

} DecodeInfo;

// Function prototypes for decoding
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);
Status open_files_decode(DecodeInfo *decInfo);
Status open_files_output(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
char decode_byte_from_lsb(char *imagebuffer);
int decode_size_from_lsb(char *imagebuffer);

#endif
