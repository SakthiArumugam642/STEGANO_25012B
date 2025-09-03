/*
Name: Sakthivel A
Reg : 25012B_093
Date: 01-09-2025
Desc: LSB image steganography in C
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr,0,SEEK_END);
    uint  size = ftell(fptr);
    rewind(fptr);
    return size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //  Step 1 : Check the argv[2] is having the .bmp extension or not
    // true - > Store into the structure member step 2 , false - > return e_failure
    // Step 2 : Check the argv[3] is having the .txt extension or not
    // true - > Store into the structure member step 3 , false - > return e_failure
    // Step 3 : Check argv[4] != NULL or not true - > Step 4, false -> Step 5
    // Step 4 : Check the argv[4] is having the .bmp extension or not
    // true - > Store into the structure member step 6 , false - > return e_failure
    // Step 5 : Store the default.bmp into the structure member - > Step 6
    // Step 6 : return e_success;
    int len = strlen(argv[2]);
    int sec_flag,src_flag,des_flag;
    sec_flag = src_flag = des_flag  = 0; // flags for the validation of arguments passed through the cla 

    if(strcmp(argv[2] + (len - 4), ".bmp") == 0){
        if(strcmp(argv[2]+(len-8),".bmp.bmp") == 0 ){
                printf("Error : Invalid file format, only one .bmp should be given\n");
                src_flag = 1;
            }
        encInfo->src_image_fname = argv[2];
    }
    else{
        printf("Invalid file format.. Expected format .bmp\n");
        return e_failure;
    }
    if (argv[3] != NULL) {
        int len = strlen(argv[3]);
        int dot_count2 = 0,i;
        for (i = 0; argv[3][i] != '\0'; i++) {
            if (argv[3][i] == '.')
                dot_count2++;
        }
        if (dot_count2 > 1) {
            printf("Error: Invalid file format, only one dot allowed\n");
            sec_flag = 1;
        }
            encInfo->secret_fname = argv[3];
    }

    if(argv[4] != NULL){
        len = strlen(argv[4]);   // recompute for argv[4]
        if(strcmp(argv[4] + (len - 4), ".bmp") == 0){
            if(strcmp(argv[4]+(len-8),".bmp.bmp") == 0 ){
                printf("Error : Invalid file format, only one .bmp should be given\n");
                des_flag = 1;
            }
            encInfo->stego_image_fname = argv[4];
        }
        else{
        printf("Invalid file format.. Expected format .bmp\n");
        return e_failure;
    }
    }else{
        encInfo->stego_image_fname = "default.bmp";
    }
    if(src_flag || sec_flag || des_flag) return e_failure;
    if(!strcmp( encInfo->stego_image_fname,encInfo->src_image_fname)){
        printf("Error :  Same file name given for encoding\n");
        return e_failure;    
    }
    printf("Read and validate arguments is success\n");
    return e_success;
}

Status open_files_encode(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // Step 1 :  Call get_image_size_for_bmp(encinfo->fptr_image) store into the encinfo->image_capacity
    // Step 2 : Call get_file_size(encinfo->fptr_secret) store into the encinfo-> size_secret_file;
    // Step 3 : encinfo->image_capacity > 16 + 32 + 32 + 32 + (encinfo-> size_secret_file * 8)
    //   true - return e_ succes , false - > e _ failure
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    printf("Secret file size : %ld\n",encInfo->size_secret_file);
    if(encInfo->image_capacity > (((strlen(MAGIC_STRING)+encInfo->size_secret_file+strlen(encInfo->extn_secret_file))*8)+64)) return e_success;
   
        printf("Insufficient capacity\n");
        return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // char ImageBuffer[54];
    // fseek(fptr_src_image, 0 , SEEK_SET)
    // fread(ImageBuffer , 54,1 , fptr_src_image)
    // fwrite(ImageBuffer, 54,1,fptr_dest_image)
    // return e_success;
    char imagebuffer[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(imagebuffer,54,1,fptr_src_image);
    fwrite(imagebuffer,54,1,fptr_dest_image);
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // char imageBuffer[8]
    // Step 1 : Generate the loop upto the magic_string size
    // Step 2 : Read the 8 bytes from src store into image buffer
    // Step 3 : Call the encode_byte_to_lsb(magic_string[i],imagebuffer)
    // Step 4 : Write the imageBuffer into destination file
    // Repeat the process upto the size
    // Return e_success
    char imagebuffer[8];
    for(int i=0;i<strlen(magic_string);i++){
        fread(imagebuffer,8,1,encInfo->fptr_src_image); // reading from src file and storing it in image buffer
        encode_byte_to_lsb(magic_string[i],imagebuffer);
        fwrite(imagebuffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    //  char imageBuffer[32];
    // Step 1 : Read the 32 bytes from src store into image buffer
    // Step 2 : Call the encode_size_to_lsb(size,imagebuffer)
    // Step 3 :  Write the imageBuffer into destination file
    // Return e_success
    char imagebuffer[32];
    fread(imagebuffer,32,1,encInfo->fptr_src_image); // reading from src file and storing it in image buffer
    encode_size_to_lsb(size,imagebuffer);
    // fwrite(encInfo->fptr_stego_image,32,1,imagebuffer); // writing bytes into dest file from image buffer
    fwrite(imagebuffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char imagebuffer[8];
    for(int i=0;i<strlen(file_extn);i++){
        fread(imagebuffer,8,1,encInfo->fptr_src_image); // reading from src file and storing it in image buffer
        encode_byte_to_lsb(file_extn[i],imagebuffer);
        // fwrite(encInfo->fptr_stego_image,32,1,imagebuffer);
        fwrite(imagebuffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char imagebuffer[32];
    fread(imagebuffer,32,1,encInfo->fptr_src_image); //  getting bytes from src image and storing it in image buffer
    encode_size_to_lsb(file_size,imagebuffer);
    // fwrite(encInfo->fptr_stego_image,32,1,imagebuffer); // writing bytes into dest file from image buffer
    fwrite(imagebuffer,32,1,encInfo->fptr_stego_image);
    return e_success;    
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
// Step 1: Rewind  the secret_fptr
    // Step 2 : Declare char secret_file_data[encInfo->size_secret_file]
    // Step 3 :fread(secret_file_data,encInfo->size_secret_file,1 ,secretfilefptr)
    // Step 4 : char imageBuffer[8]
    // Step 5 : Generate the loop upto the encInfo->size_secret_file
    // Step 6 : Read the 8 bytes from src store into image buffer
    // Step 7 : Call the encode_byte_to_lsb(secret_file_data[i],imagebuffer)
    // Step 8 : Write the imageBuffer into destination file
    // Repeat the process upto the size
    // Return e_success
    rewind(encInfo->fptr_secret);
    char secret_file_data[encInfo->size_secret_file];
    fread(secret_file_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    char imagebuffer[8];
    for(int i=0;i<encInfo->size_secret_file;i++){
        fread(imagebuffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(secret_file_data[i],imagebuffer);
        fwrite(imagebuffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // Copy the reamaining the data from src to dest
    int ch;
    while((ch = fgetc(fptr_src)) != EOF) {
        fputc(ch, fptr_dest);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *imagebuffer)
{
    // Step 1 Generate the loop from 0 to 7
    // Step 2 : Clear the lsb bit of imageBuffer[i]
    // Step 3 : Get the bit from data
    // Step 4 : Replace the get bit into lsb of imageBuffer[i]
    // Repeat the process up to the size
    // Return e_success;
    for(int i=0;i<8;i++){
        imagebuffer[i] = (imagebuffer[i] & ~1)| ((data>> (7-i))&1);
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imagebuffer)
{
    // Step 1 Generate the loop from 0 to 31
    // Step 2 : Clear the lsb bit of imageBuffer[i]
    // Step 3 : Get the bit from size
    // Step 4 : Replace the get bit into  the lsb of imageBuffer[i]
    // Repeat the process up to the size
    // Return e_success;
    for(int i=0;i<32;i++){
        imagebuffer[i] = (imagebuffer[i] & ~1)| ((size>>(31-i))&1);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    // Step 1 : Call the Open files(encinfo) true -> step 2 false - > return e_failure
    // Step 2 : Call the copy_bmp_header(); true - > step 3 false - > return e_failure
    // Step 3 : Call the  encode_magic_string()  true - > step 4 false - > return e_failure
    // Step 4 : Extract the extension from secret file name and store into the encinfo - > extn_size
    // Step 5 : Find the secret file extension size
    // Step 6: Call the encode_secret_file_extn_size(); true - step -> 7 ,false - > return e_failure
    // Step 8 : Call the encode_secret_file_extn_size(secret_file_extn) true - step -> 9 ,false - > return e_failure
    // Step 9 : Call the encode_secret_file_data_size() true - step -> 10 ,false - > return e_failure
    // Step 10:  Call the encode_secret_file_data()true - step -> 11 ,false - > return e_failure
    // Step 11 : Call the copy_remaining_img_data()
    // true - step -> e_success ,false - > return e_failure
    printf("----------Started encoding----------\n");
    if(open_files_encode(encInfo) == e_failure) return e_failure;
    printf("Open file is sucess\n");
    if(check_capacity(encInfo) == e_failure) return e_failure;
    printf("Check capacity is success\n");
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure) return e_failure;
    printf("Copy bmp header is success\n");
    printf("OFFSET at %ld before encoding magic string\n",ftell(encInfo->fptr_src_image));
    if(encode_magic_string(MAGIC_STRING,encInfo) == e_failure) return e_failure;
    printf("Encoding magic string is success\n");
    printf("OFFSET at %ld before encoding secret file extension size\n",ftell(encInfo->fptr_src_image));
    strcpy(encInfo->extn_secret_file,strchr(encInfo->secret_fname,'.'));
    long int secret_ext_size = strlen(strchr(encInfo->secret_fname,'.'));
    if(encode_secret_file_extn_size(secret_ext_size,encInfo) == e_failure) return e_failure;
    printf("Encoding secret file extension size is success\n");
    printf("OFFSET at %ld before encoding secret file extension\n",ftell(encInfo->fptr_src_image));
    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_failure) return e_failure;
    printf("Encoding secret file extension is success\n");
    printf("OFFSET at %ld before encoding secret file data size\n",ftell(encInfo->fptr_src_image));
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_failure) return e_failure;
    printf("Encoding secret file data size is success\n");
    printf("OFFSET at %ld before encoding secret file data\n",ftell(encInfo->fptr_src_image));
    if (encode_secret_file_data(encInfo) == e_failure) return e_failure;
    printf("Encoding secret file data is success\n");
    printf("OFFSET at %ld before copying remaining data of secret file\n",ftell(encInfo->fptr_src_image));
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure) return e_failure;
    printf("Encoding is success\n");
    return e_success;
}
