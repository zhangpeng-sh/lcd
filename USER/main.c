#include "led.h"
#include "beep.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "ILI93xx.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "sram.h"
#include "timer.h"
#include "sdio_sdcard.h"
#include "malloc.h"
#include "GUI.h"
#include "ff.h"
#include "exfuns.h"
#include "w25qxx.h"
#include "includes.h"
#include "FramewinDLG.h"
#include "WM.h"
#include "DIALOG.h"

extern WM_HWIN* hWin0;

//STemWin GUIBuliderÊ¹ÓÃ

//ÈÎÎñÓÅÏÈ¼¶
#define START_TASK_PRIO				3
//ÈÎÎñ¶ÑÕ»´óÐ¡	
#define START_STK_SIZE 				1024
//ÈÎÎñ¿ØÖÆ¿é
OS_TCB StartTaskTCB;
//ÈÎÎñ¶ÑÕ»	
CPU_STK START_TASK_STK[START_STK_SIZE];
//ÈÎÎñº¯Êý
void start_task(void *p_arg);

//TOUCHÈÎÎñ
//ÉèÖÃÈÎÎñÓÅÏÈ¼¶
#define TOUCH_TASK_PRIO				4
//ÈÎÎñ¶ÑÕ»´óÐ¡
#define TOUCH_STK_SIZE				128
//ÈÎÎñ¿ØÖÆ¿é
OS_TCB TouchTaskTCB;
//ÈÎÎñ¶ÑÕ»
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touchÈÎÎñ
void touch_task(void *p_arg);

//LED0ÈÎÎñ
//ÉèÖÃÈÎÎñÓÅÏÈ¼¶
#define LED0_TASK_PRIO 				5
//ÈÎÎñ¶ÑÕ»´óÐ¡
#define LED0_STK_SIZE				128
//ÈÎÎñ¿ØÖÆ¿é
OS_TCB Led0TaskTCB;
//ÈÎÎñ¶ÑÕ»
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0ÈÎÎñ
void led0_task(void *p_arg);

//EMWINDEMOÈÎÎñ
//ÉèÖÃÈÎÎñÓÅÏÈ¼¶
#define EMWINDEMO_TASK_PRIO			6
//ÈÎÎñ¶ÑÕ»´óÐ¡
#define EMWINDEMO_STK_SIZE			2048
//ÈÎÎñ¿ØÖÆ¿é
OS_TCB EmwindemoTaskTCB;
//ÈÎÎñ¶ÑÕ»
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_taskÈÎÎñ
void emwindemo_task(void *p_arg);

