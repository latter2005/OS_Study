#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "devices/shutdown.h"
#include "devices/input.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "filesys/directory.h"
#include "filesys/inode.h"
#include "filesys/off_t.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
static void syscall_handler(struct intr_frame *);

struct lock filesys_lock;
struct file
  {
    struct inode *inode;        /* File's inode. */
    off_t pos;                  /* Current position. */
    bool deny_write;            /* Has file_deny_write() been called? */
  };
void syscall_init(void)
{
	lock_init(&filesys_lock);
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void valid_check(const void *esp)
{
	if (!is_user_vaddr(esp))
	{
		_exit(-1);
	}
}
static void
syscall_handler(struct intr_frame *f UNUSED)
{
	//printf("system call!\nnumber : %d\n", *(uint32_t*)(f->esp));
	//hex_dump(f->esp, f->esp, 100, 1);
	switch (*(uint32_t *)(f->esp)) {
	case SYS_HALT:
		_halt();
		break;
	case SYS_EXIT:
		valid_check(f->esp + 4);
		_exit(*(uint32_t *)(f->esp + 4));
		break;
	case SYS_EXEC:
	{
		valid_check(f->esp + 4);
		f->eax = (uint32_t)_exec((const char *)*(uint32_t*)(f->esp + 4));
		break;
	}
	case SYS_WAIT:
		valid_check(f->esp + 4);
		f->eax = _wait((pid_t)*(uint32_t *)(f->esp + 4));
		break;
	case SYS_CREATE:
		valid_check(f->esp + 4);
		valid_check(f->esp + 8);
		f->eax = _create((const char *)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
		break;
		break;
	case SYS_REMOVE:
		valid_check(f->esp + 4);
		f->eax = _remove((const char*)*(uint32_t *)(f->esp + 4));
		break;
	case SYS_OPEN:
		valid_check(f->esp + 4);
		f->eax = _open((const char*)*(uint32_t *)(f->esp + 4));
		break;
	case SYS_FILESIZE:
		valid_check(f->esp + 4);
		f->eax = _filesize((int)*(uint32_t *)(f->esp + 4));
		break;
	case SYS_READ:
		valid_check(f->esp + 4);
		valid_check(f->esp + 8);
		valid_check(f->esp + 12);
		f->eax = _read((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp + 12)));
		break;
	case SYS_WRITE:
		valid_check(f->esp + 4);
		valid_check(f->esp + 8);
		valid_check(f->esp + 12);
		f->eax = _write((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp + 12)));
		break;
	case SYS_SEEK:
		valid_check(f->esp + 4);
		valid_check(f->esp + 8);
		_seek((int)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
		break;
	case SYS_TELL:
		valid_check(f->esp + 4);
		f->eax = _tell((int*)*(uint32_t *)(f->esp + 4));
		break;
	case SYS_CLOSE:
		valid_check(f->esp + 4);
		_close((int*)*(uint32_t*)(f->esp + 4));
		break;
	}

	//thread_exit ();
}
void _halt()
{
	shutdown_power_off();
}

void _exit(int status)
{
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current() -> exit_status = status;
	thread_exit();
}

pid_t _exec(const char *cmdline)
{
	valid_check(cmdline);
	pid_t pid = process_execute(cmdline);
	return pid;
}

int _wait(pid_t pid)
{
	return process_wait(pid);
}

bool _create(const char *file, unsigned initial_size)
{
	if (file == NULL)
		_exit(-1);
	return filesys_create(file, initial_size);
}
bool _remove(const char *file)
{
	return filesys_remove(file);
}
int _open(const char *file)
{
	int i;
	struct file* fp;
	valid_check(file);
	lock_acquire(&filesys_lock);
	if (file == NULL)
	{
		lock_release(&filesys_lock);
		return -1;
	}
	fp = filesys_open(file);
	if (fp == NULL) {
		lock_release(&filesys_lock);
		return -1;
	}
	else
	{
		for (i = 3; i<128; i++)
		{
			if (thread_current()->fd[i] == NULL)
			{
				if(strcmp(thread_current()->name, file) == 0)
				{
					file_deny_write(fp);	
				}
				thread_current()->fd[i] = fp;
				lock_release(&filesys_lock);
				return i;
			}
		}
	}
	lock_release(&filesys_lock);
	return -1;
}
int _filesize(int fd)
{
	if (thread_current()->fd[fd] == NULL)
		_exit(-1);
	return file_length(thread_current()->fd[fd]);
}

int _write(int fd, const void *buffer, unsigned size)
{
	valid_check(buffer);
	lock_acquire(&filesys_lock);
	if (fd == 1)
	{
		putbuf(buffer, size);
		lock_release(&filesys_lock);
		return size;
	}
	else if (fd >2)
	{
		lock_release(&filesys_lock);	
		if(thread_current()->fd[fd]->deny_write)
		{
			file_deny_write(thread_current()->fd[fd]);
		}
		return file_write(thread_current()->fd[fd], buffer, size);
		
	}
	else
	{	lock_release(&filesys_lock);
		_exit(-1);
	}
}
int _read(int fd, void *buffer, unsigned size)
{
	int i;
	valid_check(buffer);
	lock_acquire(&filesys_lock);
	if (fd == 0) {
		for (i = 0; i<size; i++) {
			*((char*)(buffer + i)) = input_getc();
		}
		lock_release(&filesys_lock);
		return size;
	}
	else if (fd >2)
	{	
		lock_release(&filesys_lock);
		return file_read(thread_current()->fd[fd], buffer, size);
	}
	else
	{	
		//lock_release(&filesys_lock);
		_exit(-1);
	}
}
void _seek(int fd, unsigned position)
{
	struct file *f = thread_current()->fd[fd];
	if (f == NULL)
		_exit(-1);
	file_seek(f, position);
}
unsigned _tell(int fd)
{
	struct file *f = thread_current()->fd[fd];
	if (f == NULL)
		_exit(-1);
	return file_tell(f);
}
void _close(int fd)
{
	struct file *fp;
	if (thread_current()->fd[fd] == NULL)
	{
		_exit(-1);
	}
	fp = thread_current()->fd[fd];
	thread_current()->fd[fd] = NULL;
	return file_close(fp);
}
