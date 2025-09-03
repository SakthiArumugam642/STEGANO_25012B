/*
Name: Sakthivel A
Reg : 25012B_093
Date: 01-09-2025
Desc: LSB image steganography in C
*/

#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Decode magic string
Status decode_magic_string(DecodeInfo *decInfo)
{
    char imagebuffer[8];
    char ch;
    for (int i = 0; i < strlen(MAGIC_STRING); i++)
    {
        fread(imagebuffer, 8, 1, decInfo->fptr_stego_image); // reading from stego image
        ch = decode_byte_from_lsb(imagebuffer);
        if (ch != MAGIC_STRING[i])
        {
            printf("Error : Magic string matching is failured\n");
            return e_failure;
        }
    }
    return e_success;
}

// Decode secret file extension size
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char imagebuffer[32];
    fread(imagebuffer, 32, 1, decInfo->fptr_stego_image);  // reading from stego image
    decInfo->secret_extn_size = decode_size_from_lsb(imagebuffer); // storing the returning value into secret file extn size
    return e_success;
}

// Decode secret file extension
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char imagebuffer[8];
    for (int i = 0; i < decInfo->secret_extn_size; i++)
    {
        fread(imagebuffer, 8, 1, decInfo->fptr_stego_image);  // reading from stego image
        decInfo->extn_secret_file[i] = decode_byte_from_lsb(imagebuffer); // storing the returning value into secret file extn
    }
    decInfo->extn_secret_file[decInfo->secret_extn_size] = '\0';
    strcat(decInfo->secret_file_name,decInfo->extn_secret_file); // concatenating secret file base name with extn  
    decInfo->secret_fname = decInfo->secret_file_name; // assigning secret file base name startng address
    return e_success;
}

// Decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char imagebuffer[32];
    fread(imagebuffer, 32, 1, decInfo->fptr_stego_image);  // reading from stego image
    decInfo->size_secret_file = decode_size_from_lsb(imagebuffer); // storing returning value into secret file size
    return e_success;
}

// Decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char imagebuffer[8];
    char secret_file_data;
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(imagebuffer, 8, 1, decInfo->fptr_stego_image);  // reading from stego image
        secret_file_data = decode_byte_from_lsb(imagebuffer); // storing returning value into one char var
        fputc(secret_file_data, decInfo->fptr_secret); // inserting the char var content into output file
    }
    return e_success;
}

// Decode one byte from LSB
char decode_byte_from_lsb(char *imagebuffer)
{
    char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = (ch << 1) | (imagebuffer[i] & 1);
    }
    return ch;
}

// Decode integer size from LSB
int decode_size_from_lsb(char *imagebuffer)
{
    int num = 0;
    for (int i = 0; i < 32; i++)
    {
        num = (num << 1) | (imagebuffer[i] & 1);
    }
    return num;
}

// opening files
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (!decInfo->fptr_stego_image){
         perror("fopen");
         fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
         return e_failure; 
        }

    return e_success;
}

Status open_files_output(DecodeInfo *decInfo)
{
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    if (!decInfo->fptr_secret){ 
        perror("fopen"); 
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
        return e_failure; 
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("----------Started decoding----------\n");
    if (open_files_decode(decInfo) == e_failure) return e_failure;
    printf("Open file is success\n");
    // Skip BMP header that was copied during encoding
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    if (decode_magic_string(decInfo) == e_failure) return e_failure;
    printf("Magic string Length and Magic string character matching is success\n");
    if (decode_secret_file_extn_size(decInfo) == e_failure) return e_failure;
    if (decode_secret_file_extn(decInfo) == e_failure) return e_failure;
    printf("Secret File extension and extension size reading is success\n");
    if (open_files_output(decInfo) == e_failure) return e_failure;
    printf("Opening output file is success\n");
    if (decode_secret_file_size(decInfo) == e_failure) return e_failure;
    printf("Secret File size decoding is success\n");
    printf("Decoding secret file's data...........\n");
    if (decode_secret_file_data(decInfo) == e_failure) return e_failure;
    printf("Decoding is success\n");
    return e_success;
}


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    int stego_flag,sec_flag;
    stego_flag = sec_flag = 0;    // flags for file format checking 
    // Validate 3rd argument (argv[2])
    if (argv[2] == NULL) {
        printf("Invalid input: no (.bmp) file for decoding\n");
        stego_flag = 1;
    }

    int len = strlen(argv[2]);

    // Must end with .bmp
    if (len < 4 || strcmp(argv[2] + (len - 4), ".bmp") != 0) {
        printf("Invalid file format.. Expected format .bmp\n");
        stego_flag = 1;
    }

    // Count number of dots in argv[2]
    int dot_count = 0;
    for (int i = 0; argv[2][i] != '\0'; i++) {
        if (argv[2][i] == '.')
            dot_count++;
    }
    if (dot_count > 1) {
        printf("Error: Invalid file format, only one dot allowed\n");
        stego_flag = 1;
    }

    decInfo->stego_image_fname = argv[2];

    // Validate 4th argument (argv[3])
    if (argv[3] != NULL) {
        int dot_count2 = 0;
        for (int i = 0; argv[3][i] != '\0'; i++) {
            if (argv[3][i] == '.')
                dot_count2++;
        }
        if (dot_count2 > 1) {
            printf("Error: Invalid file format, only one dot allowed\n");
            sec_flag = 1;
        }

        // Extract only the base name (before dot, if any)
        char *dot = strchr(argv[3], '.');
        if (dot) {
            int base_len = dot - argv[3];
            strncpy(decInfo->secret_file_name, argv[3], base_len);
            decInfo->secret_file_name[base_len] = '\0';
        } else {
            strcpy(decInfo->secret_file_name, argv[3]);  // no dot, take full string
        }

        decInfo->secret_fname = decInfo->secret_file_name;
    } else {
        // No arg passed, use default
        strcpy(decInfo->secret_file_name, "default");
        decInfo->secret_fname = decInfo->secret_file_name;
    }

    if(stego_flag || sec_flag) return e_failure;
    printf("Validate Arguments success\n");
    return e_success;
}
