#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#define ERROR_1 -1
#define S_CLOSE false
#define S_OPEN true
#define S_READ true
#define S_WRITE true
#define S_FILESIZE true

#include "threads/synch.h"
#include "threads/thread.h"

void syscall_init(void);
static void halt(void);
void filling_array_t(int t_id, int index);
int wait(int t_id);

struct p_arr
{
	struct thread *thr;
	int tid;
	bool is_used;
};

#endif /* userprog/syscall.h */
