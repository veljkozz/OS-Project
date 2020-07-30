#ifndef _DECLARE_
#define _DECLARE_

#define lock asm { pushf; cli; }
#define unlock asm popf;

//#define lock asm cli
//#define unlock asm sti

#define softLock lockFlag = 0;
#define softUnlock lockFlag = 1;

#endif