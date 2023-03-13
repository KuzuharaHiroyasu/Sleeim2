/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_ob1203_if.h"

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global variables
 *********************************************************************************************************************/
fsp_err_t rm_ob1203_delay_ms(rm_ob1203_ctrl_t * const p_ctrl, uint32_t const delay_ms);
fsp_err_t rm_ob1203_irq_open(rm_ob1203_ctrl_t * const p_api_ctrl);
fsp_err_t rm_ob1203_irq_close(rm_ob1203_ctrl_t * const p_api_ctrl);
void      rm_ob1203_irq_callback(external_irq_callback_args_t * p_args);

/**********************************************************************************************************************
 * Private (static) variables and functions
 *********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Delay some milliseconds.
 *
 * @retval FSP_SUCCESS              successfully configured.
 **********************************************************************************************************************/
fsp_err_t rm_ob1203_delay_ms (rm_ob1203_ctrl_t * const p_ctrl, uint32_t const delay_ms)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    /* Software delay */
//    R_OB1203_SoftwareDelay(delay_ms, OB1203_DELAY_MILLISECS);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Open IRQ driver and set callback function.
 *
 * @retval FSP_SUCCESS              successfully configured.
 **********************************************************************************************************************/
fsp_err_t rm_ob1203_irq_open (rm_ob1203_ctrl_t * const p_api_ctrl)
{
#ifndef RM_OB1203_CFG_IRQ_ENABLE
    fsp_err_t err = FSP_SUCCESS;
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;
    external_irq_instance_t   * p_irq  = (external_irq_instance_t *) p_ctrl->p_irq_instance;

    /* Open IRQ driver */
    p_irq->p_sstart();
#endif

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Close IRQ driver.
 *
 * @retval FSP_SUCCESS              successfully configured.
 **********************************************************************************************************************/
fsp_err_t rm_ob1203_irq_close (rm_ob1203_ctrl_t * const p_api_ctrl)
{
#if RM_OB1203_CFG_IRQ_ENABLE
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;
    external_irq_instance_t   * p_irq  = (external_irq_instance_t *) p_ctrl->p_irq_instance;

    /* Close IRQ driver */
    p_irq->p_stop();
#endif
    return FSP_SUCCESS;
}

void rm_ob1203_irq_callback (external_irq_callback_args_t * p_args)
{
#if RM_OB1203_CFG_IRQ_ENABLE
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_args->p_context;
    rm_ob1203_callback_args_t   ob1203_callback_args;

    /* Set context */
    ob1203_callback_args.p_context = p_ctrl->p_context;

    /* Set event */
    switch (p_ctrl->p_mode->mode_irq)
    {
        case RM_OB1203_OPERATION_MODE_LIGHT:
        {
            ob1203_callback_args.event = RM_OB1203_EVENT_THRESHOLD_CROSSED;
            break;
        }

        case RM_OB1203_OPERATION_MODE_PROXIMITY:
        {
            if (RM_OB1203_PROX_INTERRUPT_TYPE_LOGIC == p_ctrl->p_mode->prox_interrupt_type)
            {
                ob1203_callback_args.event = RM_OB1203_EVENT_OBJECT_NEAR;
            }
            else
            {
                /* RM_OB1203_PROX_INTERRUPT_TYPE_NORMAL */
                ob1203_callback_args.event = RM_OB1203_EVENT_THRESHOLD_CROSSED;
            }

            break;
        }

        case RM_OB1203_OPERATION_MODE_PPG:
        {
            ob1203_callback_args.event = RM_OB1203_EVENT_MEASUREMENT_COMPLETE;
            break;
        }

        default:
        {
            ob1203_callback_args.event = RM_OB1203_EVENT_ERROR;
            break;
        }
    }

    if (NULL != p_ctrl->p_irq_callback)
    {
        /* Call callback function */
        p_ctrl->p_irq_callback(&ob1203_callback_args);
    }
#endif
}
