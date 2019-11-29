// Srinjay Kumar
// 170722
// Date : 26/8/2019

//Libraries to be included for the execution of the commands
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>


// the fuction to implement grep
void count(char* path, char* pattern);

// the function to visit all the directories recursively
void visitalldirectories(char *path, char* pattern);

// it matches if the pattern is present in the text
int str_match(char* text, char* pattern);

// function to look for the occurences if the path is a file
void count_file(char* path, char* pattern);

// the main function
// it takes two arguments :
// i) argc is the number of arguments
// ii) argv is the array of strings which are the parameters passed to the function
int main(int argc, char* argv[]){
    // the pattern is the string passed as the 2nd parameter to the main function
    char* pattern = argv[1];

    // the path is the string passed as the 3rd parameter to the main function
    char* path = argv[2];
    //printf("%ld\n",strlen(pattern));
    /*printf("%s\n\n\n",pattern);
    printf("%s\n",path);
    char back[] = "\\";
    strcat(back,path);*/

    // recursively traverse all the sub-directories and the files
    visitalldirectories(path, pattern);
    //count("/home/srinjayk/Desktop/CS_330/IITK_Kanpur.txt");
    //char* pattern = argv[1];
    return 0;
}


// the function to match if pattern is present in the text
int str_match(char* text, char* pattern){

  // n is the length of the pattern
  int n = strlen(pattern);
  // m is the length of the text
  int m = strlen(text);

  //printf("%d %d\n",n,m);

  /*for(int i=0;i<m;i++){
    printf("%c",text[i]);
  }*/

  // iterate for all the substrings of size n that can
  // be generated from the text
  for(int i = 0; i < m-n; i++){
    char str[n];
    // store the substring in a separate string
    for(int j = 0; j < n; j++){
      str[j] = text[i+j];
    }
    //if(str == pattern) return 1;
    //printf("%s %s\n",str,pattern);

    // indicator if the string is matched or not
    int flag = 0;
    for(int j = 0; j < n; j++){
      //if the strings are not equal break from the loop
      if(str[j] != pattern[j]){
        flag = 1;
        break;
      }
    }
    //printf("%d\n",flag);

    //if the indicator stays true then return true
    if(!flag) return 1;
  }
  return 0;
}



// the recursive function to go through all the sub directories and files
void visitalldirectories(char *path, char* pattern){
    //printf("I am here\n");

    // initiation for the parent data path
    DIR *dir_parent;
    // initiation for the children of the parent
    struct dirent *dir_child;

    // data type for checking if the path is of a file
    struct stat store;
    // if the file cannot be opened
    if(stat(path, &store) != 0) return;
    // see the occurences from the file
    if(store.st_mode == 33188) count_file(path, pattern);

    //int n = strlen(path);
    //char pathnew[n];

   /* char* pathnew;
    pathnew = (char*)malloc(strlen(path)*sizeof(char));*/
    //for(int i=0;i<strlen(path);i++)pathnew[i] = path[i];
    //printf("%s\n",pathnew);
    //dir = opendir(path);
    //if (!(dir_parent = opendir(path)))
      //  return;

    // opendir commnad is used to open the path specified in the paramenter
    // passed to the function.
    dir_parent = opendir(path);
    // if the path cannot be returned, then return
    if(!dir_parent) return;

    //dir_child = readdir(dir);

    // parent is read and it is passed to the child
    // if there is any file or subdirectory present in the
    // parent then we enter the while loop
    while ((dir_child = readdir(dir_parent)) != NULL) {
        //if the child is a directory in if (strcmp(dir_child->d_name, ".") == 0itself
        if (dir_child->d_type == DT_DIR) {
            // construct the path for the child
            // give it size equal to the size of newpath
            char* pathnew = (char*)malloc((sizeof(path)+1+sizeof(dir_child->d_name)));
            /*if (strcmp(dir_child->d_name, ".") == 0 || strcmp(dir_child->d_name, "..") == 0)
                continue;*/
            /*if((dir_child->d_name == ".") || (dir_child->d_name == "..")) continue;*/

            // do not do anything if the directory is .
            if (strcmp(dir_child->d_name, ".") == 0)
                continue;
            // similar for ..
            if (strcmp(dir_child->d_name, "..") == 0)
                continue;

            // copy the earlier path into the new path and
            // append it with the new path and the backslash
            strcpy(pathnew, path);
            strcat(pathnew, "/");
            strcat(pathnew, dir_child->d_name);
            //char back[] = {"/"};
            //strcat(strcat(pathnew,back),dir_child->d_name);
            //printf("%s\n",pathnew);

            // recurse for the subdirectory
            visitalldirectories(pathnew,pattern);

        }
        // if the child is not a directory
        else {
            // construct the path for the child
            // give it size equal to the size of newpath
            char* pathnew = (char*)malloc((sizeof(path)+1+sizeof(dir_child->d_name)));
            // copy the earlier path into the new path and
            // append it with the new path and the backslash
            strcpy(pathnew, path);
            strcat(pathnew, "/");
            strcat(pathnew, dir_child->d_name);
            //char back[] = {"/"};
            //strcat(strcat(pathnew,back),dir_child->d_name);

            // look for the occurences of the pattern in the path
            count(pathnew, pattern);
        }
    }

    // close the parent directory
    closedir(dir_parent);
}


