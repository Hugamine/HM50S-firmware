//
// Created by x on 6/15/2025.
//

#include "xpt2046.h"
#include "main.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "tim.h"
#include "spi.h"
#include "stdlib.h"
#include "w25qxx.h"
#include <stdint.h>

#define CALIB_ADDR  0x000000  // pick a dedicated 4KB sector

SPI_HandleTypeDef* spiPort;
TouchScreen_CoordinatesRaw ts_CoordinatesRaw;
TouchScreen_Coordinates ts_Coordinates;
TouchScreen_OrientationTypeDef ts_Orientation;
TouchScreen_Size ts_Size;
TouchScreen_ControlByte ts_ControlByte;
GPIO_TypeDef* ts_Cs_Port;
GPIO_TypeDef* ts_Penirq_Port;
uint16_t ts_Cs_Pin;
uint16_t ts_Penirq_Pin;
uint8_t command;
TouchScreen_Calibration ts_Calibration;

/**
 * Init the moduel
 */
void xpt2046_init()
{
	ts_ControlByte.bitMode = XPT2046_12BIT_MODE;
	ts_ControlByte.powerMode = XPT2046_POWER_DOWN;
	ts_ControlByte.reference = XPT2046_DFR_MODE;
	ts_ControlByte.channel = XPT2046_DFR_X; //by default
	ts_ControlByte.startBit = XPT2046_START;
	ts_Orientation = XPT2046_ORIENTATION_PORTRAIT;
	ts_Size.width = XPT2046_WIDTH;
	ts_Size.height = XPT2046_HEIGHT;
	ts_Cs_Pin = TP_CS_Pin;
	ts_Cs_Port = TP_CS_GPIO_Port;
	ts_Penirq_Pin = TP_IRQ_Pin;
	ts_Penirq_Port = TP_IRQ_GPIO_Port;
	command = ts_ControlByte.bitMode |  ts_ControlByte.powerMode |ts_ControlByte.reference |ts_ControlByte.channel |ts_ControlByte.startBit;
	xpt2046_unselect();
}


void xpt2046_orientation(TouchScreen_OrientationTypeDef orientation_)
{
	ts_Orientation = orientation_;
}

void xpt2046_bit_mode(uint8_t b)
{
	ts_ControlByte.bitMode = b;
}
void xpt2046_set_size(uint16_t w, uint16_t h)
{
	ts_Size.width = w;
	ts_Size.height = h;
}

void xpt2046_cs(GPIO_TypeDef* cs_port, uint16_t cs_pin)
{
	ts_Cs_Port = cs_port;
	ts_Cs_Pin = cs_pin;
}

void xpt2046_penirq(GPIO_TypeDef* penirq_port, uint16_t penirq_pin)
{
	ts_Penirq_Port = penirq_port;
	ts_Penirq_Pin = penirq_pin;
}

