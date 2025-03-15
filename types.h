/*
Name : pruthvi jyoti
date : 15/10/2024
*/
#ifndef TYPES_H
	//	printf("fpinter in copy at = %ld\n", ftell(fptr_src_image));
	//	printf("fpinter in copy at = %ld\n", ftell(fptr_dest_image));
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

// color code 

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#endif
