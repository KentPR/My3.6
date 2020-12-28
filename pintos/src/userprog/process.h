#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/synch.h"
#include "threads/thread.h"

int attr;
tid_t process_execute(const char *file_name);
void is_first_look(struct thread *th);
int search_for_free_index(void);
void filling_array(int id);
void upping_s(struct thread *parent);
int process_wait(tid_t);
void process_exit(void);
void process_activate(void);
bool compare(void *a, void *b);

struct w_arr
{
	struct semaphore sem;
	struct thread *parent_thr;
	bool is_used;
};

struct p_arr
{
	struct thread *thr;
	int tid;
	bool is_used;
};

#endif /* userprog/process.h */