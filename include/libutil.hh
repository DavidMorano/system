/* libutil HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LIBUTIL_INCLUDE
#define	LIBUTIL_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| + |strnlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#if defined(__has_builtin)
#  if __has_builtin(__builtin_addc)
#define	F_ADDC	1
constexpr inline uint builtin_addc(uint v0,uint v1,uint ci,
		uint *cop) noex {
    return __builtin_addc(v0,v1,ci,cop) ;
}
constexpr inline ulong builtin_addc(ulong v0,ulong v1,ulong ci,
		ulong *cop) noex {
    return __builtin_addcl(v0,v1,ci,cop) ;
}
#  else
#define	F_ADDC	0
constexpr inline uint builtin_addc(uint v0,uint v1,uint ci,
		uint *) noex {
    return v0 + v1 + ci ;
}
constexpr inline ulong builtin_addc(ulong v0,ulong v1,ulong ci,
		ulong *) noex {
    return v0 + v1 + ci ;
}
#  endif
#else
#define	F_ADDC	0
constexpr inline uint builtin_addc(uint v0,uint v1,uint ci,
		uint *) noex {
    return v0 + v1 + ci ;
}
constexpr inline ulong builtin_addc(ulong v0,ulong v1,ulong ci,
		ulong *) noex {
    return v0 + v1 + ci ;
}
#endif /* defined(__has_builtin) */

constexpr inline cbool		f_machaddc = F_ADDC ;

#ifndef	SUBROUTINE_CSTRNLEN
#define	SUBROUTINE_CSTRNLEN
constexpr inline int cstrnlen(cchar *sp,int sl = -1) noex {
    	cchar		*cp = sp ;
	while (*cp++ && sl) {
	    sl -= 1 ;
	}
	return int(cp - sp) ;
}
#endif /* SUBROUTINE_CSTRNLEN */

#ifndef	SUBROUTINE_CSTRLEN
#define	SUBROUTINE_CSTRLEN
constexpr inline int cstrlen(cchar *sp) noex {
    	int		len = 0 ;
	while (sp[len]) len += 1 ;
	return len ;
}
#endif /* SUBROUTINE_CSTRLEN */

#ifndef	SUBROUTINE_XSTRNLEN
#define	SUBROUTINE_XSTRNLEN
constexpr inline int xstrnlen(cchar *sp,int sl) noex {
    	int		len = 0 ;
	if (sl >= 0) {
    	    csize	ssize = size_t(sl) ;
	    len = intconv(strnlen(sp,ssize)) ;
	} else {
	    len = intconv(strlen(sp)) ;
	}
    	return len ;
}
#endif /* SUBROUTINE_XSTRNLEN */

#ifndef	SUBROUTINE_XSTRLEN
#define	SUBROUTINE_XSTRLEN
constexpr inline int xstrlen(cchar *sp) noex {
    	return intconv(strlen(sp)) ;
}
#endif /* SUBROUTINE_XSTRLEN */

#ifndef	SUBROUTINE_XSTRCSPN
#define	SUBROUTINE_XSTRCSPN
constexpr inline int xstrcspn(cchar *sp,cchar *ss) noex {
    	return intconv(strcspn(sp,ss)) ;
}
#endif	/* SUBROUTINE_XSTRCSPN */

#ifndef	SUBROUTINE_XSTRCPY
#define	SUBROUTINE_XSTRCPY
constexpr inline char *xstrcpy(char *dp,cchar *ss) noex {
    	return stpcpy(dp,ss) ;
}
#endif	/* SUBROUTINE_XSTRCPY */

#ifndef	SUBROUTINE_XSTRNCPY
#define	SUBROUTINE_XSTRNCPY
constexpr inline char *xstrcpy(char *dp,cchar *ss,int n) noex {
    	csize	nsize = size_t(n) ;
    	return stpncpy(dp,ss,nsize) ;
}
#endif	/* SUBROUTINE_XSTRNCPY */

#ifndef	SUBROUTINE_XSTPCPY
#define	SUBROUTINE_XSTPCPY
constexpr inline char *xstpcpy(char *dp,cchar *ss) noex {
    	return stpcpy(dp,ss) ;
}
#endif	/* SUBROUTINE_XSTPCPY */

#ifndef	SUBROUTINE_XSTPNCPY
#define	SUBROUTINE_XSTPNCPY
constexpr inline char *xstpncpy(char *dp,cchar *ss,int n) noex {
    	csize	nsize = size_t(n) ;
    	return stpncpy(dp,ss,nsize) ;
}
#endif	/* SUBROUTINE_XSTPNCPY */

template<typename UT> constexpr inline uint mach_addone(UT &r,UT v) noex {
	r = v + 1 ;
	return int((r < v) || (r == 0)) ;
} /* end subroutine (mach_addone) */

template<typename UT> 
constexpr inline UT mach_addc(UT v0,UT v1,UT ci,UT *cop) noex {
    	UT		r ;
    	if_constexpr (f_machaddc) {
	    r = builtin_addc(v0,v1,ci,cop) ;
	} else {
    	    uint	co = 0 ;
	    r = v0 + v1 ;
	    co = (r < v0) || (r < v1) ;
	    if (ci) {
	        UT t = r ;
	        co += mach_addone(r,t) ;
	    }
	    if (cop) *cop = UT(co) ;
	} /* end if_constexpr (f_machaddc) */
	return r ;
} /* end subroutine (mach_addc) */


#endif /* __cplusplus (C++ only) */
#endif /* LIBUTIL_INCLUDE */


