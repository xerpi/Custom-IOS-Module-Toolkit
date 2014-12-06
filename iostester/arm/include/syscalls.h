#ifndef IOS_SYSCALLS_H
#define IOS_SYSCALLS_H

/*******************************************************************************
 *
 * syscalls.h - IOS syscalls
 *
 *******************************************************************************
 *
 *
 * v1.0 - initial release
 *
 */


void os_thread_set_priority(unsigned int priority);
unsigned int os_message_queue_create(void* ptr, unsigned int id);
unsigned int os_message_queue_receive(unsigned int queue, unsigned int* message, unsigned int flags);
unsigned int os_heap_create(void* ptr, unsigned int size);
unsigned int os_device_register(const char* devicename, unsigned int queuehandle);
void os_message_queue_ack(void* message, int result);
void* os_heap_alloc(unsigned int heap, unsigned int size);
void os_sync_before_read(void* ptr, unsigned int size);
void os_sync_after_write(void* ptr, unsigned int size);
void os_syscall_50(unsigned int unknown);

#endif // IOS_SYSCALLS_H
