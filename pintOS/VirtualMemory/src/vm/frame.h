#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdbool.h>
#include "threads/synch.h"

struct frame 
  {
    struct lock lock;           
    void *base;                
    struct page *page;         
  };

void frame_init (void);

struct frame *frame_alloc_and_lock (struct page *);
void frame_lock (struct page *);

void frame_free (struct frame *);
void frame_unlock (struct frame *);

#endif 
