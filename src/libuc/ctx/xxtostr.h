/* xxtostr HEADER */
/* lang=C20,C++20 */

/* subroutines to convert an integer to a sring (base-10) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	XXTOSTR_INCLUDE
#define	XXTOSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<usystem.h>
#include	<ucvariables.hh>
#include	<varnames.hh>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus

static const int	xxtostr_maxbase = strlen(varname.digtab) ;

template<typename UT>
inline constexpr int uxxtostr(char *endp,int b,UT v) noex {
	int		rs = SR_FAULT ;
	char		*rp = endp ;
	if (endp) {
	    rs = SR_NOTSUP ;
	    if ((b >= 2) && (b <= xxtostr_maxbase)) {
	        *rp = '\0' ;
	        if (v != 0) {
                    int		di ;
	            if constexpr (sizeof(UT) > sizeof(ulong)) {
	                const UT	vmask(~LONG_MAX) ;
		        UT		nv ;
	                while ((v & vmask) != 0L) {
	                    nv = v / b ;
                            di = int(v - (nv * b)) ;
                            *--rp = varname.digtab[di] ;
	                    v = nv ;
	                } /* end while (slower) */
	                {
		            ulong	lv = ulong(v) ;
		            ulong	nv ;
		            while (lv != 0) {
	                        nv = lv / b ;
                                di = int(lv - (nv * b)) ;
                                *--rp = varname.digtab[di] ;
	                        lv = nv ;
		            } /* end while */
		            v = lv ;
	                } /* end block (faster) */
	            } else {
		        UT		nv ;
	                while (v != 0L) {
	                    nv = v / b ;
                            di = int(v - (nv * b)) ;
                            *--rp = varname.digtab[di] ;
	                    v = nv ;
	                } /* end while (regular) */
		    } /* end if (constexpr) */
	            rs = SR_OK ;
	        } else {
	            rs = SR_OK ;
	            *--rp = '0' ;
	        } /* end if */
	    } /* end if (base supported) */
	} /* end if (non-null) */
	return (rs >= 0) ? (endp - rp) : rs ;
}
/* end subroutine-template (uxxtostr) */

template<typename T,typename UT>
inline constexpr int sxxtostr(char *endp,int b,UT,T v) noex {
	UT		ulv = (UT) v ;
	int		rs = SR_FAULT ;
	char		*rp = nullptr ;
	if (v < 0) ulv = (- ulv) ;
	if (endp) {
	    if ((rs  = uxxtostr(endp,b,ulv)) >= 0) {
		cint	len = rs ;
		rp = (endp - len) ;
	        if (v < 0) *--rp = '-' ;
	    }
	}
	return (rs >= 0) ? (endp - rp) : rs ;
}
/* end subroutine (ctdecx) */

#endif /* __cplusplus */

EXTERNC_begin

char *itostr(int,char *) noex ;
char *ltostr(long,char *) noex ;
char *lltostr(longlong,char *) noex ;

char *uitostr(uint,char *) noex ;
char *ultostr(ulong,char *) noex ;
char *ulltostr(ulonglong,char *) noex ;

EXTERNC_end


#endif /* XXTOSTR_INCLUDE */


