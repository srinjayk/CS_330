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


// the fuction to count the size
int count(char* path);

// the function to visit all the directories recursively
void visitalldirectories(char *path, char* pattern);

// it matches if the pattern is present in the text
int str_match(char* text, char* pattern);

// the function to get the size of the children subdirectories
int get_Children_directory_size(char* path, char* dir_child_name, int root_size);


// the main function
// it takes two arguments :
// i) argc is the number of arguments
// ii) argv is the array of strings which are the parameters passed to the function
int main(int argc, char* argv[]){
    //visitalldirectories("/home/srinjayk/Desktop/Assignment1/Part_1/Test_Cases");
    //count("/home/srinjayk/Desktop/CS_330/IITK_Kanpur.txt");
    //int b = get_directory_size(argv[1], "Root", 0);

    // initiate the variable containig the size of the root variable
    int root_size = 0;

    // initiation for the parent data path
    DIR *dir_parent;
    // initiation for the children of the parent
    struct dirent *dir_child;

    //int n = strlen(path);
    //char pathnew[n];

   /* char* pathnew;
    pathnew = (char*)malloc(strlen(path)*sizeof(char));*/
    //for(int i=0;i<strlen(path);i++)pathnew[i] = path[i];
    //printf("%s\n",pathnew);
    //dir = opendir(path);

    // the path is in the second argument
    char* path = argv[1];

    int n = strlen(path);

    int i=0;
    int pos = 0;

    for(i=0;i<n;i++){
        if(path[i] == '/'  && i!=n-1) pos = i;
    }


    // opendir commnad is used to open the path specified in the paramenter
    // passed to the function.
    dir_parent = opendir(path);
    // if the path cannot be returned, then return
    if(!dir_parent) return 0;

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
            //get_directory_size(pathnew);

            // intiatilize the fiel descriptor with two elements
            int fd[2];
            // construct a pipe from the file descriptor
            pipe(fd);
            //declare the pid
            pid_t pid;
            // fork to create a child process
            pid = fork();
            // if we are in a parent directory
            if(!pid){
                // caluclate the size of the subdirectory
                int a = get_Children_directory_size(pathnew,dir_child->d_name,0);
                // pass the size in the pipe
                write(fd[1],&a,sizeof(a));
                // close both the ends of the pipe
                close(fd[1]);
                close(fd[0]);
                // print the child of the subdirectory
                printf("%s %d\n",dir_child->d_name,a);
                // exit from the process
                exit(0);
            }

            // if we are in a child process
            else{
                int a;
                // close the writing end of the pipe
                close(fd[1]);
                wait(NULL);
                // read the size into the variable a
                read(fd[0],&a,sizeof(a));
                // close the reading end
                close(fd[0]);
                // increment the root size
                root_size = root_size + a;
            }
            //printf("%\n");

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
            //printf("%s %d\n",dir_child->d_name, count(pathnew));

            // increment the root size by the size of the file
            root_size = root_size + count(pathnew);
            // print the size of the file
            printf("%s %d\n",dir_child->d_name,count(pathnew));
        }

    }
    //if(layer <= 1)printf("Root %d\n",root_size);
    // print the path of the directory
    // for(i = pos;i<n;i++){
    //     if(path[i] != '/') printf("%c",path[i]);
    // }
     printf("%s %d\n",path,root_size);
    // printting the size of the parent directory
    //printf(" %d\n",root_size);
    // close the parent directory
    closedir(dir_parent);


    return 0;
}

int get_Children_directory_size(char* path, char* dir_child_name, int root_size){

    //int root_size = 0;

    // initiation for the parent data path
    DIR *dir_parent;
    // initiation for the children of the parent
    struct dirent *dir_child;

    //int n = strlen(path);
    //char pathnew[n];

   /* char* pathnew;
    pathnew = (char*)malloc(strlen(path)*sizeof(char));*/
    //for(int i=0;i<strlen(path);i++)pathnew[i] = path[i];
    //printf("%s\n",pathnew);
    //dir = opendir(path);

    // opendir commnad is used to open the path specified in the paramenter
    // passed to the function.
    dir_parent = opendir(path);
    // if the path cannot be returned, then return
    if(!dir_parent) return 0;

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
            //get_directory_size(pathnew);
            //printf("%d\n",get_Children_directory_size(pathnew,dir_child->d_name,0));
            // increase the root size
            root_size = root_size + get_Children_directory_size(pathnew,dir_child->d_name,0);

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

            // increase the root size
            root_size = root_size + count(pathnew);

        }
    }
    //if(layeprintf("%s %d\n",dir_child_name,root_size);

    // close the parent directory
    closedir(dir_parent);
    // return the root size
    return root_size;
}

// give the size of the file
int count(char* path){

    // data type for calculating the size
    struct stat store;
    // if the file cannot be opened
    if(stat(path, &store) != 0) return 0;
    // return the size of the file
    return store.st_size;
}

// the function to match the strings
int str_match(char* text, char* pattern){
  int n = strlen(pattern);
  int m = strlen(text);

  //printf("%d %d\n",n,m);

  /*for(int i=0;i<m;i++){
    printf("%c",text[i]);
  }*/

  for(int i = 0; i < m-n; i++){
    char str[n];
    for(int j = 0; j < n; j++){
      str[j] = text[i+j];
    }
    //if(str == pattern) return 1;
    //printf("%s %s\n",str,pattern);
    int flag = 0;
    for(int j = 0; j < n; j++){
      if(str[j] != pattern[j]){
        flag = 1;
        break;
      }
    }
    //printf("%d\n",flag);
    if(!flag) return 1;
  }
  return 0;
}


// the function all the directories recursively
void visitalldirectories(char *path, char* pattern)
{
    //printf("I am here\n");

    // initiation for the parent data path
    DIR *dir_parent;
    // initiation for the children of the parent
    struct dirent *dir_child;

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
            visitalldirectories(pathnew, pattern);

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
            count(pathnew);
        }
    }

    // close the parent directory
    closedir(dir_parent);
}
