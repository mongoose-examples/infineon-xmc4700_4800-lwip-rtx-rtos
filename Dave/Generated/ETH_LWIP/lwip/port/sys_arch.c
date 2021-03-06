/*********************************************************************************************************************
* DAVE APP Name : ETH_LWIP       APP Version: 4.1.4
*
* NOTE:
* This file is generated by DAVE. Any manual modification done to this file will be lost when the code is regenerated.
*********************************************************************************************************************/

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */


#include <string.h>
#include "CMSIS_RTOS/cmsis_rtos.h"

#include "../include/lwip/opt.h"
#include "../include/lwip/sys.h"
#include "../include/lwip/stats.h"
#include "../include/lwip/debug.h"

#if defined ( __CC_ARM )
int32_t errno;
#endif

#if !NO_SYS

#if (CMSIS_RTOS_RTX_PRIVCNT < 4)
#error "ETH_LWIP: requires at least 4 threads with user-provided stack size when running in an OS environment"
#endif

#if (CMSIS_RTOS_RTX_PRIVSTKSIZE < 2560)
#error "ETH_LWIP: requires at least 2560 bytes of stack size for threads with user-provided stack size"
#endif

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  memset(sem, 0, sizeof(sys_sem_t));
  sem->def.semaphore = sem->cb;

  sem->id = osSemaphoreCreate(&sem->def, count);

  if (sem->id == NULL)
  {
    SYS_STATS_INC(sem.err);
    return ERR_MEM;
  }
  else
  {
    SYS_STATS_INC_USED(sem);
    return ERR_OK;
  }
}

void sys_sem_free(sys_sem_t *sem)
{
  if (sem->id != NULL)
  {
    osSemaphoreDelete(sem->id);
    SYS_STATS_DEC(sem.used);
  }

}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  int32_t  count;
  uint32_t tick_start;
  uint32_t tick_stop;
  uint32_t tick_elapsed;

  tick_start = osKernelSysTick();

  if (timeout == 0)
  {
    count = osSemaphoreWait(sem->id, osWaitForever);
  }
  else
  {
    count = osSemaphoreWait(sem->id, timeout);
  }

  tick_stop = osKernelSysTick();

  if (count < 1)
  {
    return SYS_ARCH_TIMEOUT;
  }

  /* Take care of wrap-around. */
  if (tick_stop >= tick_start)
  {
    tick_elapsed = tick_stop - tick_start;
  }
  else
  {
    tick_elapsed = (UINT32_MAX - tick_start) + tick_stop;
  }

  return (u32_t)(1000U * ((float)tick_elapsed / osKernelSysTickFrequency));
}

void sys_sem_signal(sys_sem_t *sem)
{
  if (sem->id != NULL)
  {
    osSemaphoreRelease(sem->id);
  }
}

err_t sys_mutex_new(sys_mutex_t *mutex)
{
  memset(mutex, 0, sizeof(sys_mutex_t));
  mutex->def.mutex = mutex->cb;

  mutex->id = osMutexCreate(&mutex->def);

  if (mutex->id == NULL)
  {
    SYS_STATS_INC(mutex.err);
    return ERR_MEM;
  }
  else
  {
    SYS_STATS_INC_USED(mutex);
    return ERR_OK;
  }
}

void sys_mutex_free(sys_mutex_t *mutex)
{
  if (mutex->id != NULL)
  {
    osMutexDelete(mutex->id);
    SYS_STATS_DEC(mutex.used);    
  }
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
  if (mutex->id != NULL)
  {
    osMutexWait(mutex->id, osWaitForever);
  }
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
  if (mutex->id != NULL)
  {
    osMutexRelease(mutex->id);
  }
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  if (size > SYS_MBOX_SIZE) {
    return ERR_MEM;
  }

  memset(mbox, 0, sizeof(sys_mbox_t));
  mbox->def.pool = mbox->q;
  mbox->def.queue_sz = size;

  mbox->id = osMessageCreate(&mbox->def, NULL);

  if (mbox->id == NULL)
  {
    SYS_STATS_INC(mbox.err);
    return ERR_MEM;
  }
  else
  {
    SYS_STATS_INC_USED(mbox);
    return ERR_OK;
  }
}

void sys_mbox_free(sys_mbox_t *mbox)
{
  if (mbox->id != NULL)
  {
    osMessageGet(mbox->id, 0);

    mbox->id = NULL;
    SYS_STATS_DEC(mbox.used);
  }    
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  if (mbox->id != NULL)
  {
    osMessagePut(mbox->id, (uint32_t)msg, osWaitForever);
  }
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  osStatus status;

  if (mbox->id != NULL)
  {
    status = osMessagePut(mbox->id, (uint32_t)msg, 0);
    if (status != osOK)
    {
      return ERR_MEM;
    }
    return ERR_OK;
  }
  else
  {
    return ERR_ARG;
  }
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  osEvent  event;
  uint32_t tick_start;
  uint32_t tick_stop;
  uint32_t tick_elapsed;

  tick_start = osKernelSysTick();

  if (timeout == 0)
  {
    event = osMessageGet(mbox->id, osWaitForever);
  } else
  {
    event = osMessageGet(mbox->id, timeout);
  }

  if (event.status != osEventMessage)
  {
    return SYS_ARCH_TIMEOUT;
  }

  if (msg != NULL)
  {
    *msg = event.value.p;
  }

  tick_stop = osKernelSysTick();

  /* Take care of wrap-around. */
  if (tick_stop >= tick_start)
  {
    tick_elapsed = tick_stop - tick_start;
  }
  else
  {
    tick_elapsed = (UINT32_MAX - tick_start) + tick_stop;
  }

  return (u32_t)(1000U * ((float)tick_elapsed / osKernelSysTickFrequency));
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  osEvent event;

  if (mbox->id != NULL)
  {
    event = osMessageGet(mbox->id, 0);
    if (event.status != osEventMessage) 
    {
      return SYS_MBOX_EMPTY;
    }

    if (msg != NULL) 
    {
      *msg = event.value.p;
    }

    return ERR_OK;
  }
  else
  {
    return ERR_ARG;
  }
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
  LWIP_UNUSED_ARG(name);

  osThreadId threadId;
  osThreadDef_t threadDef;

  threadDef.pthread = (os_pthread)function;
  threadDef.tpriority = (osPriority)prio;
  threadDef.instances = 1;
  threadDef.stacksize = stacksize;

  threadId = osThreadCreate(&threadDef, arg);
  LWIP_PLATFORM_ASSERT(threadId != NULL);

  return threadId;
}

#if SYS_LIGHTWEIGHT_PROT

static sys_mutex_t lwip_mutex;

sys_prot_t sys_arch_protect(void)
{
  sys_mutex_lock(&lwip_mutex);
  return 0;
}

void sys_arch_unprotect(sys_prot_t pval)
{
  LWIP_UNUSED_ARG(pval);
  sys_mutex_unlock(&lwip_mutex);
}

#endif /* SYS_LIGHTWEIGHT_PROT */


#endif /* !NO_SYS */

extern uint32_t os_time; // Time keping in RTOS RTX systick handler

uint32_t sys_now(void)
{
#if CMSIS_RTOS_RTX_TICK == 1000
  return os_time;
#else
  return (os_time * (CMSIS_RTOS_RTX_TICK / 1000));
#endif  
}

void sys_init(void)
{
#if !NO_SYS
#if SYS_LIGHTWEIGHT_PROT
  sys_mutex_new(&lwip_mutex);
#endif
#endif /* !NO_SYS */
}

