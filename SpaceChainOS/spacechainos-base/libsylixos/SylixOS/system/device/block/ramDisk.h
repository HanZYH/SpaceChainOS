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
** ��   ��   ��: ramDisk.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2008 �� 09 �� 26 ��
**
** ��        ��: RAM DISK ��������
*********************************************************************************************************/

#ifndef __RAMDISK_H
#define __RAMDISK_H

/*********************************************************************************************************
  �ü���
*********************************************************************************************************/
#if LW_CFG_MAX_VOLUMES > 0

LW_API ULONG    API_RamDiskCreate(PVOID  pvDiskAddr, UINT64  ullDiskSize, PLW_BLK_DEV  *ppblkdRam);
LW_API INT      API_RamDiskDelete(PLW_BLK_DEV  pblkdRam);

#define ramDiskCreate       API_RamDiskCreate
#define ramDiskDelete       API_RamDiskDelete

#endif                                                                  /*  LW_CFG_MAX_VOLUMES          */
#endif                                                                  /*  __RAMDISK_H                 */
/*********************************************************************************************************
  END
*********************************************************************************************************/