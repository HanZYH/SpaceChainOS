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
** ��   ��   ��: lwip_ifctl.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2017 �� 12 �� 08 ��
**
** ��        ��: ioctl ����ӿ�֧��.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  �ü�����
*********************************************************************************************************/
#if LW_CFG_NET_EN > 0
#include "net/if.h"
#include "net/if_arp.h"
#include "net/if_type.h"
#include "net/if_lock.h"
#include "net/if_flags.h"
#include "sys/socket.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#if LW_CFG_NET_WIRELESS_EN > 0
#include "net/if_wireless.h"
#include "./wireless/lwip_wl.h"
#endif                                                                  /*  LW_CFG_NET_WIRELESS_EN > 0  */
/*********************************************************************************************************
** ��������: __ifConfSize
** ��������: �������ӿ��б���������Ҫ���ڴ��С
** �䡡��  : piSize    �����ڴ��С
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID __ifConfSize (INT  *piSize)
{
           INT       iNum = 0;
    struct netif    *pnetif;
    
    NETIF_FOREACH(pnetif) {
        iNum += sizeof(struct ifreq);
    }
    
    *piSize = iNum;
}
/*********************************************************************************************************
** ��������: __ifConf
** ��������: �������ӿ��б�����
** �䡡��  : pifconf   �б����滺��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID __ifConf (struct ifconf  *pifconf)
{
           INT           iSize;
           INT           iNum = 0;
    struct netif        *pnetif;
    struct ifreq        *pifreq;
    struct sockaddr_in  *psockaddrin;
    
    iSize = pifconf->ifc_len / sizeof(struct ifreq);                    /*  ����������                  */
    
    pifreq = pifconf->ifc_req;
    NETIF_FOREACH(pnetif) {
        if (iNum >= iSize) {
            break;
        }
        netif_get_name(pnetif, pifreq->ifr_name);
        psockaddrin = (struct sockaddr_in *)&(pifreq->ifr_addr);
        psockaddrin->sin_len    = sizeof(struct sockaddr_in);
        psockaddrin->sin_family = AF_INET;
        psockaddrin->sin_port   = 0;
        psockaddrin->sin_addr.s_addr = netif_ip4_addr(pnetif)->addr;
        
        iNum++;
        pifreq++;
    }
    
    pifconf->ifc_len = iNum * sizeof(struct ifreq);                     /*  ��ȡ����                    */
}
/*********************************************************************************************************
** ��������: __ifReq6Size
** ��������: �������ӿ� IPv6 ��ַ��Ŀ
** �䡡��  : ifreq6    ifreq6 ������ƿ�
** �䡡��  : �������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT __ifReq6Size (struct in6_ifreq  *pifreq6)
{
    INT              i;
    INT              iNum   = 0;
    struct netif    *pnetif = netif_get_by_index(pifreq6->ifr6_ifindex);
    
    if (pnetif == LW_NULL) {
        _ErrorHandle(EADDRNOTAVAIL);                                    /*  δ�ҵ�ָ��������ӿ�        */
        return  (PX_ERROR);
    }
    
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(pnetif->ip6_addr_state[i])) {
            iNum++;
        }
    }
    
    pifreq6->ifr6_len = iNum * sizeof(struct in6_ifr_addr);             /*  ��дʵ�ʴ�С                */
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __ifSubIoctlIf
** ��������: ����ӿ� ioctl ���� (��������ӿ�)
** �䡡��  : iCmd      ����
**           pvArg     ����
** �䡡��  : �������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  __ifSubIoctlIf (INT  iCmd, PVOID  pvArg)
{
    INT              iRet   = PX_ERROR;
    INT              iFlags;
    struct ifreq    *pifreq = LW_NULL;
    struct netif    *pnetif;
    
    pifreq = (struct ifreq *)pvArg;
    
    pnetif = netif_find(pifreq->ifr_name);
    if (pnetif == LW_NULL) {
        _ErrorHandle(EADDRNOTAVAIL);                                    /*  δ�ҵ�ָ��������ӿ�        */
        return  (iRet);
    }

    switch (iCmd) {
    
    case SIOCGIFFLAGS:                                                  /*  ��ȡ���� flag               */
        pifreq->ifr_flags = netif_get_flags(pnetif);
        iRet = ERROR_NONE;
        break;
        
    case SIOCSIFFLAGS:                                                  /*  �������� flag               */
        iFlags = netif_get_flags(pnetif);
        if (iFlags != pifreq->ifr_flags) {
            if (!pnetif->ioctl) {
                break;
            }
            iRet = pnetif->ioctl(pnetif, SIOCSIFFLAGS, pvArg);
            if (iRet < ERROR_NONE) {
                break;
            }
            if (pifreq->ifr_flags & IFF_PROMISC) {
                pnetif->flags2 |= NETIF_FLAG2_PROMISC;
            } else {
                pnetif->flags2 |= ~NETIF_FLAG2_PROMISC;
            }
            if (pifreq->ifr_flags & IFF_ALLMULTI) {
                pnetif->flags2 |= NETIF_FLAG2_ALLMULTI;
            } else {
                pnetif->flags2 |= ~NETIF_FLAG2_ALLMULTI;
            }
            if (pifreq->ifr_flags & IFF_UP) {
                netif_set_up(pnetif);
            } else {
                netif_set_down(pnetif);
            }
        }
        iRet = ERROR_NONE;
        break;
        
    case SIOCGIFTYPE:                                                   /*  �����������                */
        if ((pnetif->flags & NETIF_FLAG_BROADCAST) == 0) {
            pifreq->ifr_type = IFT_PPP;
        } else if (pnetif->flags & (NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP)) {
            pifreq->ifr_type = IFT_ETHER;
        } else if (ip4_addr_isloopback(netif_ip4_addr(pnetif))) {
            pifreq->ifr_type = IFT_LOOP;
        } else {
            pifreq->ifr_type = IFT_OTHER;
        }
        iRet = ERROR_NONE;
        break;
        
    case SIOCGIFINDEX:                                                  /*  ������� index              */
        pifreq->ifr_ifindex = netif_get_index(pnetif);
        iRet = ERROR_NONE;
        break;
        
    case SIOCGIFMTU:                                                    /*  ������� mtu                */
        pifreq->ifr_mtu = pnetif->mtu;
        iRet = ERROR_NONE;
        break;
        
    case SIOCSIFMTU:                                                    /*  �������� mtu                */
        if (pifreq->ifr_mtu != pnetif->mtu) {
            if (pnetif->ioctl) {
                if (pnetif->ioctl(pnetif, SIOCSIFMTU, pvArg) == 0) {
                    pnetif->mtu = pifreq->ifr_mtu;
                    iRet = ERROR_NONE;
                }
            }
        }
        _ErrorHandle(ENOSYS);
        break;
        
    case SIOCGIFHWADDR:                                                 /*  ���������ַ                */
        if (pnetif->flags & (NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP)) {
            INT i;
            for (i = 0; i < IFHWADDRLEN; i++) {
                pifreq->ifr_hwaddr.sa_data[i] = pnetif->hwaddr[i];
            }
            pifreq->ifr_hwaddr.sa_family = ARPHRD_ETHER;
            iRet = ERROR_NONE;
        } else {
            _ErrorHandle(EINVAL);
        }
        break;
        
    case SIOCSIFHWADDR:                                                 /*  ���� mac ��ַ               */
        if (pnetif->flags & (NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP)) {
            INT i;
            INT iIsUp = netif_is_up(pnetif);
            if (pnetif->ioctl) {
                netif_set_down(pnetif);                                 /*  �ر�����                    */
                iRet = pnetif->ioctl(pnetif, SIOCSIFHWADDR, pvArg);
                if (iRet == ERROR_NONE) {
                    for (i = 0; i < IFHWADDRLEN; i++) {
                        pnetif->hwaddr[i] = pifreq->ifr_hwaddr.sa_data[i];
                    }
                }
                if (iIsUp) {
                    netif_set_up(pnetif);                               /*  ��������                    */
                }
            } else {
                _ErrorHandle(ENOTSUP);
            }
        } else {
            _ErrorHandle(EINVAL);
        }
        break;
        
    case SIOCGIFMETRIC:                                                 /*  ����������                */
        pifreq->ifr_metric = pnetif->metric;
        break;
        
    case SIOCSIFMETRIC:                                                 /*  �����������                */
        pnetif->metric = pifreq->ifr_metric;                            /*  Ŀǰ��������                */
        break;
        
    case SIOCADDMULTI:                                                  /*  �����鲥�˲���              */
    case SIOCDELMULTI:
        if (pnetif->ioctl) {
            iRet = pnetif->ioctl(pnetif, iCmd, pvArg);
        }
        break;
    }
    
    return  (iRet);
}
/*********************************************************************************************************
** ��������: __ifSubIoctl4
** ��������: ����ӿ� ioctl ���� (��� ipv4)
** �䡡��  : iCmd      ����
**           pvArg     ����
** �䡡��  : �������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  __ifSubIoctl4 (INT  iCmd, PVOID  pvArg)
{
           INT           iRet   = PX_ERROR;
    struct ifreq        *pifreq = LW_NULL;
    struct netif        *pnetif;
    struct sockaddr_in  *psockaddrin;
    
    pifreq = (struct ifreq *)pvArg;
    
    pnetif = netif_find(pifreq->ifr_name);
    if (pnetif == LW_NULL) {
        _ErrorHandle(EADDRNOTAVAIL);                                    /*  δ�ҵ�ָ��������ӿ�        */
        return  (iRet);
    }
    
    switch (iCmd) {                                                     /*  ����Ԥ����                  */
    
    case SIOCGIFADDR:                                                   /*  ��ȡ��ַ����                */
    case SIOCGIFNETMASK:
    case SIOCGIFDSTADDR:
    case SIOCGIFBRDADDR:
        psockaddrin = (struct sockaddr_in *)&(pifreq->ifr_addr);
        psockaddrin->sin_len    = sizeof(struct sockaddr_in);
        psockaddrin->sin_family = AF_INET;
        psockaddrin->sin_port   = 0;
        break;
        
    case SIOCSIFADDR:                                                   /*  ���õ�ַ����                */
    case SIOCSIFNETMASK:
    case SIOCSIFDSTADDR:
    case SIOCSIFBRDADDR:
        psockaddrin = (struct sockaddr_in *)&(pifreq->ifr_addr);
        break;
    }
    
    switch (iCmd) {                                                     /*  �������                  */
        
    case SIOCGIFADDR:                                                   /*  ��ȡ���� IP                 */
        psockaddrin->sin_addr.s_addr = netif_ip4_addr(pnetif)->addr;
        iRet = ERROR_NONE;
        break;
    
    case SIOCGIFNETMASK:                                                /*  ��ȡ���� mask               */
        psockaddrin->sin_addr.s_addr = netif_ip4_netmask(pnetif)->addr;
        iRet = ERROR_NONE;
        break;
        
    case SIOCGIFDSTADDR:                                                /*  ��ȡ����Ŀ���ַ            */
        if ((pnetif->flags & NETIF_FLAG_BROADCAST) == 0) {
            psockaddrin->sin_addr.s_addr = INADDR_ANY;
            iRet = ERROR_NONE;
        } else {
            _ErrorHandle(EINVAL);
        }
        break;
        
    case SIOCGIFBRDADDR:                                                /*  ��ȡ�����㲥��ַ            */
        if (pnetif->flags & NETIF_FLAG_BROADCAST) {
            psockaddrin->sin_addr.s_addr = (netif_ip4_addr(pnetif)->addr 
                                         | (~(netif_ip4_netmask(pnetif)->addr)));
            iRet = ERROR_NONE;
        } else {
            _ErrorHandle(EINVAL);
        }
        break;
        
    case SIOCSIFADDR:                                                   /*  ����������ַ                */
        if (psockaddrin->sin_family == AF_INET) {
            ip4_addr_t ipaddr;
            ipaddr.addr = psockaddrin->sin_addr.s_addr;
            netif_set_ipaddr(pnetif, &ipaddr);
            iRet = ERROR_NONE;
        } else {
            _ErrorHandle(EAFNOSUPPORT);
        }
        break;
        
    case SIOCSIFNETMASK:                                                /*  ������������                */
        if (psockaddrin->sin_family == AF_INET) {
            ip4_addr_t ipaddr;
            ipaddr.addr = psockaddrin->sin_addr.s_addr;
            netif_set_netmask(pnetif, &ipaddr);
            iRet = ERROR_NONE;
        } else {
            _ErrorHandle(EAFNOSUPPORT);
        }
        break;
        
    case SIOCSIFDSTADDR:                                                /*  ��������Ŀ���ַ            */
        if ((pnetif->flags & NETIF_FLAG_BROADCAST) == 0) {
            iRet = ERROR_NONE;
        } else {
            _ErrorHandle(EINVAL);
        }
        break;
        
    case SIOCSIFBRDADDR:                                                /*  ���������㲥��ַ            */
        if (pnetif->flags & NETIF_FLAG_BROADCAST) {
            iRet = ERROR_NONE;
        } else {
            _ErrorHandle(EINVAL);
        }
        break;
    }
    
    return  (iRet);
}
/*********************************************************************************************************
** ��������: __ifSubIoctl6
** ��������: ����ӿ� ioctl ���� (��� ipv6)
** �䡡��  : iCmd      ����
**           pvArg     ����
** �䡡��  : �������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  __ifSubIoctl6 (INT  iCmd, PVOID  pvArg)
{
#define __LWIP_GET_IPV6_FROM_NETIF() \
        pifr6addr->ifr6a_addr.un.u32_addr[0] = ip_2_ip6(&pnetif->ip6_addr[i])->addr[0]; \
        pifr6addr->ifr6a_addr.un.u32_addr[1] = ip_2_ip6(&pnetif->ip6_addr[i])->addr[1]; \
        pifr6addr->ifr6a_addr.un.u32_addr[2] = ip_2_ip6(&pnetif->ip6_addr[i])->addr[2]; \
        pifr6addr->ifr6a_addr.un.u32_addr[3] = ip_2_ip6(&pnetif->ip6_addr[i])->addr[3];
        
#define __LWIP_SET_IPV6_TO_NETIF() \
        ip_2_ip6(&pnetif->ip6_addr[i])->addr[0] = pifr6addr->ifr6a_addr.un.u32_addr[0]; \
        ip_2_ip6(&pnetif->ip6_addr[i])->addr[1] = pifr6addr->ifr6a_addr.un.u32_addr[1]; \
        ip_2_ip6(&pnetif->ip6_addr[i])->addr[2] = pifr6addr->ifr6a_addr.un.u32_addr[2]; \
        ip_2_ip6(&pnetif->ip6_addr[i])->addr[3] = pifr6addr->ifr6a_addr.un.u32_addr[3]; 
        
#define __LWIP_CMP_IPV6_WITH_NETIF() \
        (ip_2_ip6(&pnetif->ip6_addr[i])->addr[0] == pifr6addr->ifr6a_addr.un.u32_addr[0]) && \
        (ip_2_ip6(&pnetif->ip6_addr[i])->addr[1] == pifr6addr->ifr6a_addr.un.u32_addr[1]) && \
        (ip_2_ip6(&pnetif->ip6_addr[i])->addr[2] == pifr6addr->ifr6a_addr.un.u32_addr[2]) && \
        (ip_2_ip6(&pnetif->ip6_addr[i])->addr[3] == pifr6addr->ifr6a_addr.un.u32_addr[3]) 

           INT           i;
           INT           iSize;
           INT           iNum = 0;
           INT           iRet    = PX_ERROR;
    struct in6_ifreq    *pifreq6 = LW_NULL;
    struct in6_ifr_addr *pifr6addr;
    struct netif        *pnetif;
    
    pifreq6 = (struct in6_ifreq *)pvArg;
    
    pnetif = netif_get_by_index(pifreq6->ifr6_ifindex);
    if (pnetif == LW_NULL) {
        _ErrorHandle(EADDRNOTAVAIL);                                    /*  δ�ҵ�ָ��������ӿ�        */
        return  (iRet);
    }
    
    iSize = pifreq6->ifr6_len / sizeof(struct in6_ifr_addr);            /*  ����������                  */
    pifr6addr = pifreq6->ifr6_addr_array;
    
    switch (iCmd) {                                                     /*  �������                  */
    
    case SIOCGIFADDR6:                                                  /*  ��ȡ���� IP                 */
        for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (iNum >= iSize) {
                break;
            }
            if (ip6_addr_isvalid(pnetif->ip6_addr_state[i])) {
                __LWIP_GET_IPV6_FROM_NETIF();
                if (ip6_addr_isloopback(ip_2_ip6(&pnetif->ip6_addr[i]))) {
                    pifr6addr->ifr6a_prefixlen = 128;
                } else if (ip6_addr_islinklocal(ip_2_ip6(&pnetif->ip6_addr[i]))) {
                    pifr6addr->ifr6a_prefixlen = 6;
                } else {
                    pifr6addr->ifr6a_prefixlen = 64;                    /*  TODO: Ŀǰ�޷���ȡ          */
                }
                iNum++;
                pifr6addr++;
            }
        }
        pifreq6->ifr6_len = iNum * sizeof(struct in6_ifr_addr);         /*  ��дʵ�ʴ�С                */
        iRet = ERROR_NONE;
        break;
        
    case SIOCSIFADDR6:                                                  /*  �������� IP                 */
        if (iSize != 1) {                                               /*  ÿ��ֻ������һ�� IP ��ַ    */
            _ErrorHandle(EOPNOTSUPP);
            break;
        }
        if (IN6_IS_ADDR_LOOPBACK(&pifr6addr->ifr6a_addr) ||
            IN6_IS_ADDR_LINKLOCAL(&pifr6addr->ifr6a_addr)) {            /*  �����ֶ��������������͵ĵ�ַ*/
            _ErrorHandle(EADDRNOTAVAIL);
            break;
        }
        for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {                 /*  ������ͼ����                */
            if (ip6_addr_isinvalid(pnetif->ip6_addr_state[i])) {
                __LWIP_SET_IPV6_TO_NETIF();
                pnetif->ip6_addr_state[i] = IP6_ADDR_VALID | IP6_ADDR_TENTATIVE;
                netif_ip6_addr_set_valid_life(pnetif, i, IP6_ADDR_LIFE_STATIC);
                netif_ip6_addr_set_pref_life(pnetif, i, IP6_ADDR_LIFE_STATIC);
                break;
            }
        }
        if (i >= LWIP_IPV6_NUM_ADDRESSES) {                             /*  �޷�����                    */
            for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {             /*  ���ȸ����ݶ���ַ            */
                if (!ip6_addr_islinklocal(ip_2_ip6(&pnetif->ip6_addr[i])) &&
                    ip6_addr_istentative(pnetif->ip6_addr_state[i])) {
                    __LWIP_SET_IPV6_TO_NETIF();
                    pnetif->ip6_addr_state[i] = IP6_ADDR_VALID | IP6_ADDR_TENTATIVE;
                    netif_ip6_addr_set_valid_life(pnetif, i, IP6_ADDR_LIFE_STATIC);
                    netif_ip6_addr_set_pref_life(pnetif, i, IP6_ADDR_LIFE_STATIC);
                    break;
                }
            }
        }
        if (i >= LWIP_IPV6_NUM_ADDRESSES) {                             /*  �޷�����                    */
            for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {             /*  �滻�� linklocal ��ַ       */
                if (!ip6_addr_islinklocal(ip_2_ip6(&pnetif->ip6_addr[i]))) {
                    __LWIP_SET_IPV6_TO_NETIF();
                    pnetif->ip6_addr_state[i] = IP6_ADDR_VALID | IP6_ADDR_TENTATIVE;
                    netif_ip6_addr_set_valid_life(pnetif, i, IP6_ADDR_LIFE_STATIC);
                    netif_ip6_addr_set_pref_life(pnetif, i, IP6_ADDR_LIFE_STATIC);
                    break;
                }
            }
        }
        iRet = ERROR_NONE;
        break;
        
    case SIOCDIFADDR6:                                                  /*  ɾ��һ�� IPv6 ��ַ          */
        if (iSize != 1) {                                               /*  ÿ��ֻ������һ�� IP ��ַ    */
            _ErrorHandle(EOPNOTSUPP);
            break;
        }
        for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (ip6_addr_isvalid(pnetif->ip6_addr_state[i])) {
                if (__LWIP_CMP_IPV6_WITH_NETIF()) {                     /*  TODO û���ж�ǰ׺����       */
                    pnetif->ip6_addr_state[i] = IP6_ADDR_INVALID;
                    break;
                }
            }
        }
        iRet = ERROR_NONE;
        break;
        
    default:
        _ErrorHandle(ENOSYS);                                           /*  TODO: ����������δʵ��      */
        break;
    }
    
    return  (iRet);
}
/*********************************************************************************************************
** ��������: __ifSubIoctlCommon
** ��������: ͨ������ӿ� ioctl ����
** �䡡��  : iCmd      ����
**           pvArg     ����
** �䡡��  : �������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  __ifSubIoctlCommon (INT  iCmd, PVOID  pvArg)
{
    INT     iRet = PX_ERROR;
    
    switch (iCmd) {
    
    case SIOCGIFCONF: {                                                 /*  ��������б�                */
            struct ifconf *pifconf = (struct ifconf *)pvArg;
            __ifConf(pifconf);
            iRet = ERROR_NONE;
            break;
        }
        
    case SIOCGIFNUM: {                                                  /*  �������ӿ�����            */
            if (pvArg) {
                *(INT *)pvArg = (INT)netif_get_total();
                iRet = ERROR_NONE;
            } else {
                _ErrorHandle(EINVAL);
            }
            break;
        }
    
    case SIOCGSIZIFCONF: {                                              /*  SIOCGIFCONF ���軺���С    */
            INT  *piSize = (INT *)pvArg;
            __ifConfSize(piSize);
            iRet = ERROR_NONE;
            break;
        }
    
    case SIOCGSIZIFREQ6: {                                              /*  ���ָ������ ipv6 ��ַ����  */
            struct in6_ifreq *pifreq6 = (struct in6_ifreq *)pvArg;
            iRet = __ifReq6Size(pifreq6);
            break;
        }
    }
    
    return  (iRet);
}
/*********************************************************************************************************
** ��������: __ifIoctlInet
** ��������: INET ����ӿ� ioctl ����
** �䡡��  : iCmd      ����
**           pvArg     ����
** �䡡��  : �������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  __ifIoctlInet (INT  iCmd, PVOID  pvArg)
{
    INT     iRet = PX_ERROR;
    
    if (pvArg == LW_NULL) {
        _ErrorHandle(EINVAL);
        return  (iRet);
    }
    
    if (iCmd == SIOCGIFNAME) {                                          /*  ���������                  */
        struct ifreq *pifreq = (struct ifreq *)pvArg;
        if (if_indextoname(pifreq->ifr_ifindex, pifreq->ifr_name)) {
            iRet = ERROR_NONE;
        }
        return  (iRet);
    }
    
    switch (iCmd) {                                                     /*  ����Ԥ����                  */
    
    case SIOCGIFCONF:                                                   /*  ͨ������ӿڲ���            */
    case SIOCGIFNUM:
    case SIOCGSIZIFCONF:
    case SIOCGSIZIFREQ6:
        LWIP_IF_LIST_LOCK(LW_FALSE);                                    /*  �����ٽ���                  */
        iRet = __ifSubIoctlCommon(iCmd, pvArg);
        LWIP_IF_LIST_UNLOCK();                                          /*  �˳��ٽ���                  */
        break;
    
    case SIOCSIFFLAGS:                                                  /*  ��������ӿڲ���            */
    case SIOCGIFFLAGS:
    case SIOCGIFTYPE:
    case SIOCGIFINDEX:
    case SIOCGIFMTU:
    case SIOCSIFMTU:
    case SIOCGIFHWADDR:
    case SIOCSIFHWADDR:
    case SIOCGIFMETRIC:
    case SIOCSIFMETRIC:
    case SIOCADDMULTI:
    case SIOCDELMULTI:
        LWIP_IF_LIST_LOCK(LW_FALSE);                                    /*  �����ٽ���                  */
        iRet = __ifSubIoctlIf(iCmd, pvArg);
        LWIP_IF_LIST_UNLOCK();                                          /*  �˳��ٽ���                  */
        break;
    
    case SIOCGIFADDR:                                                   /*  ipv4 ����                   */
    case SIOCGIFNETMASK:
    case SIOCGIFDSTADDR:
    case SIOCGIFBRDADDR:
    case SIOCSIFADDR:
    case SIOCSIFNETMASK:
    case SIOCSIFDSTADDR:
    case SIOCSIFBRDADDR:
        LWIP_IF_LIST_LOCK(LW_FALSE);                                    /*  �����ٽ���                  */
        iRet = __ifSubIoctl4(iCmd, pvArg);
        LWIP_IF_LIST_UNLOCK();                                          /*  �˳��ٽ���                  */
        break;
        
    case SIOCGIFADDR6:                                                  /*  ipv6 ����                   */
    case SIOCGIFNETMASK6:
    case SIOCGIFDSTADDR6:
    case SIOCSIFADDR6:
    case SIOCSIFNETMASK6:
    case SIOCSIFDSTADDR6:
    case SIOCDIFADDR6:
        LWIP_IF_LIST_LOCK(LW_FALSE);                                    /*  �����ٽ���                  */
        iRet = __ifSubIoctl6(iCmd, pvArg);
        LWIP_IF_LIST_UNLOCK();                                          /*  �˳��ٽ���                  */
        break;
    
    default:
        _ErrorHandle(ENOSYS);
        break;
    }
    
    return  (iRet);
}
/*********************************************************************************************************
** ��������: __ifIoctlWireless
** ��������: WEXT ����ӿ� ioctl ����
** �䡡��  : iCmd      ����
**           pvArg     ����
** �䡡��  : �������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
#if LW_CFG_NET_WIRELESS_EN > 0

INT  __ifIoctlWireless (INT  iCmd, PVOID  pvArg)
{
    INT     iRet = PX_ERROR;
    
    if ((iCmd >= SIOCIWFIRST) && (iCmd <= SIOCIWLASTPRIV)) {            /*  ������������                */
        LWIP_IF_LIST_LOCK(LW_FALSE);                                    /*  �����ٽ���                  */
        iRet = wext_handle_ioctl(iCmd, (struct ifreq *)pvArg);
        LWIP_IF_LIST_UNLOCK();                                          /*  �˳��ٽ���                  */
        if (iRet) {
            _ErrorHandle(lib_abs(iRet));
            iRet = PX_ERROR;
        }
    } else {
        _ErrorHandle(ENOSYS);
    }
    
    return  (iRet);
}

