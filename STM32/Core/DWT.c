#include "main.h"
#include "DWT.h"
void DWT_Init()
{
    DEM_CR  |=  DEM_CR_TRCENA; /*��DEMCR�Ĵ�����λ24���ƣ�д1ʹ��DWT���衣*/
    DWT_CYCCNT = 0;/*����DWT��CYCCNT�����Ĵ�����0��*/
    DWT_CR  |=  DWT_CR_CYCCNTENA;/*��DWT���ƼĴ�����λ0���ƣ�д1ʹ��CYCCNT�Ĵ�����*/
}

void DWT_DelayUS(uint32_t _ulDelayTime)
{
    uint32_t tCnt, tDelayCnt;
    uint32_t tStart;
           
    tStart = DWT_CYCCNT; /* �ս���ʱ�ļ�����ֵ */
    tCnt = 0;
    tDelayCnt = _ulDelayTime * (SystemCoreClock / 1000000);
    /* ��Ҫ�Ľ����� */    /*SystemCoreClock :ϵͳʱ��Ƶ��*/                 

    while(tCnt < tDelayCnt)
      {
        tCnt = DWT_CYCCNT - tStart; 
        /* ��������У����������һ��32λ���������¼�������Ȼ������ȷ���� */       
      }
}


