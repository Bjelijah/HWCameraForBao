#ifndef __HW_DEBUG_H__
#define __HW_DEBUG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#ifdef WIN32 

#define HW_DBG_PRINT(args...)					\
	do{											\
		TRACE(args);							\
	}while(0)

#else

#define HW_DBG_PRINT(args...)					\
	do{											\
		printf(args);							\
	}while(0)


#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HW_DEBUG_H__ */
