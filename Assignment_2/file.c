// Srinjay Kumar
// 170722

#include<types.h>
#include<context.h>
#include<file.h>
#include<lib.h>
#include<serial.h>
#include<entry.h>
#include<memory.h>
#include<fs.h>
#include<kbd.h>
#include<pipe.h>


/************************************************************************************/
/***************************Do Not Modify below Functions****************************/
/************************************************************************************/
void free_file_object(struct file *filep)
{
    if(filep)
    {
       os_page_free(OS_DS_REG ,filep);
       stats->file_objects--;
    }
}

struct file *alloc_file()
{

  struct file *file = (struct file *) os_page_alloc(OS_DS_REG);
  file->fops = (struct fileops *) (file + sizeof(struct file));
  bzero((char *)file->fops, sizeof(struct fileops));
  stats->file_objects++;
  return file;
}

static int do_read_kbd(struct file* filep, char * buff, u32 count)
{
  kbd_read(buff);
  return 1;
}

static int do_write_console(struct file* filep, char * buff, u32 count)
{
  struct exec_context *current = get_current_ctx();
  return do_write(current, (u64)buff, (u64)count);
}

struct file *create_standard_IO(int type)
{
  struct file *filep = alloc_file();
  filep->type = type;
  if(type == STDIN)
     filep->mode = O_READ;
  else
      filep->mode = O_WRITE;
  if(type == STDIN){
        filep->fops->read = do_read_kbd;
  }else{
        filep->fops->write = do_write_console;
  }
  filep->fops->close = generic_close;
  filep->ref_count = 1;
  return filep;
}

int open_standard_IO(struct exec_context *ctx, int type)
{
   int fd = type;
   struct file *filep = ctx->files[type];
   if(!filep){
        filep = create_standard_IO(type);
   }else{
         filep->ref_count++;
         fd = 3;
         while(ctx->files[fd])
             fd++;
   }
   ctx->files[fd] = filep;
   return fd;
}
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/




/*void free_pipe_info(struct pipe_info *p_info)
{
    if(p_info)
    {
        os_page_free(OS_DS_REG ,p_info->pipe_buff);
        os_page_free(OS_DS_REG ,p_info);
    }
}*/

void do_file_fork(struct exec_context *child)
{
   /*TODO the child fds are a copy of the parent. Adjust the refcount*/
   //struct file* file[MAX_OPEN_FILES] = child->files;

   // the maxinum number of possible open files
   // is given in file.h
   for(int i=0; i<MAX_OPEN_FILES ;i++){
     // decrease the ref count of the file if the
     // file is not NULL
     // we iterate for all the files refrenced from the
     // execution context
     // increasing the ref count
     if(child->files[i] != NULL){
        int m = child->files[i]->ref_count;
        child->files[i]->ref_count = m+1;
    }
   }

}


// the function to exit from the system by closing all the files
void do_file_exit(struct exec_context *ctx)
{
   /*TODO the process is exiting. Adjust the ref_count
     of files*/
    //struct file* files[MAX_OPEN_FILES] = ctx->files;
    // iterate through all the files and close all the files
    // by calling the generic_close function
    for(int i=0; i<MAX_OPEN_FILES ;i++){
      //ctx->files[i]->ref_count = 0;
      generic_close(ctx->files[i]);
      //ctx->files[i] = NULL;
    }
}


