/* memx HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MEMX_INCLUDE
#define	MEMX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<string.h>		/* |memset(3c)| + |memcpy(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#ifndef	SUBROUTINE_MEMCLEAR
#define	SUBROUTINE_MEMCLEAR
#ifdef	__cplusplus
constexpr inline int memclear(void *objp,int sz) noex {
    	csize	objs = size_t(sz) ;
    	memset(objp,0,objs) ;
	return sz ;
}
#else /* __cplusplus */
static inline int memclear(void *objp,int sz) noex {
    	csize	objs = size_t(sz) ;
    	memset(objp,0,objs) ;
	return sz ;
}
#endif /* __cplusplus */
#endif /* SUBROUTINE_MEMCLEAR */

#ifndef	TEMPLATE_MEMCLEAR
#define	TEMPLATE_MEMCLEAR
#ifdef	__cplusplus
template<typename T>
constexpr inline int memclear(T *op) noex {
	int	sz = szof(T) ;
	return memclear(op,sz) ;
}
#endif /* __cplusplus */
#endif /* TEMPLATE_MEMCLEAR */

#ifndef	TEMPLATE_MEMCPY
#define	TEMPLATE_MEMCPY
#ifdef	__cplusplus
template<typename T>
constexpr inline void *memcpy(T *dp,cvoid *sp) noex {
	csize	dsz = sizeof(T) ;
	return memcpy(dp,sp,dsz) ;
}
#endif /* __cplusplus */
#endif /* TEMPLATE_MEMCPY */


#endif /* MEMX_INCLUDE */