// function to look for the occurences
void count(char* path, char* pattern){

    // initiate the file pointer
    int fptr;
    // open the file with read only permission
    fptr = open(path,O_RDONLY, 0);
    //char sentence[1000];
    //for(int i=0;i<1000;i++) sentence[i]='\n';


    //int i = 0;

    //char pattern[] = {"IIT Kanpur"};

    //if(fptr != 0) printf("I am happy.\n");

    // if the file is opened
    if(fptr != 0){

        // initiate the character
        char c;

        // iterate till the loop encounters break condition inside the loop
        // since the loop condition is always satisfied
        while(1){
            //printf("Happy\n" );

            // initiate the buffer
            char* buffer;
            // the initial size should be 0
            int size = 0;
            // dynamically allocate the memory for the buffer
            buffer = (char*)malloc((100000)*sizeof(char));
            //char c;

            // read a single character to the character c
            // the function would return the number of bytes read
            int m = read(fptr,&c,1);

            /*while(c=='\n' && size==0){
                m=read(fptr,&c,1);
            }*/

            // if nothing is read then break from the function
            if(m == 0) break;
            //printf("%d\n",m);
            // read the character to the buffer
            buffer[size] = c;
            // increase the size
            size++;

            // iterate till the loop encounters break condition inside the loop
            // since the loop condition is always satisfied
            while(1){
                m = read(fptr,&c,1);
                //if(m==0) break;
                //if(c == '\n'){
                    //m=read(fptr,&c,1);
                  //  break;
                //}
                //if(m==0) break;
                //else if(c == '\n' && size==1)continue;
                //buffer = realloc(buffer,(size + 1)*sizeof(char));
                // read the character to the buffer
                buffer[size] = c;
                // increase the size
                size++;

                // if endline is encounterd then break from the loop
                if(c == '\n') break;


            }
            //if(m == 0) {if(str_match(buffer,pattern) == 1)printf("%s:%s\n",path,buffer);break;}

            // if the file has ended then break from the loop
            if(m == 0)break;

            // if the pattern matches
            if(str_match(buffer,pattern) == 1){
                int i = 0;
                // do not print the newline character in the beginning of the string
                while(buffer[i] == '\n'){
                    i++;
                }
                printf("%s:%s",path,buffer+i);
            }
            //printf("", buffer);
            //size = 0;
            //buffer = realloc(buffer,(size + 1)*sizeof(char));
            //if(m == 0) break;
            //m = read(fptr,&c,1);
        }

        //printf("%s\n", buffer);
        /*printf("%s\n", );
        printf("%s\n", sentence);*/
    }
}


// function to look for the occurences if the path is a file
void count_file(char* path, char* pattern){

    // initiate the file pointer
    int fptr;
    // open the file with read only permission
    fptr = open(path,O_RDONLY, 0);
    //char sentence[1000];
    //for(int i=0;i<1000;i++) sentence[i]='\n';


    //int i = 0;

    //char pattern[] = {"IIT Kanpur"};

    //if(fptr != 0) printf("I am happy.\n");

    // if the file is opened
    if(fptr != 0){
        /*sentence[0] = fgetc(fptr);
        i++;*/

        //char* buffer;
        //buffer = (char*)malloc(10000*sizeof(char));
        //read(fptr,buffer,10000);
        //printf("%s\n",buffer);

        // initiate the character
        char c;

        // iterate till the loop encounters break condition inside the loop
        // since the loop condition is always satisfied
        while(1){
            //printf("Happy\n" );

            // initiate the buffer
            char* buffer;
            // the initial size should be 0
            int size = 0;
            // dynamically allocate the memory for the buffer
            buffer = (char*)malloc((100000)*sizeof(char));
            //char c;

            // read a single character to the character c
            // the function would return the number of bytes read
            int m = read(fptr,&c,1);

            /*while(c=='\n' && size==0){
                m=read(fptr,&c,1);
            }*/

            // if nothing is read then break from the function
            if(m == 0) break;
            //printf("%d\n",m);
            // read the character to the buffer
            buffer[size] = c;
            // increase the size
            size++;

            // iterate till the loop encounters break condition inside the loop
            // since the loop condition is always satisfied
            while(1){
                m = read(fptr,&c,1);
                //if(m==0) break;
                //if(c == '\n'){
                    //m=read(fptr,&c,1);
                  //  break;
                //}
                //if(m==0) break;
                //else if(c == '\n' && size==1)continue;
                //buffer = realloc(buffer,(size + 1)*sizeof(char));
                // read the character to the buffer
                buffer[size] = c;
                // increase the size
                size++;

                // if endline is encounterd then break from the loop
                if(c == '\n') break;


            }
            //if(m == 0) {if(str_match(buffer,pattern) == 1)printf("%s:%s\n",path,buffer);break;}

            // if the file has ended then break from the loop
            if(m == 0)break;

            // if the pattern matches
            if(str_match(buffer,pattern) == 1){
                int i = 0;
                // do not print the newline character in the beginning of the string
                while(buffer[i] == '\n'){
                    i++;
                }
                printf("%s",buffer+i);
            }
            //printf("", buffer);
            //size = 0;
            //buffer = realloc(buffer,(size + 1)*sizeof(char));
            //if(m == 0) break;
            //m = read(fptr,&c,1);
        }
    }
}
