#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/synch.h"
#include "threads/thread.h"

int attr;
tid_t process_execute(const char *file_name);
int process_wait(tid_t);
void process_exit(void);
void process_activate(void);
bool compare(void *a, void *b);

struct waiting
{
	struct semaphore wait[10];
	struct thread *parent[10];
};
struct w_node
{
	struct semaphore swait;
	struct thread *parent;
	struct list_elem elem;
	int id;
};

struct storage
{
	struct thread *proc;
	int pid;
};

#endif /* userprog/process.h */