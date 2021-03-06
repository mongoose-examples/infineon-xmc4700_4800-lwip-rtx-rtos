/**
 * @file http_server.h
 * @date 2016-11-07
 *
 * NOTE:
 * This file is generated by DAVE. Any manual modification done to this file will be lost when the code is regenerated.
 *
 * @cond
 ***********************************************************************************************************************
 * HTTP_SERVER v4.0.10 - Initializes HTTP server.
 *
 * Copyright (c) 2015-2016, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes
 * with Infineon Technologies AG (dave@infineon.com).
 ***********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2016-03-02:
 *     - Initial version for DAVEv4
 *
 * 2016-11-07:
 *     - Make post_buffer word aligned in HTTP_SERVER_POST_RESP_t
 *
 * @endcond
 *
 */

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "xmc_common.h"
#include <DAVE_common.h>
#include "ETH_LWIP/eth_lwip.h"
#include "fs.h"
#if HTTPD_FAT_FS
#include "FATFS/fatfs.h"
#endif
#include "httpd.h"
#include "http_server_conf.h"

/**********************************************************************************************************************
 * ENUMS
 **********************************************************************************************************************/
/**
 * @ingroup HTTP_SERVER_enumerations
 * @{
 */

/**
 * @brief Enum to describe the possible status values, returned
 *  by HTTP_SERVER init API.
 */
typedef enum HTTP_SERVER_STATUS
{
  HTTP_SERVER_STATUS_SUCCESS = 0U, /**< Status Success if initialization is successful */
  HTTP_SERVER_STATUS_FAILURE  /**< Status Failure if initialization is failed */
} HTTP_SERVER_STATUS_t;
/**
 * @}
 */

/**
 * @ingroup HTTP_SERVER_datastructures
 * @{
 */
#if LWIP_HTTPD_SUPPORT_POST
/**
 * @brief Structure for holding HTTP POST data. This structure is available only when POST support is enabled in the GUI.
 * Pointer of an object of this structure will be passed as argument to the POST finished callback function.
 */
typedef struct HTTP_SERVER_POST_RESP
{
  uint8_t post_buffer[HTTP_SERVER_POST_BUFFER_SIZE];   /**< Buffer for storing the data received as part of POST
	                                                            method*/
  char ** post_response_uri;                           /**< Pointer to the string where URI of the response should be
                                                            copied by the application in the callback function*/
  uint32_t post_data_size;                             /**< Size of POST data received*/
  uint16_t http_request_len;                           /**< Size of arguments received*/
  char post_target_uri[HTTP_SERVER_POST_URI_SIZE];     /**< Buffer for storing the URI addressed by the POST method*/
  char http_request_hdr[HTTP_SERVER_POST_HDR_SIZE];    /**< Buffer for request arguments received when the POST method
                                                            was processed*/
} HTTP_SERVER_POST_RESP_t;
#endif /*LWIP_HTTPD_SUPPORT_POST*/

/**
 * @brief Handler structure with pointer to initialization function.
 */
typedef struct HTTP_SERVER
{
  ETH_LWIP_t * lwip;                                          /**< Pointer to ETH_LWIP APP instance used for HTTP
                                                                   server implementation*/
#if HTTPD_FAT_FS
  FATFS_t * fatfs;                                            /**< Pointer to FATFS APP instance used by HTTP server */
#endif
#if LWIP_HTTPD_SUPPORT_POST
  void (*post_cbk)(HTTP_SERVER_POST_RESP_t * ptr_post_data);  /**< Callback function to be executed when POST method is
                                                                   completely received.
                                                                   This structure member is available only when POST is
                                                                   supported. */
#endif /*LWIP_HTTPD_SUPPORT_POST*/

} HTTP_SERVER_t;

/**
 * @}
 */

/***********************************************************************************************************************
 * API Prototypes
 **********************************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @ingroup HTTP_SERVER_apidoc
 * @{
 */

/**
 * @brief API to retrieve the version of the HTTP_SERVER APP.
 * @param None
 * @return DAVE_APP_VERSION_t Structure containing major version, minor version
 *         and patch version.<br>
 *
 * Example Usage:
 * @code
 * #include <DAVE.h>
 * //Example below, executes periodic lwIP function only if the version of the HTTP_SERVER APP is v4.0.x
 * //Value of x can be anything.
 * int main(void)
 * {
 *   HTTP_SERVER_STATUS_t init_status;
 *   DAVE_APP_VERSION_t http_server_version;
 *
 *   init_status = HTTP_SERVER_Init(&HTTP_SERVER_0);
 *   if(init_status == HTTP_SERVER_STATUS_SUCCESS)
 *   {
 *     http_server_version = HTTP_SERVER_GetAppVersion();
 *
 *     //Check the APP version
 *     if((http_server_version.major == 4) &&
 *       (http_server_version.minor == 0))
 *     {
 *       //Periodic function
 *       while(1)
 *       {
 *         sys_check_timeouts();
 *       }
 *     }
 *   }
 *   else
 *   {
 *     XMC_DEBUG("main: Application initialization failed");
 *     while(1U)
 *     {
 *     }
 *   }
 *   return 1U;
 * }
 *
 *  @endcode
 */
DAVE_APP_VERSION_t HTTP_SERVER_GetAppVersion(void);

/**
 * @brief API to initialize the LWIP software stack and other dependent code modules.
 *
 * @param  handle HTTP_SERVER APP handle pointer of type HTTP_SERVER_t*
 *
 * @return  HTTP_SERVER_STATUS_t<br>
 *          HTTP_SERVER_STATUS_SUCCESS: for successful LWIP initialization.<BR>
 *          HTTP_SERVER_STATUS_FAILURE: If LWIP initialization fails.<BR>
 *
 * Example Usage:
 * @code
 * #include <DAVE.h>
 * //Example below, executes periodic lwIP function after initializing HTTP_SERVER APP
 * //It responds with default HTTP response file generated by Dave.
 * int main(void)
 * {
 *   HTTP_SERVER_STATUS_t init_status;
 *
 *   init_status = HTTP_SERVER_Init(&HTTP_SERVER_0);
 *   if(init_status == HTTP_SERVER_STATUS_SUCCESS)
 *   {
 *       //Periodic function
 *       while(1)
 *       {
 *         sys_check_timeouts();
 *       }
 *   }
 *   else
 *   {
 *     XMC_DEBUG("main: Application initialization failed");
 *     while(1U)
 *     {
 *     }
 *   }
 *   return 1U;
 * }
 *
 *  @endcode
 */
HTTP_SERVER_STATUS_t HTTP_SERVER_Init(HTTP_SERVER_t* handle);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#include "http_server_extern.h"

#endif
