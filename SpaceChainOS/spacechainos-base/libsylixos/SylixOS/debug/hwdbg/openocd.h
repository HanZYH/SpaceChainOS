/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: openocd.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2015 �� 11 �� 19 ��
**
** ��        ��: SylixOS OpenOCD ������֧��.
*********************************************************************************************************/

#ifndef __OPENOCD_H
#define __OPENOCD_H

/*********************************************************************************************************
  ˵��:
  API_OpenOCDStep1(), API_OpenOCDStep2(), API_OpenOCDStep3() Ϊ���� OpenOCD �ӿڳ�ʼ������, ���밴˳�����
  ����������������Ҫ�ڲ���ϵͳ��ʼ�� CACHE ֮ǰ, �Ƽ����� bspInit ���λ��.
  ��ϵͳִ��֮ǰ, ��Ҫ�� API_OpenOCDStep2() �����ϵ�, Ȼ�󵥲�ִ��, ֱ�� API_OpenOCDStep3() ִ�����.
*********************************************************************************************************/

LW_API VOID     API_OpenOCDStep1(VOID);
LW_API VOID     API_OpenOCDStep2(VOID);
LW_API VOID     API_OpenOCDStep3(VOID);

#endif                                                                  /*  __OPENOCD_H                 */
/*********************************************************************************************************
  END
*********************************************************************************************************/