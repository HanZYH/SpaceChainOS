;/*********************************************************************************************************
;**
;**                                    �й�������Դ��֯
;**
;**                                   Ƕ��ʽʵʱ����ϵͳ
;**
;**                                       SylixOS(TM)
;**
;**                               Copyright  All Rights Reserved
;**
;**--------------�ļ���Ϣ--------------------------------------------------------------------------------
;**
;** ��   ��   ��: ppcCache83xxAsm.S
;**
;** ��   ��   ��: Jiao.JinXing (������)
;**
;** �ļ���������: 2016 �� 03 �� 30 ��
;**
;** ��        ��: PowerPC MPC83XX ��ϵ���� CACHE ����.
;*********************************************************************************************************/

#define __SYLIXOS_PPC603__                  0x603
#define __SYLIXOS_PPCEC603__                0xEC603
#define __SYLIXOS_PPC604__                  0x604

#define __SYLIXOS_PPC_CPU__                 __SYLIXOS_PPC603__           /*  MPC83XX �� 603 �ı���       */

#define __SYLIXOS_PPC83XX__

/*********************************************************************************************************
  ���¶���ӿ���
*********************************************************************************************************/

#define ppc60xICacheEnable                  ppc83xxICacheEnable
#define ppc60xDCacheEnable                  ppc83xxDCacheEnable
#define ppc60xICacheDisable                 ppc83xxICacheDisable
#define ppc60xDCacheDisable                 ppc83xxDCacheDisable
#define ppc60xICacheInvalidate              ppc83xxICacheInvalidate
#define ppc60xDCacheInvalidate              ppc83xxDCacheInvalidate
#define ppc60xDCacheClear                   ppc83xxDCacheClear
#define ppc60xDCacheFlush                   ppc83xxDCacheFlush
#define ppc60xICacheInvalidateAll           ppc83xxICacheInvalidateAll
#define ppc60xDCacheInvalidateAll           ppc83xxDCacheInvalidateAll
#define ppc60xDCacheFlushAll                ppc83xxDCacheFlushAll
#define ppc60xDCacheClearAll                ppc83xxDCacheClearAll
#define ppc60xBranchPredictionDisable       ppc83xxBranchPredictionDisable
#define ppc60xBranchPredictionEnable        ppc83xxBranchPredictionEnable
#define ppc60xBranchPredictorInvalidate     ppc83xxBranchPredictorInvalidate
#define ppc60xTextUpdate                    ppc83xxTextUpdate

/*********************************************************************************************************
  ���� 60X CACHE ����
*********************************************************************************************************/

#include "ppcCache60xAsm.S"

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/