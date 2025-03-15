/*
Name : pruthvi jyoti
date : 15/10/2024
*/


#include <stdio.h>
#include "encode.h"    // Make sure this contains EncodeInfo and encoding functions
#include "decode.h"    // For DecodeInfo and decoding functions
#include "types.h"     // User-defined types
#include "common.h"    // For check_operation_type

int main(int argc, char *argv[])
{
    if (argc >= 3)
    {
        int val = check_operation_type(argv);
        if (val == e_encode)
        {
            printf(YELLOW"ENCODE OPERATION\n"RESET);
            EncodeInfo encInfo;  // Declare EncodeInfo
            if (read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                printf(GREEN"ENCODE ARGUMENTS ARE VALID\n"RESET);
                if (do_encoding(&encInfo) == e_success)
                {
                    printf(GREEN"ENCODE SUCCESSFUL...!\n"RESET);
                }
                else
                {
                    printf(RED"ENCODE FAILED...!\n"RESET);
                }
            }
            else
            {
                printf(RED"ENCODE ARGUMENTS ARE NOT VALID\n"RESET);
            }
        }
        else if (val == e_decode)
        {
            printf(YELLOW"DECODE OPERATION \n"RESET);
            DecodeInfo decInfo;  // Declare DecodeInfo 
            if (read_and_validate_decode_args(argv, &decInfo) == e_success)
            {
                if (do_decoding(argv, &decInfo) == e_success)
                {
                    printf(GREEN"DECODE SUCCESSFUL...!\n"RESET);
                }
                else
                {
                    printf(RED"DECODE FAILED..!\n"RESET);
                }
            }
            else
            {
                printf(RED"DECODE ARGUMENTS ARE NOT VALID\n"RESET);
            }
        }
        else
        {
            printf(RED"OPERATION TYPE UNSUPPORTED\n"RESET);
        }
    }
    else
    {
        printf(RED"INVALID ARGUMENTS\n"RESET);
    }
    return 0;
}
