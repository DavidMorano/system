/* usupport_primet HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTPRIME_INCLUDE
#define	USUPPORTPRIME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<time.h>		/* |time_t| */
#include	<string.h>		/* |memset(3c)| + |memcpy(3c)| */
#include	<stdint.h>
#include	<clanguage.h>
#include	<usysbase.h>


#ifndef	TYPEDEF_MTIME
#define	TYPEDEF_MTIME
typedef int64_t		mtime_t ;
#endif

#ifndef	SUBROUTINE_UMTIME
#define	SUBROUTINE_UMTIME
EXTERNC_begin
extern mtime_t	umtime(void) noex ;
EXTERNC_end
#endif

#ifndef	SUBROUTINE_MSLEEP
#define	SUBROUTINE_MSLEEP
EXTERNC_begin
extern int	msleep(int) noex ;
EXTERNC_end
#endif

#ifndef	OBJECT_GETMTIME
#define	OBJECT_GETMTIME
#ifdef	__cplusplus

struct usys_mtime {
	operator mtime_t () noex {
	    return umtime() ;
	} ;
	static mtime_t operator () () noex {
	    return umtime() ;
	} ;
} ; /* end struct (usys_mtime) */

extern usys_mtime	getusmtime ;


#endif /* __cplusplus */
#endif /* OBJECT_GETMTIME */

#ifndef	OBJECT_GETUSTIME
#define	OBJECT_GETUSTIME
#ifdef	__cplusplus

namespace libu {
    struct ugetustime {
	operator time_t () noex {
	    return time(nullptr) ;
	} ;
	static time_t operator () () noex {
	    return time(nullptr) ;
	} ;
    } ; /* end struct (getustime) */
} /* end namespace (libu) */

extern libu::ugetustime		getustime ;

#endif /* __cplusplus */
#endif /* OBJECT_GETUSTIME */

#ifdef	__cplusplus
namespace libu {
    extern char *strwcpy(char *,cchar *,int = -1) noex ;
}
namespace libu {
    extern int ustrftime(char *,int,cchar *,CTM *) noex ;
}
#endif /* __cplusplus */


#endif /* USUPPORTPRIME_INCLUDE */


