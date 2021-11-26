#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define DO_DEBUG						//uncomment this to output basic debug level msgs on TxD

#ifdef DO_DEBUG
#define DBG(...)			printf( __VA_ARGS__ )
#define NOTIF(...)		        printf( __VA_ARGS__ )
#else
#define DBG printf
#endif

//#define RAND_0_TO_X(x)			( (uint32_t)os_random() / ((~(uint32_t)0)/(x)) ) //get uint32_t random number 0..x (including)

#endif
