/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO., LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 *
 * See LICENSE file for details.
 */

#pragma once

#include <string.h>
#include "esp_gmf_err.h"
#include "esp_log.h"
#include "esp_gmf_oal_mem.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define DEFAULT_GMF_AFE_CFG(__afe_manager, __event_cb, __event_ctx, __models)  {               \
    .afe_manager    = __afe_manager,                        /*   */                            \
    .models         = __models,                             /*   */                            \
    .delay_samples  = ESP_GMF_AFE_DEFAULT_DELAY_SAMPLES,    /* Default delay samples, 64ms */  \
    .wakeup_time    = ESP_GMF_AFE_DEFAULT_WAKEUP_TIME_MS,   /* Default wakeup duration */      \
    .wakeup_end     = ESP_GMF_AFE_DEFAULT_WAKEUP_END_MS,    /* Default wakeup end time */      \
    .vcmd_detect_en = false,                                /*   */                            \
    .vcmd_timeout   = ESP_GMF_AFE_DEFAULT_VCMD_TIMEOUT_MS,  /*   */                            \
    .mn_language    = "cn",                                 /* Default language `cn` */        \
    .event_cb       = __event_cb,                           /*   */                            \
    .event_ctx      = __event_ctx,                          /*   */                            \
}

/**
 * @brief  GMF argument types enumeration
 */
typedef enum {
    ESP_GMF_ARGS_TYPE_NIL    = 0x00,  /*!< No type or null value */
    ESP_GMF_ARGS_TYPE_UINT8  = 0x01,  /*!< Unsigned 8-bit integer */
    ESP_GMF_ARGS_TYPE_INT8   = 0x02,  /*!< Signed 8-bit integer */
    ESP_GMF_ARGS_TYPE_UINT16 = 0x03,  /*!< Unsigned 16-bit integer */
    ESP_GMF_ARGS_TYPE_INT16  = 0x04,  /*!< Signed 16-bit integer */
    ESP_GMF_ARGS_TYPE_UINT32 = 0x05,  /*!< Unsigned 32-bit integer */
    ESP_GMF_ARGS_TYPE_INT32  = 0x06,  /*!< Signed 32-bit integer */
    ESP_GMF_ARGS_TYPE_UINT64 = 0x07,  /*!< Unsigned 64-bit integer */
    ESP_GMF_ARGS_TYPE_INT64  = 0x08,  /*!< Signed 64-bit integer */
    ESP_GMF_ARGS_TYPE_FLOAT  = 0x09,  /*!< Single-precision floating point */
    ESP_GMF_ARGS_TYPE_DOUBLE = 0x0a,  /*!< Double-precision floating point */
    ESP_GMF_ARGS_TYPE_ARRAY  = 0x0b,  /*!< Array data type */
} esp_gmf_args_type_t;

/**
 * @brief  Test function with license comment preserved
 *
 * @note  This file tests license comment block detection
 *
 * @param[in]  param1  First parameter
 * @param[in]  param2  Second parameter
 *
 * @return
 *       - ESP_OK  On success
 */
esp_err_t test_function_with_license(int param1, int param2);

/**
 * @brief  Another test function
 *
 * @param[in]  value  Input value
 *
 * @return
 *       - 0  Result value
 */
int another_function(int value);

/**
 * @brief  Function after code
 */
void function_after_code(void);

/**
 * @brief  Parse JSON response and extract file URLs
 *
 * @param  json_str  JSON string to parse
 *
 * @return
 *       - 0  On success
 */
int parse_json_and_extract_urls(const char *json_str);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
