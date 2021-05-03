#include "base_usart.h"
#include "stm32f1xx_hal.h"
#include "stimer.h"

#include "usart.h"
#include "string.h"
struct USART_t usart1_s = {0,{0},0,0,0};



void USER_UART_Receive_Timeout(void)
{
	usart1_s.RecvCount = 0;
	usart1_s.RecvFalg  = 1;
}
uint8_t USER_Get_Usart_Receive_Flag(void)
{
	return usart1_s.RecvFalg;
}

void USER_Clear_Usart_Receive_Flag(void)
{
	usart1_s.RecvFalg = 0;
	
	memset(usart1_s.RxMes, 0, sizeof(char));
}

#if 0
void USER_UART_Receive(UART_HandleTypeDef *huart)
{
	uint8_t data = (uint8_t)(huart->Instance->DR);
	usart1_s.RecvBuffer[usart1_s.RecvCount++] = data;

	usart1_s.RecvLen = usart1_s.RecvCount;
	if(usart1_s.RecvCount >= USART_MAX_LEN)
	{
		usart1_s.RecvLen = usart1_s.RecvCount;
		usart1_s.RecvCount = 0;
		return ;
	}
	spTimerStart(STIMER_USART,50,1);
}
#endif

void USER_UART1_SendData(uint8_t *data,uint16_t len)
{

  HAL_UART_Transmit(&huart1,data,len,0xFF);

}
#if 0
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t isrflags   = READ_REG(huart->Instance->ISR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its;
  uint32_t errorflags;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
  if (errorflags == RESET)
  {
    /* UART in mode Receiver ---------------------------------------------------*/
    if(((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      //UART_Receive_IT(huart);
			/* add your code */
			USER_UART_Receive(huart);
      return;
    }
  }  

  /* If some errors occur */
  cr3its = READ_REG(huart->Instance->CR3);
  if(   (errorflags != RESET)
     && (   ((cr3its & USART_CR3_EIE) != RESET)
         || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)) )
  {
    /* UART parity error interrupt occurred -------------------------------------*/
    if(((isrflags & USART_ISR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
    {
      __HAL_UART_CLEAR_IT(huart, UART_CLEAR_PEF);

      huart->ErrorCode |= HAL_UART_ERROR_PE;
    }

    /* UART frame error interrupt occurred --------------------------------------*/
    if(((isrflags & USART_ISR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      __HAL_UART_CLEAR_IT(huart, UART_CLEAR_FEF);

      huart->ErrorCode |= HAL_UART_ERROR_FE;
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if(((isrflags & USART_ISR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      __HAL_UART_CLEAR_IT(huart, UART_CLEAR_NEF);

      huart->ErrorCode |= HAL_UART_ERROR_NE;
    }
    
    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if(((isrflags & USART_ISR_ORE) != RESET) &&
       (((cr1its & USART_CR1_RXNEIE) != RESET) || ((cr3its & USART_CR3_EIE) != RESET)))
    {
      __HAL_UART_CLEAR_IT(huart, UART_CLEAR_OREF);

      huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }

    /* Call UART Error Call back function if need be --------------------------*/
    if(huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
      /* UART in mode Receiver ---------------------------------------------------*/
      if(((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
      {
        //UART_Receive_IT(huart);
      }

      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
         consider error as blocking */
      if (((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) ||
          (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)))
      {  
        /* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
        //UART_EndRxTransfer(huart);

        /* Disable the UART DMA Rx request if enabled */
        if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
        {
          CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

          /* Abort the UART DMA Rx channel */
          if(huart->hdmarx != NULL)
          {
            /* Set the UART DMA Abort callback : 
               will lead to call HAL_UART_ErrorCallback() at end of DMA abort procedure */
          //  huart->hdmarx->XferAbortCallback = UART_DMAAbortOnError;

            /* Abort DMA RX */
            if(HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
            {
              /* Call Directly huart->hdmarx->XferAbortCallback function in case of error */
              huart->hdmarx->XferAbortCallback(huart->hdmarx);
            }
          }
          else
          {
            /* Call user error callback */
            HAL_UART_ErrorCallback(huart);
          }
        }
        else
        {
          /* Call user error callback */
          HAL_UART_ErrorCallback(huart);
        }
      }
      else
      {
        /* Non Blocking error : transfer could go on. 
           Error is notified to user through user error callback */
        HAL_UART_ErrorCallback(huart);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;

  } /* End if some error occurs */

  /* UART wakeup from Stop mode interrupt occurred ---------------------------*/
  if(((isrflags & USART_ISR_WUF) != RESET) && ((cr3its & USART_CR3_WUFIE) != RESET))
  {
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_WUF);
    /* Set the UART state ready to be able to start again the process */
    huart->gState  = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
    HAL_UARTEx_WakeupCallback(huart);
    return;
  }

  /* UART in mode Transmitter ------------------------------------------------*/
  if(((isrflags & USART_ISR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    //UART_Transmit_IT(huart);
    return;
  }

  /* UART in mode Transmitter (transmission end) -----------------------------*/
  if(((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
   // UART_EndTransmit_IT(huart);
    return;
  }

}
#endif
void Base_Usart1_Init(void)
{
	memset(usart1_s.RxMes,0,sizeof(usart1_s.RecvBuffer));
	usart1_s.RecvCount = 0;
	usart1_s.RecvFalg = 0;
	usart1_s.RecvLen = 0;
	//__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	//__HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&usart1_s.RecvBuffer, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		UNUSED(huart);
		if(huart == &huart1)
		{
			if(usart1_s.RecvBuffer == '\r')
			{
				usart1_s.RecvLen = usart1_s.RecvCount;
				USER_UART_Receive_Timeout();		
			}
			else
			{
				usart1_s.RxMes[usart1_s.RecvCount] = usart1_s.RecvBuffer;
				++usart1_s.RecvCount;
			}
			HAL_UART_Receive_IT(&huart1, (uint8_t *)&usart1_s.RecvBuffer, 1);
		}	
		
}
