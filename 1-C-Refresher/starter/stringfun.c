#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
void reverse_string(char *, int);
void print_words(char *, int);


int setup_buff(char *buff, char *user_str, int len){
    int user_str_len = 0;
    int buff_index = 0;
    int space_flag = 0;

    // calculate the length of the user string
    char *ptr = user_str;
    while (*ptr != '\0'){
        user_str_len++;
        ptr++;
    }

    // check if user_str is to big
    if (user_str_len > BUFFER_SZ){
        return -1;
    }
    
    // copy the user string to the buffer
    ptr = user_str;
    while (*ptr != '\0' && buff_index < len){
        if (*ptr == ' ' || *ptr == '\t'){
            if (!space_flag){
                buff[buff_index++] = ' ';
                space_flag = 1;
            }
        }else{
            buff[buff_index++] = *ptr;
            space_flag = 0;
        }
        ptr++;
    }
    
    // fill the rest of buff with ...
    while (buff_index < len){
        buff[buff_index++] = '.';
    }

    return user_str_len; 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int word_count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++){
        if (*(buff + i) != ' ' && *(buff + i) != '.'){
            if (!in_word){
                word_count++;
                in_word = 1;
            }
        }else{
            in_word = 0;
        }
    }
    printf("Word Count: %d\n", word_count);
    return word_count;
}

void reverse_string(char *buff, int str_len){
    char *start = buff;
    char *end = buff + str_len - 1;
    char temp;

    while (start < end){
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }

    printf("Reversed String: ");
    for (int i = 0; i < str_len; i++){
        putchar(*(buff + i));
    }
    putchar('\n');
}

void print_words(char *buff, int str_len){
    int word_index = 1;
    char *start = buff;
    char *end = buff;

    printf("Word Print\n----------\n");

    while (*end != '\0' && end < buff + str_len){
        if (*end == ' ' || *end == '.'){
            if (start != end){
                printf("%d. ", word_index++);
                for (char *ptr = start; ptr < end; ptr++) {
                    putchar(*ptr);
                }
                printf(" (%ld)\n", end - start);
            }
            start = end + 1;
        }
        end++;
    }

    if (start != end){
        printf("%d. ", word_index++);
        for (char *ptr = start; ptr < end; ptr++) {
            putchar(*ptr);
        }
        printf(" (%ld)\n", end - start);
    }
}



int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      
    //      This is safe because it ensures that ther are at least two command
    //      line arguments. If there are not, the program will print the usage
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      
    //      This if statement checks if there are less than 3 
    //      command line arguments
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char *)malloc(BUFFER_SZ);
    if (buff == NULL){
        printf("Error allocating buffer");
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("error: Provided input string is too long\n");
        free(buff);
        exit(3);
    }

    switch (argv[1][1]){
        case 'h':
            usage(argv[0]);
            break;
        case 'c':
            count_words(buff, BUFFER_SZ, user_str_len);
            break;
        case 'r':
            reverse_string(buff, user_str_len);
            break;
        case 'w':
            print_words(buff, user_str_len);
            break;
        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          Providing both the pointer and length is good practice because it 
//          allows the function to be more flexible and reusable with different
//          buffer sizes. It also can help prevent buffer overflows. 
//         