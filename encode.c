/*
Name : pruthvi jyoti
date : 15/10/2024 
*/
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>
#include <unistd.h>  // Include for sleep()

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
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
uint get_file_size(FILE *fptr)//funtion to get file size
{
		fseek(fptr, 0, SEEK_END);
		//printf("file size %ld\n", ftell(fptr));
		return ftell(fptr);//return position of file pointer using ftell
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s\n"RESET, encInfo->src_image_fname);

      	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s\n"RESET, encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s\n"RESET, encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
OperationType check_operation_type(char *argv[])//funtion to check wich operation
{
		if(strcmp(argv[1], "-e") == 0)//check operation is -e using strcmp
		{
				return e_encode;
		}
		else if(strcmp(argv[1], "-d") == 0)//check operation is -d using strcmp
		{
				return e_decode;
		}
		else
		{
				return e_unsupported;//if not encode or deocode then return unsupported
		}
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)//funtion to validate encode arguments
{
		if(strstr(argv[2], ".bmp") != NULL)//check argv[2] is .bmp or not
		{
				encInfo->src_image_fname = argv[2];//store source file name into pointer
	            if(argv[3] != NULL)
				{
						if(strstr(argv[3], ".txt") != NULL || strstr(argv[3], ".c") != NULL || strstr(argv[3], ".sh") != NULL)//check argv[3] is .txt or .c or .sh
						{
								encInfo->secret_fname = argv[3];//store secret file name into pointer
							    strcpy(encInfo->extn_secret_file, strstr(argv[3], "."));//store extension of secret file to array
					        
						        if(argv[4] == NULL)//check if argv[4] is entered or not
						        {
						        		encInfo->stego_image_fname="stego.bmp";//if not then store stego.bmp in pointer
								        return e_success;
						        }
						        else
						        {
						        		if(strstr(argv[4], ".bmp") != NULL)//if argv[4] is entered the check it is .bmp or not
								        {
								        		encInfo->stego_image_fname = argv[4];//store file name to pointer
								                return e_success;
								        }
								        else
								        {
								        		return e_failure;
								        }
						        }  

				        }
				        else
				        {
				        		return e_failure;
				        }
				}
				else
				{
						return e_failure;
				}
		}
		else
		{
				return e_failure;
		}
}
Status check_capacity(EncodeInfo *encInfo)//funtion to check capacity of file to store encoded data
{
		int val = 54 + (strlen(MAGIC_STRING) + 4 + strlen(strstr(encInfo->secret_fname, ".")) + 4 + get_file_size(encInfo->fptr_secret) * 8);//total data needed for encode
		if(val < get_image_size_for_bmp(encInfo->fptr_src_image))//if val is less than file size then return success
		{
				return e_success;
		}
	
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)//fun to copy src header to dest file
{
		fseek(fptr_src_image,0,SEEK_SET);//set file pointer at begining
		char buffer[54];
		fread(buffer, 54, 1, fptr_src_image);//read 54 byte of header into from source file
		fwrite(buffer, 54, 1, fptr_dest_image);//wrote 54 byte of header to dest file
		return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)//funtion to encode byte lsb
{
		int i;
		for(i = 0; i < 8; i++)//run loop to encode 8 bits into lsb of 8 bytes
		{
				image_buffer[i] = (image_buffer[i] & ~1) | ((data & (1 << 7 - i)) >> 7 - i);//bitwise operation to clear lsb bit of src data
																							//get msb bit of byte of info data
																							//replace msb bit to lsb bit of src data
		}
}
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)//funtion to encode data to image
{
		char image_buffer[8];//declare one buffer array to store 8 byte of data
		int i;
		for(i = 0; i < size; i++)//loop to encode size number of bytes
		{
				fread(image_buffer, 8, 1, fptr_src_image);//read 8 bytes from src file
				encode_byte_to_lsb(data[i], image_buffer);//funtion call to encode data into 8 bytes of src
				fwrite(image_buffer, 8, 1, fptr_stego_image);//write encoded data to dest file
		}
		return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)//funtion call to encode magic string #*
{
		encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image);
		return e_success;
}
Status encode_size_to_lsb(int size, char *image_buffer)//funtion to encode size to lsb
{
		int i;
		for(i = 0; i < 32; i++)//encode 4 byte of data into 32 bytes of src file
		{
				image_buffer[i] = (image_buffer[i] & ~1) | (((unsigned)(size & (1 << 31 - i))) >> 31 - i);
		}
		return e_success;
}
Status encode_extn_size(int size, EncodeInfo *encInfo)//funtion to encode extension size
{
		char image_buffer[32];//array to store 32 byte of data to encode 4 byte of data
		fread(image_buffer, 32, 1, encInfo->fptr_src_image);//read 32 bytes from src file
		encode_size_to_lsb(size, image_buffer);//do encode
		fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);//write 32 bytes to dest file
		return e_success;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)//funtion to encode secret file extension
{
		 encode_data_to_image(encInfo->extn_secret_file, strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image);//pass extension stored in arr, length of extesion and src , dest file 
		 return e_success;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)//funtion to encode secret file size
{
		char secret_buffer[32];//array to store 32 byte of data from scr file
		fread(secret_buffer, 32, 1, encInfo->fptr_src_image);//read 32 byte of data from src file
		encode_size_to_lsb(file_size, secret_buffer);//do encoding
		fwrite(secret_buffer, 32, 1, encInfo->fptr_stego_image);//write encoded data to dest file
		return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)//
{
		encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	//	printf("val = %d\n", val);
	    char secret_buffer[encInfo->size_secret_file];
	    rewind(encInfo->fptr_secret);
	   // printf("fptr at = %ld\n", ftell(encInfo->fptr_secret));
		fread(secret_buffer, encInfo->size_secret_file, 1, encInfo->fptr_secret);
		encode_data_to_image(secret_buffer, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);
        return e_success;

}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)//funtion to copy remaining data from src file to dest file
{
		char ch;
		while(fread(&ch,1,1,fptr_src) != 0 )//read and write data byte by byte
		{
			fwrite(&ch,1,1,fptr_dest);
		}
		return e_success;
}

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo) {
    printf(GREEN"INFO1 :OPEN THE FILES GETS SUCCESSFUL...\n"RESET);
    sleep(1); // sleep for 1 second

    if (open_files(encInfo) == e_success) {
        printf(GREEN"INFO2 : CHECK THE CAPACITY GETS SUCCESSFUL...\n"RESET);
        sleep(1);

        if (check_capacity(encInfo) == e_success) {
            printf(GREEN"INFO3 :CHECK THE BMP HEADER GETS SUCCESSFUL...\n"RESET);
            sleep(1);

            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) {
                printf(GREEN"INFO4 :CHECK THE MAGIC STRING GETS SUCCESSFUL...\n"RESET);
                sleep(1);

                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success) {
                    printf(GREEN"INFO5 :CHECK THE EXTENTION SIZE SUCCESSFUL...\n"RESET);
                    sleep(1);

                    if (encode_extn_size(strlen(strstr(encInfo->secret_fname, ".") + 1), encInfo) == e_success) {
                        printf(GREEN"INFO6 :SECRETE FILE EXTENTION SUCCESSFUL...\n"RESET);
                        sleep(1);
                        if (encode_secret_file_extn(encInfo->secret_fname, encInfo) == e_success) {
                            printf(GREEN"INFO7 :ENCODE SECRET FILE SIZE SUCCESSFUL...\n"RESET);
                            sleep(1);

                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success) {
                                printf(GREEN"INFO8 :ENCODE SECRET FILE DATA SUCCESSFUL...\n"RESET);
                                sleep(1);

                                if (encode_secret_file_data(encInfo) == e_success) {
                                    printf(GREEN"INFO9 :COPY REMAINING DATA SUCCESSFUL...\n"RESET);
                                    sleep(1);

                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) {
                                        //printf(GREEN"INF10 :Encoding successful!\n"RESET);
                                        return e_success;
                                    } else {
                                        printf(RED" COPY REMAINING DATA FAILED\n"RESET);
                                        return e_failure;
                                    }
                                } else {
                                    printf(RED" COPY SECRET FILE DATA FAILED\n"RESET);
                                    return e_failure;
                                }
                            } else {
                                printf(RED" COPY SECRET FILE SIZE FAILED\n"RESET);
                                return e_failure;
                            }
                        } else {
                            printf(RED" COPY SECRET FILE EXTENSION FAILED\n"RESET);
                            return e_failure;
                        }
                    } else {
                        printf(RED" EXTENSION SIZE ENCODING FAILED\n"RESET);
                        return e_failure;
                    }
                } else {
                    printf(RED" MAGIC STRING ENCODING FAILED\n"RESET);
                    return e_failure;
                }
            } else {
                printf(RED" BMP HEADER COPYING FAILED\n"RESET);
                return e_failure;
            }
        } else {
            printf(RED" CAPACITY CHECK FAILED\n"RESET);
            return e_failure;
        }
    } else {
        printf(RED" FILE OPENING FAILED\n"RESET);
        return e_failure;
    }
}