// the function to close the files and the pipe if required
long generic_close(struct file *filep)
{
  /** TODO Implementation of close (pipe, file) based on the type
   * Adjust the ref_count, free file object
   * Incase of Error return valid Error code
   */
    int ret_fd = -EINVAL;
    //int m = filep->ref_count;
    //filep->ref_count = m-1;
    // check if the file is NULL and if the file is NULL then return error
    if(filep == NULL) {
      return -EINVAL;
    }
    // if(filep->inode == NULL){
    //   return -EACCES;
    // }
    //if(filep->ref_count < 0) return -EINVAL;

    // if the file is regular and not a pipe
    if(filep->type == REGULAR){
      // if the file is referenced only once
      if(filep->ref_count == 1){
        //filep = NULL;
        // free the file object
        free_file_object(filep);
        //filep = NULL;
      }
      // if the file is referenced only once
      if(filep->ref_count > 1){
        // reduce the reference count of the file by 1
        int m = filep->ref_count;
        filep->ref_count = m-1;
      }
    }


    //if the file is a pipe
    if(filep->type == PIPE){
      // if the file is referenced only once
      if(filep->ref_count == 1){
        // if the file is of mode write
        if(filep->mode == O_WRITE){
          // if the read not closed then only close the file
          if(filep->pipe->is_ropen != 0){
            // change the wopen to 0
            filep->pipe->is_wopen = 0;
            free_file_object(filep);
          }
          // if the read is already closed then also close the pipe_read
          // because both the pipeswill be closed know
          if(filep->pipe->is_ropen == 0){
            // change the wopen to 0
            filep->pipe->is_wopen = 0;
            free_pipe_info(filep->pipe);
            free_file_object(filep);
          }
        }
        // if the file is of mode read
        if(filep->mode == O_READ){
          // if the write not closed then only close the file
          if(filep->pipe->is_wopen != 0){
            // change the ropen to 0
            filep->pipe->is_ropen = 0;
            free_file_object(filep);
          }
          // if the write is already closed then also close the pipe_write
          // because both the pipes will be closed know
          if(filep->pipe->is_wopen == 0){
            // change the ropen to 0
            filep->pipe->is_ropen = 0;
            free_pipe_info(filep->pipe);
            free_file_object(filep);
          }
        }
      }

      // if the refcount is more than one then reduce all the
      // reference count by one
      if(filep->ref_count > 1){
        //if(filep->mode == O_WRITE)
        // filep->pipe->is_ropen--;
        //if(filep->mode == O_READ)
        //  filep->pipe->is_wopen--;
        int m = filep->ref_count;
        filep->ref_count = m-1;
      }
    }
    return ret_fd;
}


// implementation of read function
static int do_read_regular(struct file *filep, char * buff, u32 count)
{
   /** TODO Implementation of File Read,
    *  You should be reading the content from File using file system read function call and fill the buf
    *  Validate the permission, file existence, Max length etc
    *  Incase of Error return valid Error code
    * */
    int ret_fd = -EINVAL;
    // if the inode is NULL then return error
    if(filep->inode == NULL){
      return -EACCES;
    }

    //check for the Mode
    if(!(filep->mode & O_READ)){
      return -EINVAL;
    }
    //ret_fd = open_standard_IO()
    //return flat_read(filep->inode,buff,count,&(filep->offp));

    // call the flat read function to read data
    //printk("--%x:file-   %x:inode-   %d:filesize   %d:offset-after read\n",filep,filep->inode,filep->inode->file_size);
    int n = filep->offp;
    int o = filep->inode->file_size;
    //printk("%d+\n",o);
    // check for the maximum characters that can be read at once
    if(count+n > o) {
      return -EINVAL;
    }
    // call the flat read to read the file
    int m = flat_read(filep->inode,buff,count,& (filep->offp));
    // incease the offset of the file
    int p = filep->offp;
    filep->offp = p + m;
    return m;

    //return ret_fd;
}

// implementation of the write function
static int do_write_regular(struct file *filep, char * buff, u32 count)
{
    /** TODO Implementation of File write,
    *   You should be writing the content from buff to File by using File system write function
    *   Validate the permission, file existence, Max length etc
    *   Incase of Error return valid Error code
    * */

    int ret_fd = -EINVAL;

    // if the inode is NULL then return error
    if(filep->inode == NULL){
      return -EACCES;
    }

    // check for the permissions
    if(!(filep->mode & O_WRITE)){
      return -EINVAL;
    }

    int n = filep->offp;
    int o = filep->inode->file_size;

    // check for the limit of maximum file size
    if(count+o > FILE_SIZE) {
      return -EINVAL;
    }

    //call the flat_write function to implement the write function
    int m = flat_write(filep->inode,buff,count,& (filep->offp));
    //int s = filep->inode->file_size;
    //filep->inode->file_size = s + count;
    //printk("%d-after write\n",filep->inode->file_size);
    //printk("--%x:file-  %x:inode-   %d:filesize   %d:offset-after write\n",filep,filep->inode,filep->inode->file_size);
    // incease the offset of the file
    //printk("%d:filesize\n",filep->inode->file_size);
    int p = filep->offp;
    filep->offp = p + m;
    // ret
    return m;

    //return ret_fd;
}

