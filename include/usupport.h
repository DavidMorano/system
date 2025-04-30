/* usupport HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* UNIX� kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	USUPPORT_INCLUDE
#define	USUPPORT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<time.h>		/* |time_t| */
#include	<string.h>		/* |memset(3c)| + |memcpy(3c)| */
#include	<stdint.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<memx.h>
#include	<usys.h>		/* <- auxillary OS support */
#include	<utimeout.h>
#include	<ulogerror.h>
#include	<usysutility.hh>
#include	<timecount.hh>
#include	<filetype.h>
#include	<aflag.hh>
#include	<stdintx.h>
#include	<intx.h>
#include	<libutil.hh>


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
} ; /* end struct (getustime) */

extern usys_mtime	mtime ;
extern usys_mtime	getmtime ;

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
}
extern libu::ugetustime	getustime ;
#endif /* __cplusplus */
#endif /* OBJECT_GETUSTIME */

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
    extern char *strwcpy(char *,cchar *,int = -1) noex ;
}
namespace libu {
    extern int snwcpy(char *,int,cchar *,int = -1) noex ;
    extern int sncpy1(char *,int,cchar *) noex ;
    inline int sncpy(char *dp,int dl,cchar *sp) noex {
	return sncpy1(dp,dl,sp) ;
    }
}
namespace libu {
    extern int ctdecui(char *,int,uint) noex ;
    extern int ctdecul(char *,int,ulong) noex ;
    extern int ctdecull(char *,int,ulonglong) noex ;
    template<typename T> inline int ctdec(char *,int,T v) noex {
	return 0 ;
    }
    template<> inline int ctdec(char *dp,int dl,uint v) noex {
	return ctdecui(dp,dl,v) ;
    }
    template<> inline int ctdec(char *dp,int dl,ulong v) noex {
	return ctdecul(dp,dl,v) ;
    }
    template<> inline int ctdec(char *dp,int dl,ulonglong v) noex {
	return ctdecull(dp,dl,v) ;
    }
}
namespace libu {
    extern int ustrftime(char *,int,cchar *,CTM *) noex ;
    extern int uitimer_get(int,ITIMERVAL *) noex ;
    extern int uitimer_set(int,CITIMERVAL *,ITIMERVAL *) noex ;
}
namespace libu {
    extern int cfdec(cchar *,int,int *) noex ;
    extern int cfdec(cchar *,int,long *) noex ;
    extern int cfdec(cchar *,int,longlong *) noex ;
}
#endif /* __cplusplus */


#endif /* USUPPORT_INCLUDE */