#endif                                                                  /*  LW_CFG_NET_WIRELESS_EN > 0  */
/*********************************************************************************************************
** ��������: __ifIoctlPacket
** ��������: PACKET ����ӿ� ioctl ����
** �䡡��  : iCmd      ����
**           pvArg     ����
** �䡡��  : �������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  __ifIoctlPacket (INT  iCmd, PVOID  pvArg)
{
    INT     iRet = PX_ERROR;
    
    if (pvArg == LW_NULL) {
        _ErrorHandle(EINVAL);
        return  (iRet);
    }
    
    if (iCmd == SIOCGIFNAME) {                                          /*  ���������                  */
        struct ifreq *pifreq = (struct ifreq *)pvArg;
        if (if_indextoname(pifreq->ifr_ifindex, pifreq->ifr_name)) {
            iRet = ERROR_NONE;
        }
        return  (iRet);
    }
    
    switch (iCmd) {                                                     /*  ����Ԥ����                  */
    
    case SIOCGIFCONF:                                                   /*  ͨ������ӿڲ���            */
    case SIOCGIFNUM:
    case SIOCGSIZIFCONF:
    case SIOCGSIZIFREQ6:
        LWIP_IF_LIST_LOCK(LW_FALSE);                                    /*  �����ٽ���                  */
        iRet = __ifSubIoctlCommon(iCmd, pvArg);
        LWIP_IF_LIST_UNLOCK();                                          /*  �˳��ٽ���                  */
        break;
    
    case SIOCSIFFLAGS:                                                  /*  ��������ӿڲ���            */
    case SIOCGIFFLAGS:
    case SIOCGIFTYPE:
    case SIOCGIFINDEX:
    case SIOCGIFMTU:
    case SIOCSIFMTU:
    case SIOCGIFHWADDR:
    case SIOCSIFHWADDR:
        LWIP_IF_LIST_LOCK(LW_FALSE);                                    /*  �����ٽ���                  */
        iRet = __ifSubIoctlIf(iCmd, pvArg);
        LWIP_IF_LIST_UNLOCK();                                          /*  �˳��ٽ���                  */
        break;
    
    default:
        _ErrorHandle(ENOSYS);
        break;
    }
    
    return  (iRet);
}

#endif                                                                  /*  LW_CFG_NET_EN               */
/*********************************************************************************************************
  END
*********************************************************************************************************/