static long do_lseek_regular(struct file *filep, long offset, int whence)
{
    /** TODO Implementation of lseek
    *   Set, Adjust the ofset based on the whence
    *   Incase of Error return valid Error code
    * */
    int ret_fd = -EINVAL;
    // if the file is NULL the return the error
    if(filep == NULL) {
      return -EINVAL;
    }
    // if the inode is NULL then return error
    if(filep->inode == NULL){
      return -EACCES;
    }

    int end = filep->inode->e_pos;
    int begin = filep->inode->s_pos;

    int size = filep->inode->file_size;

    int size1 = end - begin;
    // printk("%d-size1\n",size1);
    // printk("%d-size\n",FILE_SIZE);
    // if the whence is SET
    if(whence == SEEK_SET){
      // fit the offset to the starting of the file
      if(offset > FILE_SIZE){
        return -EINVAL;
      }
      filep->offp = offset;
      //printf("Hello0\n");
      return filep->offp;
    }
    // if the whence is CUR
    else if(whence == SEEK_CUR){
      long m = filep->offp;
      // rerurn the current offset
      if(m + offset > FILE_SIZE){
        return -EINVAL;
      }
      filep->offp = m + offset;
      //printf("Hello1\n");
      return filep->offp;
    }
    // if the whence is end
    else if(whence == SEEK_END){

      // retunr current + offset
      int m = size;
      //printk("%d--\n",size1);
      if(m + offset > FILE_SIZE){
        return -EINVAL;
      }
      filep->offp = m + offset;
      //printf("Hello2\n");
      return filep->offp;
    }
    //return ret_fd;
    return -ret_fd;
}