int main(void)
 {	
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();	    	//ÑÓÊ±º¯Êý³õÊ¼»¯	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//ÉèÖÃNVICÖÐ¶Ï·Ö×é2:2Î»ÇÀÕ¼ÓÅÏÈ¼¶£¬2Î»ÏìÓ¦ÓÅÏÈ¼¶
	uart_init(115200);	 	//´®¿Ú³õÊ¼»¯Îª115200
 	LED_Init();			    //LED¶Ë¿Ú³õÊ¼»¯
	TFTLCD_Init();			//LCD³õÊ¼»¯	
	KEY_Init();	 			//°´¼ü³õÊ¼»¯
	BEEP_Init();			//³õÊ¼»¯·äÃùÆ÷
	FSMC_SRAM_Init();		//³õÊ¼»¯SRAM
	my_mem_init(SRAMIN); 	//³õÊ¼»¯ÄÚ²¿ÄÚ´æ³Ø
	my_mem_init(SRAMEX);  	//³õÊ¼»¯Íâ²¿ÄÚ´æ³Ø
	
	exfuns_init();			//ÎªfatfsÎÄ¼þÏµÍ³·ÖÅäÄÚ´æ
	f_mount(fs[0],"0:",1);	//¹ÒÔØSD¿¨
	f_mount(fs[1],"1:",1);	//¹ÒÔØFLASH
	TP_Init();				//´¥ÃþÆÁ³õÊ¼»¯
	
	OSInit(&err);			//³õÊ¼»¯UCOSIII
	OS_CRITICAL_ENTER();	//½øÈëÁÙ½çÇø
	//´´½¨¿ªÊ¼È ÎÎñ
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//ÈÎÎñ¿ØÖÆ¿é
				 (CPU_CHAR	* )"start task", 		//ÈÎÎñÃû×Ö
                 (OS_TASK_PTR )start_task, 			//ÈÎÎñº¯Êý
                 (void		* )0,					//´«µÝ¸øÈÎÎñº¯ÊýµÄ²ÎÊý
                 (OS_PRIO	  )START_TASK_PRIO,     //ÈÎÎñÓÅÏÈ¼¶
                 (CPU_STK   * )&START_TASK_STK[0],	//ÈÎÎñ¶ÑÕ»»ùµØÖ·
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//ÈÎÎñ¶ÑÕ»Éî¶ÈÏÞÎ»
                 (CPU_STK_SIZE)START_STK_SIZE,		//ÈÎÎñ¶ÑÕ»´óÐ¡
                 (OS_MSG_QTY  )0,					//ÈÎÎñÄÚ²¿ÏûÏ¢¶ÓÁÐÄÜ¹»½ÓÊÕµÄ×î´óÏûÏ¢ÊýÄ¿,Îª0Ê±½ûÖ¹½ÓÊÕÏûÏ¢
                 (OS_TICK	  )0,					//µ±Ê¹ÄÜÊ±¼äÆ¬ÂÖ×ªÊ±µÄÊ±¼äÆ¬³¤¶È£¬Îª0Ê±ÎªÄ¬ÈÏ³¤¶È£¬
                 (void   	* )0,					//ÓÃ»§²¹³äµÄ´æ´¢Çø
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //ÈÎÎñÑ¡Ïî
                 (OS_ERR 	* )&err);				//´æ·Å¸Ãº¯Êý´íÎóÊ±µÄ·µ»ØÖµ
	OS_CRITICAL_EXIT();	//ÍË³öÁÙ½çÇø	 
	OSStart(&err);  //¿ªÆôUCOSIII
	while(1);
}

//¿ªÊ¼ÈÎÎñº¯Êý
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//Í³¼ÆÈÎÎñ                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//Èç¹ûÊ¹ÄÜÁË²âÁ¿ÖÐ¶Ï¹Ø±ÕÊ±¼ä
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //µ±Ê¹ÓÃÊ±¼äÆ¬ÂÖ×ªµÄÊ±ºò
	 //Ê¹ÄÜÊ±¼äÆ¬ÂÖ×ªµ÷¶È¹¦ÄÜ,Ê±¼äÆ¬³¤¶ÈÎª1¸öÏµÍ³Ê±ÖÓ½ÚÅÄ£¬¼È1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//¿ªÆôCRCÊ±ÖÓ
	WM_SetCreateFlags(WM_CF_MEMDEV); 	//Æô¶¯ËùÓÐ´°¿ÚµÄ´æ´¢Éè±¸
	GUI_Init();  			//STemWin³õÊ¼»¯

	OS_CRITICAL_ENTER();	//½øÈëÁÙ½çÇø
	//STemWin DemoÈÎÎñ	
	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,		
				 (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//´¥ÃþÆÁÈÎÎñ
	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 
	//LED0ÈÎÎñ
	OSTaskCreate((OS_TCB*     )&Led0TaskTCB,		
				 (CPU_CHAR*   )"Led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK*    )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//¹ÒÆð¿ªÊ¼ÈÎÎñ			 
	OS_CRITICAL_EXIT();	//ÍË³öÁÙ½çÇø
}

//EMWINDEMOÈÎÎñ
void emwindemo_task(void *p_arg)
{
	//¸ü»»Æ¤·ô
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX); 
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	MENU_SetDefaultSkin(MENU_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);
	CreateFramewin();
	while(1)
	{
		GUI_Delay(100); 
	}
}

//TOUCHÈÎÎñ
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//ÑÓÊ±5ms
	}
}

//LED0ÈÎÎñ
void led0_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//ÑÓÊ±500ms
	}
}
