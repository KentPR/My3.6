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
struct p_arr process_array[CNT];

static void halt(void)
{
	shutdown_power_off();
}

void exit(int err_type)
{
	attr = err_type;
	thread_exit(); //завершение программы
}

int search_for_free_place(void)
{
	for (int i = 0; i < CNT; i++)
	{
		if (process_array[i].is_used != true)
		{
			process_array[i].is_used = true;
			return i;
		}
	}
}

void filling_array_t(int t_id, int index)
{
	process_array[index].thr = thread_current();
	process_array[index].tid = t_id;
}

int wait(int t_id)
{
	int i = 0;
	while (process_array[i].is_used == true)
	{
		if (compare(process_array[i].thr, thread_current()) && compare(process_array[i].tid, t_id))
			return ERROR_1;
		i++;
	}
	filling_array_t(t_id, search_for_free_place());
	return process_wait(t_id);
}

struct file *open_f(const char *file)
{
	struct file *file_buf = filesys_open(file);
	if (compare(NULL, file_buf))
		return ERROR_1;
	else
		return file_buf;
}

void open(const char *file, int thid, struct intr_frame *f)
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
	struct file *fdd = thread_current()->Fbox[*ptr];
	for (int i = 0; i < thread_current()->file_cnt; i++)
	{
		if (compare(fdd, 0))
		{
			file_close(fdd);
			fdd = 0;
		}
	}
}

struct file *search_f(bool system_call)
{
	for (int i = 0; i < thread_current()->file_cnt; i++)
	{
		if (thread_current()->opened[i])
		{
			if (system_call == false)
				thread_current()->opened[i] = false;

			return thread_current()->Fbox[i];
		}
	}
	return NULL;
}

void read(int fd, void *buffer, unsigned int size, int thid, struct intr_frame *f)
{
	if (!compare(fd, 0))
	{
		if (search_f(S_READ) == NULL)
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
			*((char *)buffer + i) = input_getc();
			if (compare('\n', *((char *)buffer + i)))
				break;
		}
	}
	thid = file_read(search_f(S_READ), buffer, size);
	f->eax = thid;
	return;
}

write(int fd, const void *buffer, unsigned int size, int thid, struct intr_frame *f)
{
	if (compare(fd, S_WRITE))
	{
		putbuf(buffer, size);
		thid = size;
		f->eax = thid;
		return;
	}
	else
	{
		if (search_f(S_WRITE) == NULL)
		{
			thid = ERROR_1;
			f->eax = ERROR_1;
			return;
		}
	}
	thid = file_read(search_f(S_WRITE), buffer, size);
	f->eax = thid;
	return;
}

bool validate_usr_pointer(void *esp)
{
	if (is_user_vaddr(esp) && pagedir_get_page(thread_current()->pagedir, esp))
		return 0;
	return 1;
}

bool arg_check(int *ptr, int i)
{
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
	f->eax = thid;
	;
	return;
}

void create(int *ptr, int thid, struct intr_frame *f)
{
	thid = filesys_create(*(ptr + 1), *(ptr + 2));
	f->eax = thid;
	;
}

void remove(int *ptr, int thid, struct intr_frame *f)
{
	thid = filesys_remove(*(ptr + 1));
	f->eax = thid;
	;
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
		f->eax = thid;
		;
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
		open(*(arar + 1), &thid, f);
		break;
	case SYS_CLOSE:
		//nocheck
		file_buf = search_f(S_CLOSE);
		if (file_buf == NULL)
			break;

		file_close(file_buf);
		break;
	case SYS_FILESIZE:
		//nocheck
		file_buf = search_f(S_FILESIZE);
		if (file_buf == NULL)
			thid = -1;
		else
			thid = file_length(file_buf);

		f->eax = thid;
		;
		break;
	case SYS_READ:
		if (arg_check(arar, 2))
		{
			exit(ERROR_1);
			return;
		}
		read(*(arar + 1), *(arar + 2), *(arar + 3), &thid, f);
		break;
	case SYS_WRITE:
		if (arg_check(arar, 2))
		{
			exit(ERROR_1);
			return;
		}
		write(*(arar + 1), *(arar + 2), *(arar + 3), &thid, f);
		break;
	default:
		printf("system call!\n");
		thread_exit();
		break;
	}
}