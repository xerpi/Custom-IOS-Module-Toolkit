/*   
	IOS Tester, tests communication with custom IOS module for Wii.
    Copyright (C) 2008 neimod.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*******************************************************************************
 *
 * syscalls.c - IOS syscalls
 *
 *******************************************************************************
 *
 *
 * v1.0 - 26 July 2008				- initial release by neimod
 * v1.1 - 5 September 2008			- prepared for public release
 *
 */

/*
   NOTE: Syscalls are different across different IOS versions. Current syscalls are for IOS31.

   Best would be to first check what current IOS is running, and then map the OS functions
   to the right syscall number.
 */

extern void syscall_9(void);
extern void syscall_a(void);
extern void syscall_e(void);
extern void syscall_16(void);
extern void syscall_18(void);
extern void syscall_1b(void);
extern void syscall_2a(void);
extern void syscall_37(void);
extern void syscall_3f(void);
extern void syscall_40(void);
extern void syscall_50(void);
extern void syscall_3c(void);


void os_thread_set_priority(unsigned int priority)
{
	void (*syscall)(unsigned int) = (void (*)(unsigned int))syscall_9;

	syscall(priority);
}

unsigned int os_message_queue_create(void* ptr, unsigned int id)
{
	unsigned int (*syscall)(void*, unsigned int) = (unsigned int (*)(void*, unsigned int))syscall_a;

	return syscall(ptr, id);
}

unsigned int os_message_queue_receive(unsigned int queue, unsigned int* message, unsigned int flags)
{
	unsigned int (*syscall)(unsigned int, unsigned int*, unsigned int) = (unsigned int (*)(unsigned int, unsigned int*, unsigned int))syscall_e;

	return syscall(queue, message, flags);
}


unsigned int os_heap_create(void* ptr, unsigned int size)
{
	unsigned int (*syscall)(void*, unsigned int) = (unsigned int (*)(void*, unsigned int))syscall_16;

	return syscall(ptr, size);
}

void* os_heap_alloc(unsigned int heap, unsigned int size)
{
	void* (*syscall)(unsigned int, unsigned int) = (void* (*)(unsigned int, unsigned int))syscall_18;

	return syscall(heap, size);
}




unsigned int os_device_register(const char* devicename, unsigned int queuehandle)
{
	unsigned int (*syscall)(const char*, unsigned int) = (unsigned int (*)(const char*, unsigned int))syscall_1b;

	return syscall(devicename, queuehandle);
}


void os_message_queue_ack(void* message, int result)
{
	void (*syscall)(void*, int) = (void (*)(void*, int))syscall_2a;

	syscall(message, result);
}

void os_sync_before_read(void* ptr, unsigned int size)
{
	void (*syscall)(void*, unsigned int) = (void (*)(void*, unsigned int))syscall_3f;

	syscall(ptr, size);
}

void os_sync_after_write(void* ptr, unsigned int size)
{
	void (*syscall)(void*, unsigned int) = (void (*)(void*, unsigned int))syscall_40;

	syscall(ptr, size);
}



void os_syscall_50(unsigned int unknown)
{
	void (*syscall)(unsigned int) = (void (*)(unsigned int))syscall_50;

	syscall(unknown);
}


