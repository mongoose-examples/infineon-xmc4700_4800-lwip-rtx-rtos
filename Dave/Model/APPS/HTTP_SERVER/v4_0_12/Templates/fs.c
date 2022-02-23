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

#include "ETH_LWIP/lwip/include/lwip/opt.h"
#include "ETH_LWIP/lwip/include/lwip/def.h"
#include "fs.h"
#include "fsdata.h"
#include <string.h>
#include "http_server.h"


#if !HTTPD_FAT_FS
#include "fsdata.dat"
#endif

/*-----------------------------------------------------------------------------------*/
/* Define the number of open files that we can support. */
#ifndef LWIP_MAX_OPEN_FILES
#define LWIP_MAX_OPEN_FILES     10
#endif

/* Define the file system memory allocation structure. */
struct fs_table {
  struct fs_file file;
  u8_t inuse;
};
#if HTTPD_FAT_FS
/* Allocate file system memory */
struct fs_table fs_memory[LWIP_MAX_OPEN_FILES];
FIL fatFS_handle[LWIP_MAX_OPEN_FILES];
u8_t http_server_buff[LWIP_MAX_OPEN_FILES][HTTP_SERVER_BUFFER_SIZE];
static char file_name[HTTP_FILE_NAME_SIZE];
#endif /*HTTPD_FAT_FS*/

#if LWIP_HTTPD_CUSTOM_FILES
int fs_open_custom(struct fs_file *file, const char *name);
void fs_close_custom(struct fs_file *file);
#if LWIP_HTTPD_FS_ASYNC_READ
u8_t fs_canread_custom(struct fs_file *file);
u8_t fs_wait_read_custom(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
#endif /* LWIP_HTTPD_CUSTOM_FILES */

/*-----------------------------------------------------------------------------------*/
#if HTTPD_FAT_FS
static struct fs_file *
fs_malloc(void)
{
  int i;
  for(i = 0; i < LWIP_MAX_OPEN_FILES; i++) {
    if(fs_memory[i].inuse == 0) {
      fs_memory[i].inuse = 1;
      fs_memory[i].file.fatFS_File = &fatFS_handle[i];
      fs_memory[i].file.data = (char *)&http_server_buff[i][0];
      return(&fs_memory[i].file);
    }
  }
  return(NULL);
}


static void
fs_free(struct fs_file *file)
{
  int i;
  for(i = 0; i < LWIP_MAX_OPEN_FILES; i++) {

    if(fs_memory[i].file.fatFS_File == file->fatFS_File) {

      fs_memory[i].inuse = 0;
      break;
    }
  }
  return;
}
#endif /*HTTPD_FAT_FS*/
/*-----------------------------------------------------------------------------------*/
err_t
fs_open(struct fs_file *user_file, const char *name)
{

#if  HTTPD_FAT_FS
  struct fs_file * file;
  file = fs_malloc();
  if ((file == NULL) || (name == NULL)) {
     return ERR_ARG;
  }
  FILINFO fileStat;

#else
  const struct fsdata_file *f;
#endif
  if(user_file == NULL) {
    return ERR_VAL;
  }

#if LWIP_HTTPD_CUSTOM_FILES
  if (fs_open_custom(file, name)) {
    file->is_custom_file = 1;
    return ERR_OK;
  }
  file->is_custom_file = 0;
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#if  HTTPD_FAT_FS
    strcpy(file_name, HTTP_FATFS_DRIVE);
    strlcat(file_name, name, HTTP_FILE_NAME_SIZE);
    if(f_open(file->fatFS_File, file_name, FA_READ) == FR_OK)
    {
      f_stat(file_name,&fileStat);

      user_file->fatFS_File = file->fatFS_File;
      user_file->data = file->data;
      user_file->len = fileStat.fsize;
      user_file->index = 0;
      user_file->pextension = NULL;
      user_file->http_header_included = 0;//1;

      file->len = fileStat.fsize;
      file->index = 0;
    	  file->pextension = NULL;
    	  file->http_header_included = 1;
    	  return ERR_OK;
      }
    else
    {
      fs_free(file);
      return ERR_VAL;
    }
#else
  for (f = FS_ROOT; f != NULL; f = f->next) {
    if (!strcmp(name, (char *)f->name)) {
      user_file->data = (char *)f->data;
      user_file->len = f->len;
      user_file->index = f->len;
      user_file->pextension = NULL;
      user_file->http_header_included = f->http_header_included;
#if HTTPD_PRECALCULATED_CHECKSUM
      user_file->chksum_count = f->chksum_count;
      user_file->chksum = f->chksum;
#endif /* HTTPD_PRECALCULATED_CHECKSUM */
#if LWIP_HTTPD_FILE_STATE
      user_file->state = fs_state_init(file, name);
#endif /* #if LWIP_HTTPD_FILE_STATE */
      return ERR_OK;
    }
  }
  /* file not found */
#endif
  return ERR_VAL;
}

/*-----------------------------------------------------------------------------------*/
void
fs_close(struct fs_file *file)
{
#if LWIP_HTTPD_CUSTOM_FILES
  if (file->is_custom_file) {
    fs_close_custom(file);
  }
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#if LWIP_HTTPD_FILE_STATE
  fs_state_free(file, file->state);
#endif /* #if LWIP_HTTPD_FILE_STATE */
  LWIP_UNUSED_ARG(file);
#if HTTPD_FAT_FS
  f_close (file->fatFS_File);
  fs_free(file);
#endif

}
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_DYNAMIC_FILE_READ
#if LWIP_HTTPD_FS_ASYNC_READ
int
fs_read_async(struct fs_file *file, char *buffer, int count, fs_wait_cb callback_fn, void *callback_arg)
#else /* LWIP_HTTPD_FS_ASYNC_READ */
int
fs_read(struct fs_file *file, char *buffer, int count)
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
{
#if HTTPD_FAT_FS
	int result;
	f_read(file->fatFS_File, buffer, count, (UINT*)&result);
	if (result <=0) {
		return -1;
	}
	else {
		file->index += result;
		return result;
	}
#else

  int read;

  if(file->index == file->len) {
    return FS_READ_EOF;
  }
#if LWIP_HTTPD_FS_ASYNC_READ
#if LWIP_HTTPD_CUSTOM_FILES
  if (!fs_canread_custom(file)) {
    if (fs_wait_read_custom(file, callback_fn, callback_arg)) {
      return FS_READ_DELAYED;
    }
  }
#else /* LWIP_HTTPD_CUSTOM_FILES */
  LWIP_UNUSED_ARG(callback_fn);
  LWIP_UNUSED_ARG(callback_arg);
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#endif /* LWIP_HTTPD_FS_ASYNC_READ */

  read = file->len - file->index;
  if(read > count) {
    read = count;
  }

  MEMCPY(buffer, (file->data + file->index), read);
  file->index += read;

  return(read);
#endif
}
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_FS_ASYNC_READ
int
fs_is_file_ready(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg)
{
  if (file != NULL) {
#if LWIP_HTTPD_FS_ASYNC_READ
#if LWIP_HTTPD_CUSTOM_FILES
    if (!fs_canread_custom(file)) {
      if (fs_wait_read_custom(file, callback_fn, callback_arg)) {
        return 0;
      }
    }
#else /* LWIP_HTTPD_CUSTOM_FILES */
    LWIP_UNUSED_ARG(callback_fn);
    LWIP_UNUSED_ARG(callback_arg);
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
  }
  return 1;
}
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
/*-----------------------------------------------------------------------------------*/
int
fs_bytes_left(struct fs_file *file)
{
  return file->len - file->index;
}
