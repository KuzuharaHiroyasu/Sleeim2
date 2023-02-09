/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup RENESAS_INTERFACES
 * @defgroup RM_COMMS_API Communicatons Middleware Interface
 * @brief Interface for Communications Middleware functions.
 *
 * @section RM_COMMS_API_Summary Summary
 * The Communications interface provides multiple communications functionality.
 *
 * The Communications interface can be implemented by:
 * - @ref RM_COMMS_I2C
 *
 * @{
 **********************************************************************************************************************/

#ifndef RM_COMMS_API_H_
#define RM_COMMS_API_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
 #include <stddef.h>
 #include <stdbool.h>
 #include "r_cg_macrodriver.h"
 #include "r_fsp_error.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Event in the callback function */
typedef enum e_rm_comms_event
{
    RM_COMMS_EVENT_OPERATION_COMPLETE = 0,
    RM_COMMS_EVENT_ERROR,
} rm_comms_event_t;

/** Struct to pack params for writeRead */
typedef struct st_rm_comms_write_read_params
{
    uint8_t * p_src;
    uint8_t * p_dest;
    uint8_t   src_bytes;
    uint8_t   dest_bytes;
} rm_comms_write_read_params_t;

/** Communications middleware callback parameter definition */
typedef struct st_rm_comms_callback_args
{
    void const     * p_context;
    rm_comms_event_t event;
} rm_comms_callback_args_t;

/** Communications middleware configuration block */
typedef struct st_rm_comms_cfg
{
    uint32_t semaphore_timeout;                             ///< timeout for callback.
    void (* p_callback)(rm_comms_callback_args_t * p_args); ///< Pointer to callback function, mostly used if using non-blocking functionality.
    void const * p_lower_level_cfg;                         ///< Pointer to lower level driver configuration structure.
    void const * p_extend;                                  ///< Pointer to extended configuration by instance of interface.
    void const * p_context;                                 ///< Pointer to the user-provided context
} rm_comms_cfg_t;

/** Communications control block.  Allocate an instance specific control block to pass into the Communications API calls.
 * @par Implemented as
 * - rm_comms_i2c_instance_ctrl_t
 */
typedef void rm_comms_ctrl_t;

/** COMM APIs */
typedef struct st_rm_comms_api
{
    /** Open driver.
     * @par Implemented as
     * - @ref RM_COMMS_I2C_Open()
     *
     * @param[in]  p_ctrl   Pointer to control structure.
     * @param[in]  p_cfg    Pointer to configuration structure.
     */
    fsp_err_t (* open)(rm_comms_ctrl_t * const p_ctrl, rm_comms_cfg_t const * const p_cfg);

    /** Close driver.
     * @par Implemented as
     * - @ref RM_COMMS_I2C_Close()
     *
     * @param[in]  p_ctrl       Pointer to control structure.
     */
    fsp_err_t (* close)(rm_comms_ctrl_t * const p_ctrl);

    /** Read data.
     * @par Implemented as
     * - @ref RM_COMMS_I2C_Read()
     *
     * @param[in]  p_ctrl   Pointer to control structure.
     * @param[in] 　p_dest   Pointer to the location to store read data.
     * @param[in] 　bytes    Number of bytes to read.
     */
    fsp_err_t (* read)(rm_comms_ctrl_t * const p_ctrl, uint8_t * const p_dest, uint32_t const bytes);

    /** Write data.
     * @par Implemented as
     * - @ref RM_COMMS_I2C_Write()
     *
     * @param[in]  p_ctrl   Pointer to control structure.
     * @param[in]  p_src    Pointer to the location to get write data from.
     * @param[in]  bytes    Number of bytes to write.
     */
    fsp_err_t (* write)(rm_comms_ctrl_t * const p_ctrl, uint8_t * const p_src, uint32_t const bytes);

    /** Write bytes over comms followed by a read, will have a struct for params.
     * @par Implemented as
     * - @ref RM_COMMS_I2C_WriteRead()
     *
     * @param[in]  p_ctrl   Pointer to control structure.
     * @param[in]  write_read_params    Parameters structure.
     */
    fsp_err_t (* writeRead)(rm_comms_ctrl_t * const p_ctrl, rm_comms_write_read_params_t write_read_params);
} rm_comms_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_rm_comms_instance
{
    rm_comms_ctrl_t      * p_ctrl;
    rm_comms_cfg_t const * p_cfg;
    rm_comms_api_t const * p_api;
} rm_comms_instance_t;

#endif                                 /* RM_COMMS_API_H_ */

/*******************************************************************************************************************//**
 * @} (end addtogroup RM_COMMS_API)
 **********************************************************************************************************************/
