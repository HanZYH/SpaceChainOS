
����ע�⣺
1. qtumd��Ҫջ��С300��KB����������ջ
# shstack 500000

2. rename������Ҫpatch

libsylixos��libsylixos/SylixOS/system/ioLib/ioInterface.c

rename(...) ������
    if (_PathBuildLink(cFullFileName, MAX_FILENAME_LENGTH, 
                       LW_NULL, LW_NULL,
                       pdevhdrHdr->DEVHDR_pcName, cFullFileName) < ERROR_NONE) {
        close(iFd);
        return  (PX_ERROR);
    }
    
    unlink(cFullFileName); // �������У�������ļ����Ѿ����ڣ�Ҫɾ��

    iRet = ioctl(iFd, FIORENAME, (LONG)cFullFileName);
    
    close(iFd);
    
    if (iRet == ERROR_NONE) {
        MONITOR_EVT(MONITOR_EVENT_ID_IO, MONITOR_EVENT_IO_MOVE_FROM, pcOldName);
        MONITOR_EVT(MONITOR_EVENT_ID_IO, MONITOR_EVENT_IO_MOVE_TO,   pcNewName);
    }
    
    return  (iRet);
}


�ر��ڿ�

-staking=<true/false>
Enables or disables staking (enabled by default) 
-stakecache=<true/false>
Enables or disables the staking cache; significantly improves staking performance, but can use a lot of memory (enabled by default) 

qtum.conf������
staking=0
stakecache=0

���⣬�󲿷�������Դ�help�Ĳ������ҵ��������õ�conf�С�

