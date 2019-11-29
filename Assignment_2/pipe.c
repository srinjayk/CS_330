// Srinjay Kumar
//170722

#include<pipe.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>
/***********************************************************************
 * Use this function to allocate pipe info && Don't Modify below function
 ***********************************************************************/
struct pipe_info* alloc_pipe_info()
{
    struct pipe_info *pipe = (struct pipe_info*)os_page_alloc(OS_DS_REG);
    char* buffer = (char*) os_page_alloc(OS_DS_REG);
    pipe ->pipe_buff = buffer;
    return pipe;
}


void free_pipe_info(struct pipe_info *p_info)
{
    if(p_info)
    {
        os_page_free(OS_DS_REG ,p_info->pipe_buff);
        os_page_free(OS_DS_REG ,p_info);
    }
}
/*************************************************************************/
/*************************************************************************/



// implementation of pipe read function
int pipe_read(struct file *filep, char *buff, u32 count)
{
    /**
    *  TODO:: Implementation of Pipe Read
    *  Read the contect from buff (pipe_info -> pipe_buff) and write to the buff(argument 2);
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code
    */

    int ret_fd = -EINVAL;
    // if the file is null then return error
    if(filep == NULL){
      return -EACCES;
    }
    //ret_fd = open_standard_IO()
    //return flat_read(filep->inode,buff,count,&(filep->offp));

    //int m = flat_read(filep->inode,buff,count,& (filep->offp));
    //filep->offp = filep->offp + m;
    //return m;

    // check if the required characters are available
    int n = filep->pipe->buffer_offset;
    if(n < count){
      return -EINVAL;
    }

    char *buffer_pipe = filep->pipe->pipe_buff;
    char a;
    // initialisation of the counter
    int i = 0;
    // initialisation of the reading position
    int reading = filep->pipe->read_pos;
    // read till the count is under limit pipe is not empty
    while(i < count && buffer_pipe[i]!='\0'){
        // copy the character to the buffer
        a = filep->pipe->pipe_buff[(i + reading)%PIPE_MAX_SIZE];
        buff[i] = a;
        int m = filep->pipe->buffer_offset;
        filep->pipe->buffer_offset = m-1;
        // increase the counter in each iteration
        i++;
    }
    //change the position of reading position accordingly
    filep->pipe->read_pos = (i+reading)%PIPE_MAX_SIZE;

    //int ret_fd = -EINVAL;
    return i;
}


// Implementation of the write position
int pipe_write(struct file *filep, char *buff, u32 count)
{
    /**
    *  TODO:: Implementation of Pipe Read
    *  Write the contect from   the buff(argument 2);  and write to buff(pipe_info -> pipe_buff)
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code
    */

    int ret_fd = -EINVAL;

    // if the file is null then return error
    if(filep == NULL){
      return -EACCES;
    }
    //int m = flat_write(filep->inode,buff,count,& (filep->offp));
    //filep->offp = filep->offp + m;
    //return m;

    // check if writing does not exceed filesize
    int n = filep->pipe->buffer_offset;
    if(n + count > PIPE_MAX_SIZE){
      return -EINVAL;
    }

    //char *buffer_pipe = filep->pipe->pipe_buff;
    char a;
    // initialise the counting
    int i = 0;
    // initialise the counting position
    int writing = filep->pipe->write_pos;
    // write till the count is under limit pipe is not empty
    while(i < count && buff[i]!='\0'){
        // copy the character to the pipe
        a = buff[i];
        filep->pipe->pipe_buff[(i + writing)%PIPE_MAX_SIZE] = a;
        int m = filep->pipe->buffer_offset;
        filep->pipe->buffer_offset = m+1;
        // increase the counter
        i++;
    }
    // update the writing position
    filep->pipe->write_pos = (i + writing)%PIPE_MAX_SIZE;

    //int ret_fd = -EINVAL;
    return i;
}



// create the pipe function
int create_pipe(struct exec_context *current, int *fd)
{
    /**
    *  TODO:: Implementation of Pipe Create
    *  Create file struct by invoking the alloc_file() function,
    *  Create pipe_info struct by invoking the alloc_pipe_info() function
    *  fill the valid file descriptor in *fd param
    *  Incase of Error return valid Error code
    */
    int ret_fd = -EINVAL;

    // create the file with read end of the pipe
    struct file* read_file = alloc_file();
    // create the file with write end of the pipe
    struct file* write_file = alloc_file();
    // create the pipe
    struct pipe_info* pipe = alloc_pipe_info();
    //struct pipe_info* write_pipe = alloc_pipe_info();

    int count = 0;
    int pos = 0;

    // assign two file descriptors that are the next two empty ones
    int fd_curr = 0;
    // assign the fd[0] and fd[1]
    while(count < 2){
        while(current->files[fd_curr]){
            fd_curr++;
        }
        //count++;
        fd[count] = fd_curr;
        count++;
        //fd[count] = fd_curr+1;
        //pos++;
        fd_curr++;
    }

    char* buffer;

    // give the ropen 1
    pipe->is_ropen = 1;
    // give the wopen 1
    pipe->is_wopen = 1;
    // initialise the read pos to 0
    pipe->read_pos = 0;
    // initiaise the write pos to 0
    pipe->write_pos = 0;
    // initialsie the offset to 0
    pipe->buffer_offset = 0;

    /*u32 type;               // Type can be REGULAR, PIPE -> (You can use the above enum to set the type)
    u32 mode;               // Mode -> File/Pipe mode, READ, WRITE, WRITE_ONLY, READ_ONLY
    u32 offp;               // offset -> Last read/write offset of the file
    u32 ref_count;          // Reference count of the file object
    struct inode * inode;   // Incase of Pipe, It will be null,
    struct fileops * fops;  // Map the function call based on the type {PIPE, REGULAR}
    struct pipe_info * pipe;// Incase of Regular file, It will be null*/

    // the type of the read file is PIPE
    read_file->type = PIPE;
    // give the mode to the READ
    read_file->mode = O_READ;
    // initialise the offset to 0
    read_file->offp = 0;
    // the fuction for reading is pipe_read
    read_file->fops->read = pipe_read;
    // the function for writing is pipe_write
    read_file->fops->write = pipe_write;
    //new_file->fops->lseek = do_lseek_regular;
    // the close function is generic_close
    read_file->fops->close = generic_close;
    // the ref is set to 1
    read_file->ref_count = 1;
    // inode is set to NULL
    read_file->inode = NULL;
    // pipe is set to pipe
    read_file->pipe = pipe;
    //fd_new = open_standard_IO(ctx,new_file->type);
    //ret_fd = fd_new;
    // give the file descriptor to read file
    current->files[fd[0]] = read_file;

    // the pipe of the write file is PIPE
    write_file->type = PIPE;
    // give the mode to the WRITE
    write_file->mode = O_WRITE;
    // initilasie tht offset to 0
    write_file->offp = 0;
    //the function for reading is pipe_read
    write_file->fops->read = pipe_read;
    // the function for writing is pipe_write
    write_file->fops->write = pipe_write;
    //write_file->fops->lseek = do_lseek_regular;
    // the function for close is generic_close
    write_file->fops->close = generic_close;
    // set the refcount to 1
    write_file->ref_count = 1;
    // set the inode to NULL
    write_file->inode = NULL;
    // set the pipe to pipe
    write_file->pipe = pipe;
    //fd_new = open_standard_IO(ctx,new_file->type);
    //ret_fd = fd_new;
    // give the file descriptor to write file
    current->files[fd[1]] = write_file;
    //pipe->pipe_buff = buffer;

    return 0;
}