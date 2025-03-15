/*
Name : pruthvi jyoti
date : 15/10/2024
*/
/*
used cmd : 
encoding : [./a.out -d beautiful.bmp secret.txt ]
decoding : [ ./a.out -d stego.bmp ] 

check the image file head info :[ hd beuatiful.bmp | head ]

*/
#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <stdlib.h>
#include <unistd.h>  // Include for sleep()

#define MAGIC_STRING "#*"

// Validate decode arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->stego_image_fname = argv[2];
        if (argv[3] != NULL)
        {
            strtok(argv[3], ".");
            strcpy(decInfo->out_file_name, argv[3]);
        }
        else
        {
            strcpy(decInfo->out_file_name, "output");
        }
        return e_success;
    }
    return e_failure;
}

// Open stego file
Status open_files_dec(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (!decInfo->fptr_stego_image)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}

// Decode byte to LSB
char decode_byte_to_lsb(char *image_buffer)
{
    char decoded_byte = 0;
    for (int i = 0; i < 8; i++)
    {
        decoded_byte = decoded_byte | ((image_buffer[i] & 1) << (7 - i));
    }
    return decoded_byte;
}

// Decode image data
Status decode_image_to_data(char *str, int size, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, 1, 8, fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: Failed to read 8 bytes\n");
            return e_failure;
        }
        str[i] = decode_byte_to_lsb(image_buffer);
    }
    str[size] = '\0';
    return e_success;
}

// Decode magic string
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char str[strlen(MAGIC_STRING) + 1];  // +1 for null terminator
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    if (decode_image_to_data(str, strlen(MAGIC_STRING), decInfo->fptr_stego_image) != e_success)
    {
        return e_failure;  // Check for success of decoding
    }
    if (strcmp(str, MAGIC_STRING) != 0)
    {
        return e_failure;
    }
    return e_success;
}



// Decode extension size
Status decode_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decInfo->extn_size = decode_size_to_lsb(buffer);
    return e_success;
}


// Decode file extension
Status decode_secret_file_extn(int size, DecodeInfo *decInfo)
{
    char str_ext[size];  // Array to store required size data to decode
    decode_image_to_data(str_ext, size, decInfo->fptr_stego_image);  // Decode file extension
    strcat(decInfo->out_file_name, str_ext);  // Append extension to output file name

    // Open output file in write mode

    decInfo->fptr_out = fopen(decInfo->out_file_name, "w");
    if (decInfo->fptr_out == NULL)  // Check if the file opened successfully
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open output file %s\n", decInfo->out_file_name);
        return e_failure;
    }

    return e_success;
}



// Decode file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decInfo->size_secret_file = decode_size_to_lsb(buffer);
    return e_success;
}


Status decode_secret_file_data(const int size_data, DecodeInfo *decInfo)
{
    if (size_data <= 0)  // Check if size_data is valid
    {
        fprintf(stderr, "ERROR: Invalid size for secret file data: %d\n", size_data);
        return e_failure;
    }

    char str_data[size_data];  // Buffer to store the decoded data
   //  printf("DEBUG: Starting to decode secret file data with size %d\n", size_data);
    
    if (decode_image_to_data(str_data, size_data, decInfo->fptr_stego_image) != e_success)
    {
        fprintf(stderr, "ERROR: Failed to decode image data\n");
        return e_failure;
    }

    if (fwrite(str_data, size_data, 1, decInfo->fptr_out) != 1)  // Write decoded data to output file
    {
        fprintf(stderr, "ERROR: Failed to write to output file\n");
        return e_failure;
    }
    
    //   printf("DEBUG: Successfully decoded secret file data\n");
   return e_success;
}





// Decode size using LSBs
int decode_size_to_lsb(char *buffer)
{
    int val = 0;
    for (int i = 0; i < 32; i++)
    {
        val |= ((buffer[i] & 1) << (31 - i));
    }
    return val;
}

// Close files
Status close_files_dec(DecodeInfo *decInfo)
{
    if (decInfo->fptr_stego_image) fclose(decInfo->fptr_stego_image);
    if (decInfo->fptr_out) fclose(decInfo->fptr_out);
    return e_success;
}

Status do_decoding(char *argv[], DecodeInfo *decInfo)//funtion for decoding
{ 
	sleep(1); // sleep for 1 second
	if(open_files_dec(decInfo) == e_success)//open stego file
	{
		sleep(1);
		printf(GREEN"INFO1 : FILES OPENED SUCCESSFULL...\n"RESET);
		if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)//docode magic string
		{
			sleep(1);
			printf(GREEN"INFO2 : MAGIC STRING DECODE SUCCESSFULL...\n"RESET);
			if(decode_extn_size(decInfo) == e_success)//decode extension size
			{      
				sleep(1); 
				printf(GREEN"INFO3 : SIZE DECODE SUCCESSFULL...\n"RESET);
				if(decode_secret_file_extn(4, decInfo) == e_success)//decode secret file extension
				{
					sleep(1);
					printf(GREEN"INFO4 : OUTPUT FILE EXTENSION DECODE SUCCESSFULL...\n"RESET);
					if(decode_secret_file_size(decInfo) == e_success)//decode secret file size
					{       
						sleep(1);
						printf(GREEN"INFO5 : SECRET FILE SIZE DECODED SUCCESSFULLY...\n"RESET);
						if(decode_secret_file_data(25, decInfo) == e_success)//decode secret file data
						{ 
						
							sleep(1);
							printf(GREEN"INFO6 : SECRET DATA DECODED SUCCESSFULLY...\n"RESET);
							return e_success;
						}
						else
						{
							printf(RED"SECRET DATA DECODE FAIL\n"RESET);
							return e_failure;
						}
					}
					else
					{
						printf(RED"SECRET FILE SIZE DECODE FAIL\n"RESET);
					}
				}
				else
				{
					printf(RED"EXTENSION DECODE FAIL\n"RESET);
				}
			}
			else
			{
				printf(RED"SIZE DECODE FAILURE\n"RESET);
			}

		}
		else
		{
			printf(RED"MAGIC STRING DECODE FAILURE\n"RESET);
		}

	}
}


