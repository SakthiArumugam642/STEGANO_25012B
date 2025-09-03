/*
Name: Sakthivel A
Reg : 25012B_093
Date: 01-09-2025
Desc: LSB image steganography in C
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    if(argc < 3){
        printf("Invalid arguments passed\n");
         return 0;
    }
    if(argc < 4 && !strcmp(argv[1],"-e")){
         printf("Invalid arguments passed\n");
         return 0;
    }
    if(argc < 3 && !strcmp(argv[1],"-d")){ // if arg counts lesser than 4, then terminate program
        printf("Invalid arguments passed\n");
        return 0;
    }

    if(check_operation_type(argv[1]) == e_encode){
        printf("Now you have selected encoding\n");
        EncodeInfo enc_info;
        if(read_and_validate_encode_args(argv,&enc_info) == e_success){ // validating arguments
            do_encoding(&enc_info); // starts with encoding
        }else return 0;
    }else if (check_operation_type(argv[1]) == e_decode){
        printf("Now you have selected decoding\n");
        DecodeInfo dec_info;
        if(read_and_validate_decode_args(argv,&dec_info) == e_success){ // validating arguments
            do_decoding(&dec_info); // start decoding
        }else return 0;
    }else{
        printf("Unsupported options given\n");
        return 0; // termination of the program
    }
}

OperationType check_operation_type(char *symbol)
{
    if(strcmp("-e",symbol) == 0) return e_encode;  // if -e return encoding
    else if(strcmp("-d",symbol) == 0) return e_decode; // if -d return decoding
    else return e_unsupported; // else return unsupported 
}