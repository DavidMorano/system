/* usupport HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORT_INCLUDE
#define	USUPPORT_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/time.h>
#include	<time.h>
#include	<string.h>		/* <- for |memset(3c)| */

#include	<usys.h>		/* <- auxillary OS support */

#include	<stdint.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<utimeout.h>
#include	<msleep.h>
#include	<mtime.h>
#include	<timecount.hh>
#include	<ulogerror.h>
#include	<filetype.h>

#ifdef	__cplusplus
#include	<aflag.hh>
#endif /* __cplusplus */


#ifndef	SUBROUTINE_MEMCLEAR
#define	SUBROUTINE_MEMCLEAR
EXTERNC_begin
int memclear(void *,size_t) noex ;
EXTERNC_end
#endif /* SUBROUTINE_MEMCLEAR */


#ifndef	TEMPLATE_MEMCLEAR
#define	TEMPLATE_MEMCLEAR
#ifdef	__cplusplus

template<typename T>
int memclear(T *op) noex {
	cint	osz = sizeof(T) ;
	return memclear(op,osz) ;
}

#endif /* __cplusplus */
#endif /* TEMPLATE_MEMCLEAR */


#ifndef	SUBROUTINE_FILETYPE
#define	SUBROUTINE_FILETYPE
EXTERNC_begin
extern int filetype(mode_t) noex ;
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


#endif /* USUPPORT_INCLUDE */


