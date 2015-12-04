#define MEMSET_DEBUG(vBuf, iSet, iSize) SETMEM(vBuf, iSet, iSize,__LINE__,__FILE__)
#define MEMCPY_DEBUG(vBuf1, vBuf2, iSize) CPYMEM(vBuf1, vBuf2, iSize,__LINE__,__FILE__)