// implementation of file opem
extern int do_regular_file_open(struct exec_context *ctx, char* filename, u64 flags, u64 mode)
{
  /**  TODO Implementation of file open,
    *  You should be creating file(use the alloc_file function to creat file),
    *  To create or Get inode use File system function calls,
    *  Handle mode and flags
    *  Validate file existence, Max File count is 32, Max Size is 4KB, etc
    *  Incase of Error return valid Error code
    * */
    int ret_fd = -EINVAL;
    // create a new inode
    struct inode* new_inode;
    //struct inode* curr_inode;
    // create a new file
    struct file* new_file;
    //struct file* curr_file;
    // initiate a new file descriptor
    int fd_new;
    //int fd_curr;
    int flag;
    //lookup
    //if null and create flag given then create ncode and introduce a fiel pointer
    //i create flag not gven then error()
    // if lookup exists introduce a file pointer
    /*if(flags == "O_CREATE"){
      new_inode = create_inode(filename,mode);
    }
    else{
      curr_inode = lookup_inode(filename);
    }*/

    //curr_inode = lookup_inode(filename);
    // lookup inode looks if the file is already created
    new_inode = lookup_inode(filename);
    // if the file is not already created
    if(new_inode == NULL){
      // if the premission to create is provoded
      if(flags & O_CREAT){
        //struct file* new_file;
        // create  a new inode
        new_inode = create_inode(filename, mode);
        // allocate the file
        new_file = alloc_file();
        flag = 1;
      }
      else{
        // if create permission not given then return error
        //flag = 0;
        return -EINVAL;
      }
    }
    // if the file is already present with the same permissions
    else if(flags & O_CREAT){
      //flag = 2;
      // allocate the file
      //new_file = alloc_file();
      return -EINVAL;
      //struct file* curr_file;
    }
    else{
      new_file = alloc_file();
    }

    // allocating the file descriptor
    fd_new = 3;
    // iterate till the empty descriptor is not found
    while(ctx->files[fd_new]){
      fd_new++;
    }
    //fd_curr = fd_new;
    /*u32 type;               // Type can be REGULAR, PIPE -> (You can use the above enum to set the type)
    u32 mode;               // Mode -> File/Pipe mode, READ, WRITE, WRITE_ONLY, READ_ONLY
    u32 offp;               // offset -> Last read/write offset of the file
    u32 ref_count;          // Reference count of the file object
    struct inode * inode;   // Incase of Pipe, It will be null,
    struct fileops * fops;  // Map the function call based on the type {PIPE, REGULAR}
    struct pipe_info * pipe;// Incase of Regular file, It will be null*/

    /*if(flag == 2){
      curr_file->type = REGULAR;
      curr_file->mode = mode;
      curr_file->offp = 0;
      curr_file->fops->read = do_read_regular;
      curr_file->fops->write = do_write_regular;
      curr_file->fops->lseek = do_lseek_regular;
      curr_file->fops->close = generic_close;
      //curr_file-ref_count = ;
      curr_file->inode = curr_inode;
      curr_file->pipe = NULL;
      //fd_curr = open_standard_IO(ctx,curr_file->type);
      ret_fd = fd_curr;
      ctx->files[ret_fd] = curr_file;
    }*/

    //if(flag == 1){
      // filling out all the fields
      // thr type is regular
      new_file->type = REGULAR;
      // mode is taken input
      new_file->mode = mode;
      // offset is zero
      new_file->offp = 0;
      // readfunction pointer is given
      new_file->fops->read = do_read_regular;
      // write function pointer is given
      new_file->fops->write = do_write_regular;
      // seek function pointer is given
      new_file->fops->lseek = do_lseek_regular;
      // close function pointer is given
      new_file->fops->close = generic_close;
      //curr_file-ref_count = ;
      // set the ref count
      new_file->ref_count = 1;
      // set the inode
      new_file->inode = new_inode;
      //new_file->inode->file_size = 0;
      // set the pipe to NULL
      new_file->pipe = NULL;
      //fd_new = open_standard_IO(ctx,new_file->type);
      // give the value to ret_fd
      ret_fd = fd_new;
      // give the file pointer in exec_context
      ctx->files[fd_new] = new_file;
    //}

    return ret_fd;
}


// Implementation of dup
int fd_dup(struct exec_context *current, int oldfd)
{
     // TODO Implementation of dup
     //  *  Read the man page of dup and implement accordingly
     //  *  return the file descriptor,//
     //  *  Incase of Error return valid Error code
     //  * */
    int ret_fd = -EINVAL;
    // set the file descriptor
    int fd_new = 0;
    while(current->files[fd_new]){
      fd_new++;
    }
    // changes the file descriptor according to dup
    current->files[fd_new] = current->files[oldfd];
    // increase the ref_count
    int m = current->files[fd_new]->ref_count;
    current->files[fd_new]->ref_count = m + 1;
    // give the value for the ret_fd
    ret_fd = fd_new;
    return ret_fd;
}

// Implementation of dup2
int fd_dup2(struct exec_context *current, int oldfd, int newfd)
{
  // /** TODO Implementation of the dup2
  //   *  Read the man page of dup2 and implement accordingly
  //   *  return the file descriptor,
  //   *  Incase of Error return valid Error code
  //   * */
    //this will lead to the closing of a opened file
    if(newfd == oldfd){
      return -EINVAL;
    }
    int ret_fd = -EINVAL;
    current->files[newfd] = current->files[oldfd];
    // increae the ref count by 1
    // changes the file descriptor according to dup2
    int m = current->files[oldfd]->ref_count;
    current->files[oldfd]->ref_count = m + 1;
    // give the value to ret_fd
    ret_fd = newfd;
    return ret_fd;
}