uint16_t xpt2046_zthreshold()
{
	if(ts_ControlByte.bitMode == XPT2046_8BIT_MODE)
	{
		return Z_THRESHOLD_08BIT;
	}else
	{
		return Z_THRESHOLD_12BIT;
	}
}
void xpt2046_select()
{
	HAL_GPIO_WritePin(ts_Cs_Port,ts_Cs_Pin,GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(XPT2046_CS_Port,XPT2046_CS_Pin,GPIO_PIN_RESET);
}

void xpt2046_unselect()
{
	HAL_GPIO_WritePin(ts_Cs_Port,ts_Cs_Pin,GPIO_PIN_SET);
	//HAL_GPIO_WritePin(XPT2046_CS_Port,XPT2046_CS_Pin,GPIO_PIN_SET);
}

uint8_t xpt2046_interrupt()
{
	return HAL_GPIO_ReadPin(ts_Penirq_Port,ts_Penirq_Pin) == GPIO_PIN_RESET; //Means touchScreen was pressed
}

uint8_t xpt2046_pressed()
{
	if(xpt2046_interruptions_activated())
	{
		return xpt2046_interrupt();
	}else
	{
		uint16_t zt =  xpt2046_zthreshold();
		return (ts_CoordinatesRaw.z1 > zt );
	}
}

uint8_t xpt2046_interruptions_activated()
{
	return (ts_ControlByte.powerMode == XPT2046_POWER_DOWN || ts_ControlByte.powerMode == XPT2046_REFERENCE_ON_ADC_OFF);
}
/**
 * Generate control byte
 */
void xpt2046_control_byte_update()
{
	command =  ts_ControlByte.startBit |  ts_ControlByte.channel | ts_ControlByte.bitMode | ts_ControlByte.reference | ts_ControlByte.powerMode  ;
}


/**
 * Return max value depending on bit mode.
 */
uint16_t xpt2046_max_measurement(){
	if(ts_ControlByte.bitMode == XPT2046_8BIT_MODE)
	{
		return 120; //This must be calibrated
	}else
	{
		return 1900; // this value must be calibrated on your on touch screen
	}
}
/**
 * Comapre Coordinates using integers and TouchScreen_Coordinates
 */
uint8_t xpt2046_compare_cords(uint16_t x, uint16_t y , uint16_t z, TouchScreen_Coordinates tsc)
{
	return 0;
}

/**
 * Comapre two TouchScreen_Coordinates
 */
uint8_t xtp_compare_tsc(TouchScreen_Coordinates c, TouchScreen_Coordinates tcs)
{
	return 0;
}
/**
 *  Set internal SPI to user defined SPI port
 */
void xpt2046_spi(SPI_HandleTypeDef* spi)
{
	spiPort =  spi;
}


/**
 * Set Size of Touch screen
 */

void xtp2046_set_size(uint16_t w, uint16_t h)
{
	ts_Size.width = w;
	ts_Size.height = h;
}


/**
 * Update X,Y,Z values of touchScreen
 */
void xpt2046_update(){
	xpt2046_select();
	Delay_us(1);

	if(xpt2046_interruptions_activated())
	{
		/* this means Interrupt pin was enabled*/
		if(!xpt2046_interrupt())
		{
			return;
		}


	}

	uint8_t zeros[2] = { 0x00,0x00};
	uint32_t x_avg = 0, y_avg = 0,z1_avg=0,z2_avg=0;

	for(int i = 0; i < AVERAGE_POINTS; i++)
	{
		ts_ControlByte.channel = XPT2046_DFR_X;//I swapped here for calibration
		xpt2046_control_byte_update();

		uint8_t receiveByteX[2] = {0x00,0x00};

		HAL_SPI_Transmit(spiPort,&command,sizeof(command),HAL_MAX_DELAY);
		Delay_us(6);
		if (HAL_SPI_Receive(spiPort,receiveByteX,sizeof(receiveByteX),HAL_MAX_DELAY) !=HAL_OK)HAL_Delay(10000);


		ts_ControlByte.channel = XPT2046_DFR_Y;
		xpt2046_control_byte_update();

		uint8_t receiveByteY[2] = {0x00,0x00};
		HAL_SPI_Transmit(spiPort,&command,sizeof(command),HAL_MAX_DELAY);
		Delay_us(6);
		HAL_SPI_Receive(spiPort,receiveByteY,sizeof(receiveByteY),HAL_MAX_DELAY);


		ts_ControlByte.channel = XPT2046_DFR_Z1;
		xpt2046_control_byte_update();
		uint8_t receiveByteZ1[2] = {0x00,0x00};

		HAL_SPI_Transmit(spiPort,&command,sizeof(command),HAL_MAX_DELAY);
		Delay_us(6);
		HAL_SPI_Receive(spiPort,receiveByteZ1,sizeof(receiveByteZ1),HAL_MAX_DELAY);


		ts_ControlByte.channel = XPT2046_DFR_Z2;
		xpt2046_control_byte_update();
		uint8_t receiveByteZ2[2] = {0x00,0x00};

		HAL_SPI_Transmit(spiPort,&command,sizeof(command),HAL_MAX_DELAY);
		Delay_us(6);
		HAL_SPI_Receive(spiPort,receiveByteZ2,sizeof(receiveByteZ2),HAL_MAX_DELAY);
		if(ts_ControlByte.bitMode == XPT2046_12BIT_MODE)
		{
			x_avg += 	(((uint16_t)receiveByteX[0] 	<< 8) | receiveByteX[1])<<1>>4 ;
			y_avg+= 	(receiveByteY[0] 	<< 5) | (receiveByteY[1] 	>> 3);
			z1_avg+= 	(receiveByteZ1[0] 	<< 5) | (receiveByteZ1[1] 	>> 3);
			z2_avg+= 	(receiveByteZ2[0] 	<< 5) | (receiveByteZ2[1] 	>> 3);
		}else
		{
			x_avg+= 	(receiveByteX[1]);
			y_avg+= 	(receiveByteY[1]) ;
			z1_avg+= 	(receiveByteZ1[1]);
			z2_avg+= 	(receiveByteZ2[1]);
		}

	}

	ts_CoordinatesRaw.x = x_avg/AVERAGE_POINTS;
	ts_CoordinatesRaw.y = y_avg/AVERAGE_POINTS;
	ts_CoordinatesRaw.z1 = z1_avg/AVERAGE_POINTS;
	ts_CoordinatesRaw.z2 = z2_avg/AVERAGE_POINTS;
	uint16_t max = xpt2046_max_measurement();
	/* Consider (0,0) as the top left point of the screen */
	switch(ts_Orientation)
	{
		case XPT2046_ORIENTATION_PORTRAIT:
		{
			ts_Coordinates.x = ((ts_Calibration.yfactor * ts_CoordinatesRaw.y) + ts_Calibration.yoffset);
			ts_Coordinates.y = ((ts_Calibration.xfactor * ts_CoordinatesRaw.x) + ts_Calibration.xoffset);
			ts_Coordinates.z = ts_CoordinatesRaw.z1;
			break;
		}
		case XPT2046_ORIENTATION_LANDSCAPE:
		{
			ts_Coordinates.x = (uint16_t)(((float)ts_Size.width - ts_Calibration.xfactor * (float)ts_CoordinatesRaw.y) + (float)ts_Calibration.yoffset);
			ts_Coordinates.y = ts_Size.height - ((ts_Calibration.xfactor * ts_CoordinatesRaw.x) + ts_Calibration.xoffset);
			ts_Coordinates.z = ts_CoordinatesRaw.z1;
			break;
		}
		case XPT2046_ORIENTATION_PORTRAIT_MIRROR:
		{
			ts_Coordinates.x = (ts_Calibration.xfactor * (ts_CoordinatesRaw.x)) + ts_Calibration.xoffset;
			ts_Coordinates.y = (ts_Size.height - (ts_Calibration.yfactor * ts_CoordinatesRaw.y)) + ts_Calibration.yoffset;
			ts_Coordinates.z = ts_CoordinatesRaw.z1;
			break;
		}
		case XPT2046_ORIENTATION_LANDSCAPE_MIRROR:
		{
			ts_Coordinates.x = (ts_Calibration.xfactor *  ts_CoordinatesRaw.y) + ts_Calibration.yoffset;
			ts_Coordinates.y = (ts_Size.height - (ts_Calibration.yfactor * ts_CoordinatesRaw.x)) + ts_Calibration.xoffset;
			ts_Coordinates.z = ts_CoordinatesRaw.z1;
			break;
		}
		default:
		{
			break;
		}
	}


	xpt2046_unselect();
}

/* by default return 0,0 if the screen isn't pressed*/
void xpt2046_read_position(uint16_t* x, uint16_t* y)
{
	xpt2046_update();
	if(xpt2046_pressed())
	{
		*x = ts_Coordinates.x;
		*y = ts_Coordinates.y;
	}else
	{
		*x = 0;
		*y = 0;
	}
}

uint8_t TP_Get_Adjdata(void)
{
	int32_t tempfac;
	uint8_t tempbufa[14];
	uint8_t ID[2];
	W25Q_ReadFullID(ID);
	if((ID[0]<<8|ID[1])==W25Q256)
	{
		if(W25Q_ReadRaw(tempbufa,14,CALIB_ADDR) != W25Q_OK){
			return 1;
		}
		tempfac=tempbufa[13];
		if(tempfac==0X0B)
		{
			tempfac=0;
			tempfac|=tempbufa[3];
			tempfac<<=8;
			tempfac|=tempbufa[2];
			tempfac<<=8;
			tempfac|=tempbufa[1];
			tempfac<<=8;
			tempfac|=tempbufa[0];

			ts_Calibration.xfactor=(float)tempfac/100000000;//得到x校准参数

			tempfac=0;
			tempfac|=tempbufa[7];
			tempfac<<=8;
			tempfac|=tempbufa[6];
			tempfac<<=8;
			tempfac|=tempbufa[5];
			tempfac<<=8;
			tempfac|=tempbufa[4];
			ts_Calibration.yfactor=(float)tempfac/100000000;//得到y校准参数


			//得到x偏移量
			ts_Calibration.xoffset=0;
			ts_Calibration.xoffset|=tempbufa[9];
			ts_Calibration.xoffset<<=8;
			ts_Calibration.xoffset|=tempbufa[8];

			//得到y偏移量
			ts_Calibration.yoffset=0;
			ts_Calibration.yoffset|=tempbufa[11];
			ts_Calibration.yoffset<<=8;
			ts_Calibration.yoffset|=tempbufa[10];
		}
	}
	return 0;
}

void TP_Save_Adjdata(void)
{
	
	uint8_t tempbuf[14];
	long temp;			 
	//保存校正结果!		   							  
	temp=ts_Calibration.xfactor*100000000;//保存x校正因素      
			tempbuf[0]=temp;
			tempbuf[1]=temp>>8;
			tempbuf[2]=temp>>16;
			tempbuf[3]=temp>>24;
	temp=ts_Calibration.yfactor*100000000;//保存y校正因素    
			tempbuf[4]=temp;
			tempbuf[5]=temp>>8;
			tempbuf[6]=temp>>16;
			tempbuf[7]=temp>>24;
	//保存x偏移量
			tempbuf[8]=ts_Calibration.xoffset;
			tempbuf[9]=ts_Calibration.xoffset>>8;
	//保存y偏移量
			tempbuf[10]=ts_Calibration.yoffset;
			tempbuf[11]=ts_Calibration.yoffset>>8;
	//保存触屏类型
			tempbuf[12]=0x00;
	temp=0X0B;//标记校准过了
			tempbuf[13]=temp;
			if(W25Q_EraseSector(CALIB_ADDR) == W25Q_OK){
			    if(W25Q_ProgramRaw(tempbuf,14,CALIB_ADDR) != W25Q_OK){
				    HAL_Delay(1000);
			    }
			}

}
