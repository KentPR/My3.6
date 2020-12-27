#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "process.h"
#include "threads/vaddr.h"
#include "pagedir.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include <string.h>
#include "devices/input.h"
#include "lib/kernel/stdio.h"

static void syscall_handler(struct intr_frame *);

struct storage store[20];

int count_files = 0;
int count = 0;

void filling_eax(struct intr_frame *f, int value)
{
	f->eax = value;
}

static void halt(void)
{
	shutdown_power_off();
}

void exit(int err_type)
{
	attr = err_type;
	thread_exit(); //завершение программы
}

void filling_last(int p_id)
{
	store[count].pid = p_id;
	store[count].proc = thread_current();
	count++;
}

int wait(int p_id)
{
	for (int i = 0; i < count; i++)
	{
		if (compare(store[i].proc, thread_current()) && compare(store[i].pid, p_id))
			return ERROR_1;
	}
	filling_last(p_id);
	return process_wait(p_id);
}

struct file *open_f(const char *file)
{
	struct file *file_buf = filesys_open(file);
	if (compare(NULL, file_buf))
		return ERROR_1;
	else
		return file_buf;
}

void open1(const char *file, int thid, struct intr_frame *f)
{

	if (open_f(file) == ERROR_1)
	{
		f->eax = ERROR_1;
		thid = ERROR_1;
		return;
	}

	thread_current()->Fbox[thread_current()->file_cnt] = open_f(file);
	thid = thread_current()->file_cnt + CNT;
	f->eax = thid;
	thread_current()->opened[thread_current()->file_cnt] = true;
	thread_current()->file_cnt++;
}

void file_size(int *ptr, int thid, struct intr_frame *f)
{
	if (file_length(thread_current()->Fbox[*ptr]) == NULL)
	{
		f->eax = ERROR_1;
		thid = ERROR_1;
	}
	thid = file_length(thread_current()->Fbox[*ptr]);
	f->eax = file_length(thread_current()->Fbox[*ptr]);
}
void close(int *ptr)
{
	int fd = *ptr;
	struct file *fdd = thread_current()->Fbox[fd];
	for (int i = 0; i < thread_current()->file_cnt; i++)
	{
		if (compare(fdd, 0))
		{
			file_close(fdd);
			fdd = 0;
		}
	}
}

struct file *find_file1(int fd, bool attribute)
{
	struct file *found_file;
	for (int i = 0; i < thread_current()->file_cnt; i++)
	{
		if (thread_current()->opened[i])
		{
			found_file = thread_current()->Fbox[i];
			if (attribute == false)
				thread_current()->opened[i] = false;

			return found_file;
		}
	}
	return NULL;
}

int read(int fd, void *buffer, unsigned int size, int thid, struct intr_frame *f)
{
	char *buff = (char *)buffer;

	if (!compare(fd, 0))
	{
		if (find_file1(fd, true) == NULL)
		{
			thid = ERROR_1;
			f->eax = ERROR_1;
			return;
		}
	}
	else
	{
		for (int i = 0; i < size; i++)
		{
			buff[i] = input_getc();
			if (compare('\n', buff[i]))
				break;
		}
	}
	thid = file_read(find_file1(fd, true), buffer, size);
	f->eax = thid;
}

int write(int fd, const void *buffer, unsigned int size, int thid, struct intr_frame *f)
{
	char *buff = (const char *)buffer;
	if (1 == fd)
	{
		putbuf(buffer, size);
		return size;
	}
	else
	{
		if (find_file1(fd, true) == NULL)
		{
			thid = ERROR_1;
			f->eax = ERROR_1;
			return;
		}
	}
	thid = file_read(find_file1(fd, true), buffer, size);
	f->eax = thid;
}

bool validate_usr_pointer(void *esp)
{
	if (is_user_vaddr(esp) && pagedir_get_page(thread_current()->pagedir, esp))
		return 0;
	return 1;
}

bool arg_check(int *ptr, int i)
{
	//ptr += i;
	int *pr = *(ptr + i);
	switch (i)
	{
	case 2:
	{
		if ((
				compare(pr, NULL)) ||
			is_kernel_vaddr(pr) ||
			compare(pagedir_get_page(thread_current()->pagedir, pr), NULL))
			return true;
		else
			return false;
		break;
	}
	case 1:
	{
		if (
			compare(pr, NULL) || compare(pagedir_get_page(thread_current()->pagedir, pr), NULL) || is_kernel_vaddr(pr))
			return true;
		else
			return false;
		break;
	}
	case 0:
	{
		if (is_user_vaddr(pr) && pagedir_get_page(thread_current()->pagedir, pr))
			return false;
		return true;
		break;
	}
	}
}

void exec(int *arar, int thid, struct intr_frame *f)
{
	thid = process_execute((arar)[1]);
	if (compare((strcmp((arar)[1], "no-such-file")), false) && compare(wait(thid), ERROR_1))
	{
		thid = ERROR_1;
	}
	filling_eax(f, thid);
	return;
}

void create(int *ptr, int thid, struct intr_frame *f)
{
	thid = filesys_create(*(ptr + 1), *(ptr + 2));
	filling_eax(f, thid);
}

void remove(int *ptr, int thid, struct intr_frame *f)
{
	thid = filesys_remove(*(ptr + 1));
	filling_eax(f, thid);
}

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler(struct intr_frame *f)
{
	int thid = 0;
	int cntr;
	struct file *file_buf;
	int *arar = (int *)f->esp;

	if (validate_usr_pointer(arar))
	{
		exit(ERROR_1);
		return;
	}

	switch (*arar)
	{
	case SYS_HALT:
		halt();
		break;
	case SYS_EXIT:
		exit(*(arar + 1));
		break;
	case SYS_EXEC:
		if (arg_check(arar, 1))
		{
			exit(ERROR_1);
			return;
		}
		exec(arar, &thid, f);
		break;
	case SYS_WAIT:
		thid = wait(*(arar + 1));
		filling_eax(f, thid);
		break;
	case SYS_CREATE:
		if (arg_check(arar, 1))
		{
			exit(ERROR_1);
			return;
		}
		create(arar, &thid, f);
		break;
	case SYS_REMOVE:
		if (arg_check(arar, 1))
		{
			exit(ERROR_1);
			return;
		}
		remove(arar, &thid, f);
		break;
	case SYS_OPEN:
		if (arg_check(arar, 1))
		{
			exit(ERROR_1);
			return;
		}
		open1(*(arar + 1), &thid, f);
		break;
	case SYS_CLOSE:
		//nocheck
		file_buf = find_file1(*(arar + 1), false);
		if (file_buf == NULL)
			break;

		file_close(file_buf);
		break;
	case SYS_FILESIZE:
		//nocheck
		file_buf = find_file1(*(arar + 1), true);
		if (file_buf == NULL)
			thid = -1;
		else
			thid = file_length(file_buf);

		filling_eax(f, thid);
		break;
	case SYS_READ:
		if (arg_check(arar, 2))
		{
			exit(ERROR_1);
			return;
		}
		thid = read(*(arar + 1), *(arar + 2), *(arar + 3), &thid, f);
		break;
	case SYS_WRITE:
		if (arg_check(arar, 2))
		{
			exit(ERROR_1);
			return;
		}
		thid = write(*(arar + 1), *(arar + 2), *(arar + 3), &thid, f);
		break;
	default:
		printf("system call!\n");
		thread_exit();
		break;
	}
}