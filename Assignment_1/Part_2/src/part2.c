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
#include <sys/wait.h>
#include <sys/stat.h>


// the fuction to implement grep
void count(char* path, char* pattern);

// the function to visit all the directories recursively
void visitalldirectories(char *path, char* pattern);

// it matches if the pattern is present in the text
int str_match(char* text, char* pattern);


// the main function
// it takes two arguments :
// i) argc is the number of arguments
// ii) argv is the array of strings which are the parameters passed to the function
int main(int argc, char *argv[]){

    // the path is in the third argument
    char* pattern = argv[2];
    // the path is in the fourth argument
    char* path = argv[3];
    //printf("%s\n",pattern);
    //printf("%s\n",path);
    //printf("%d\n",argc);
    //printf("Hi\n");

    // if the number of arguments is 4
    if(argc == 4){
        //printf("I am here.\n");

        // the filedescriptor array
        int fd[2];
        // pid for the program
        pid_t pid;
        // construct the pipe
        pipe(fd);
        // fork the function
        pid = fork();
        // if it is a parent function
        if(!pid){
            // connect fd[1] to stdout
            dup2(fd[1],1);
            // close the other end of the pipe
            close(fd[0]);
            // execute the grep command
            execl("/bin/grep","grep","-rF",pattern,path,(char*)NULL);
        }
        else{
            //connect fd[0] to stdin
            dup2(fd[0],0);
            // close the other end of the pipe
            close(fd[1]);
            // execute the wc command
            execlp("wc","wc","-l",(char*)NULL);
            perror("wc");
            // exit form the process
            exit(1);
        }
    }
    else if(argc == 6){
        //printf("I am here\n");

        // the file argument is present in argv[4]
        char* file = argv[4];
        //perror("Not attempted part 2");

        // the pid's for forking
        pid_t pid1, pid2, pid3;

        //initialising the pipes
        int fd1[2];
        int fd2[2];

        //creating the pipes
        pipe(fd1);
        pipe(fd2);

        //opening the file
        int fdfile = open("output.txt",O_WRONLY | O_TRUNC);
        //fd1[1] = open("output.txt",O_WRONLY);
        //fd2[0] = open("output.txt",O_RDONLY);


        //char* output = "output.txt";

        // making the child and parent process
        pid1 = fork();

        // for the child process
        if(!pid1){
            //printf("I am here\n");
            // dup stdout to fd1[1]
            dup2(fd1[1],1);

            // close the write end of both the pipes
            close(fd2[1]);
            close(fd1[1]);

            //wait(NULL);
            //printf("frep");

            //execute the grep command
            execl("/bin/grep","grep","-rF",pattern,path,(char*)NULL);
        }

        //for the parent process
        else{
            //printf("I am also here\n");

            //forking the second process
            pid2 = fork();

            //for the child process
            if(!pid2){
                //printf("I am in child of pid2\n");
                // dup stdin to fd1[0]
                dup2(fd1[0],0);
                // dup stdout to fd2[1]
                dup2(fd2[1],1);

                //close both the write ends
                close(fd2[1]);
                close(fd1[1]);
                //wait(NULL);

                //initialise a new character
                char char_ch;

                //reading and writng the file to output.txt and therefore executing the tee command
                while(read(0,&char_ch,1)){
                    //perror("I am writing in the file");
                    // wirting to the stdout
                    write(1,&char_ch,1);
                    // writing to stdin
                    write(fdfile,&char_ch,1);
                }
                //close the stdout
                //close(1);
                //execlp("tee","tee",file,(char*)NULL);
                // execvp(str1, str2);
            }
            else{
                //printf("I am in ultimate patent\n");

                // dup stdin to fd2[0]
                dup2(fd2[0],0);

                //closing both the writing ends
                close(fd1[1]);
                close(fd2[1]);
                //wait(NULL);
                // executing the sort commands
                execlp("sort","sort",(char*) NULL);
            }
        }
    }
    else if(argc == 7){
        //perror("Not attempted part 2");
        // the file argument is present in argv[4]
        char* file = argv[4];
        //perror("Not attempted part 2");

        // the pid's for forking
        pid_t pid1, pid2, pid3;

        //initialising the pipes
        int fd1[2];
        int fd2[2];

        //creating the pipes
        pipe(fd1);
        pipe(fd2);

        //opening the file
        int fdfile = open("output.txt",O_WRONLY | O_TRUNC);
        //fd1[1] = open("output.txt",O_WRONLY);
        //fd2[0] = open("output.txt",O_RDONLY);


        char* output = "output.txt";

        // making the child and parent process
        pid1 = fork();

        // for the child process
        if(!pid1){
            //printf("I am here\n");
            // dup stdout to fd1[1]
            dup2(fd1[1],1);

            // close the write end of both the pipes
            close(fd2[1]);
            close(fd1[1]);

            //wait(NULL);
            //printf("frep");

            //execute the grep command
            execl("/bin/grep","grep","-rF",pattern,path,(char*)NULL);
        }

        //for the parent process
        else{
            //printf("I am also here\n");

            //forking the second process
            pid2 = fork();

            //for the child process
            if(!pid2){
                //printf("I am in child of pid2\n");
                // dup stdin to fd1[0]
                dup2(fd1[0],0);
                // dup stdout to fd2[1]
                dup2(fd2[1],1);

                //close both the write ends
                close(fd2[1]);
                close(fd1[1]);
                //wait(NULL);

                //initialise a new character
                char char_ch;

                //reading and writng the file to output.txt and therefore executing the tee command
                while(read(0,&char_ch,1)){
                    //perror("I am writing in the file");
                    // writing to the stdout
                    write(1,&char_ch,1);
                    // writing to stdin
                    write(fdfile,&char_ch,1);
                }
                //close the stdout
                //close(1);
                //execlp("tee","tee",file,(char*)NULL);
                // execvp(str1, str2);
            }
            else{
                //printf("I am in ultimate patent\n");

                // dup stdin to fd2[0]
                dup2(fd2[0],0);

                //closing both the writing ends
                close(fd1[1]);
                close(fd2[1]);
                //wait(NULL);
                // executing the line count commands
                //execlp("sort","sort",(char*) NULL);
                execlp("wc","wc","-l",(char*)NULL);
            }
        }
    }
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



void visitalldirectories(char *path, char* pattern)
{
    //printf("I am here\n");

    // initiation for the parent data path
    DIR *dir_parent;
    // initiation for the children of the parent
    struct dirent *dir_child;
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
    }
}
