/* uumisc HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UMISC_INCLUDE
#define	UMISC_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>		/* |statfs| */
#include	<sys/mount.h>		/* |statfs| */
#include	<sys/statvfs.h>		/* |statvfs| */
#include	<sys/stat.h>		/* |ustat| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


namespace libu {
    extern int uatfork(void_f,void_f,void_f) noex ;
    extern int uatexit(void_f) noex ;
    extern int ustatfile(cchar *,ustat *) noex ;
    extern int ustatfilefs(cchar *,ustatfs *) noex ;
    extern int ustatfilevfs(cchar *,ustatvfs *) noex ;
    extern int ugetcwd(char *,int) noex ;
    extern int uaccess(cchar *,int) noex ;
    extern int udiraccess(cchar *,int) noex ;
    static inline int ugetstat(cchar *fn,ustat *sp) noex {
	return ustatfile(fn,sp) ;
    }
    static inline int ugetstatfs(cchar *fn,ustatfs *sp) noex {
	return ustatfilefs(fn,sp) ;
    }
    static inline int ugetstatvfs(cchar *fn,ustatvfs *sp) noex {
	return ustatfilevfs(fn,sp) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* UMISC_INCLUDE */


