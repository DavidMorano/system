/* usupport HEADER */
/* lang=C20 */

/* UNIX-kernal support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORT_INCLUDE
#define	USUPPORT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/time.h>
#include	<time.h>
#include	<string.h>		/* |memset(3c)| + |memcpy(3c)| */
#include	<usys.h>		/* <- auxillary OS support */
#include	<stdint.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<utimeout.h>
#include	<ulogerror.h>
#include	<usys.h>
#include	<timecount.hh>
#include	<filetype.h>
#include	<aflag.hh>
#include	<intsat.h>


#ifndef	SUBROUTINE_MEMCLEAR
#define	SUBROUTINE_MEMCLEAR
EXTERNC_begin
extern int memclear(void *,size_t) noex ;
EXTERNC_end
#endif /* SUBROUTINE_MEMCLEAR */


#ifndef	TEMPLATE_MEMCLEAR
#define	TEMPLATE_MEMCLEAR
#ifdef	__cplusplus

template<typename T>
inline int memclear(T *op) noex {
	csize	osz = sizeof(T) ;
	return memclear(op,osz) ;
}

#endif /* __cplusplus */
#endif /* TEMPLATE_MEMCLEAR */


#ifndef	TEMPLATE_MEMCPY
#define	TEMPLATE_MEMCPY
#ifdef	__cplusplus

template<typename T>
inline void *memcpy(T *dp,void *sp) noex {
	csize	dsz = sizeof(T) ;
	return memcpy(dp,sp,dsz) ;
}

#endif /* __cplusplus */
#endif /* TEMPLATE_MEMCPY */


#ifndef	TYPEDEF_MTIME
#define	TYPEDEF_MTIME
typedef int64_t		mtime_t ;
#endif

#ifndef	SUBROUTINE_MTIME
#define	SUBROUTINE_MTIME
EXTERNC_begin
extern mtime_t	mtime(void) noex ;
EXTERNC_end
#endif


#ifndef	SUBROUTINE_MSLEEP
#define	SUBROUTINE_MSLEEP
EXTERNC_begin
extern int	msleep(int) noex ;
EXTERNC_end
#endif


#ifndef	SUBROUTINE_FILETYPE
#define	SUBROUTINE_FILETYPE
EXTERNC_begin
extern int	filetype(mode_t) noex ;
EXTERNC_end
#endif /* SUBROUTINE_FILETYPE */


#if	defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0)
#else /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */

#ifndef	TYPEDEF_PROJID
#define	TYPEDEF_PROJID
typedef int	projid_t ;
#endif

#ifndef	SUBROUTINE_GETPROJID
#define	SUBROUTINE_GETPROJID
EXTERNC_begin
extern projid_t getprojid() noex ;
EXTERNC_end
#endif /* SUBROUTINE_GETPROJID */

#endif /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */


#ifdef	__cplusplus
namespace libu {
    extern int snwcpy(char *,int,cchar *,int = -1) noex ;
    extern int sncpy1(char *,int,cchar *) noex ;
    extern char *strwcpy(char *,cchar *,int = -1) noex ;
    extern int uitimer_get(int,ITIMERVAL *) noex ;
    extern int uitimer_set(int,CITIMERVAL *,ITIMERVAL *) noex ;
    static inline int sncpy(char *dp,int dl,cchar *sp) noex {
	return sncpy1(dp,dl,sp) ;
    }
}
#endif /* __cplusplus */


#endif /* USUPPORT_INCLUDE */


