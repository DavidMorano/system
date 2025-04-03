/* netorder SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* network byte order manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code was written from scratch to get some portable
	network ordering for Ethernet (ETHCON) development work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	netorder

	Description:
	These routines are used to manipulate data that may be in
	the portable "network ordering." This ordering is used on
	network-type transfers.  We will be implementing the IP
	protocol stack (UDP but maybe not TCP) on the ETHCON (TN2060)
	circuit pack computer.

	Subroutines are provided to read and write host native:
	+ shorts
	+ ints
	+ longs (64-bit)
	+ longlongs (128-bit)

	out of or into a character buffer.  The buffer bytes are
	what are usually read or written to the network.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"netorder.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cint	nb = CHAR_BIT ;


/* subroutine-templates */

template<typename T>
static int netorder_rxx(cchar *buf,T *wp) noex {
	cint		n = szof(T) ;
	T		stage = 0 ;
	const uchar	*ubuf = ucharp(buf) ;
	for (int i = (n-1) ; i >= 0 ; i -= 1) {
	     T	v = ubuf[i] ;
	     stage |= (v << (i * nb)) ;
	}
	*wp = stage ;
	return n ;
}
/* end subroutine-template (netorder_rxx) */

template<typename T>
static int netorder_wxx(char *buf,T w) noex {
	cint		n = szof(T) ;
	uchar		*ubuf = ucharp(buf) ;
	for (int i = (n-1) ; i >= 0 ; i -= 1) {
	    ubuf[i] = uchar(w) ;
	    w >>= nb ;
	}
	return n ;
}
/* end subroutine-template (netorder_wxx) */


/* exported variables */


/* exported subroutines */

int netorder_rc(char *buf,char *cwp) noex {
	cint		rs = szof(char) ;
	uchar		*ubuf = (uchar *) buf ;
	*cwp = ubuf[0] ;
	return rs ;
}
/* end subroutine (netorder_rc) */

int netorder_rs(char *buf,short *swp) noex {
	cint		rs = szof(short) ;
	uchar		*ubuf = (uchar *) buf ;
	*swp = 0 ;
	*swp |= (ubuf[0] << 8) ;
	*swp |= (ubuf[1] << 0) ;
	return rs ;
}
/* end subroutine (netorder_rs) */

int netorder_ri(char *buf,int *iwp) noex {
	cint		rs = szof(int) ;
	uchar		*ubuf = (uchar *) buf ;
	*iwp = 0 ;
	*iwp |= (ubuf[0] << 24) ;
	*iwp |= (ubuf[1] << 16) ;
	*iwp |= (ubuf[2] << 8) ;
	*iwp |= (ubuf[3] << 0) ;
	return rs ;
}
/* end subroutine (netorder_ri) */

int netorder_rl(char *buf,long *lwp) noex {
	return netorder_rxx(buf,lwp) ;
}
/* end subroutine (netorder_rl) */

int netorder_rll(char *buf,longlong *llwp) noex {
	return netorder_rxx(buf,llwp) ;
}
/* end subroutine (netorder_rll) */

int netorder_ruc(char *buf,uchar *cwp) noex {
	cint		rs = szof(uchar) ;
	uchar		*ubuf = (uchar *) buf ;
	*cwp = ubuf[0] ;
	return rs ;
}
/* end subroutine (netorder_ruc) */

int netorder_rus(char *buf,ushort *swp) noex {
	cint		rs = szof(ushort) ;
	uchar		*ubuf = (uchar *) buf ;
	*swp = 0 ;
	*swp |= (ubuf[0] << 8) ;
	*swp |= (ubuf[1] << 0) ;
	return rs ;
}
/* end subroutine (netorder_rus) */

int netorder_rui(char *buf,uint *iwp) noex {
	cint		rs = szof(uint) ;
	uchar		*ubuf = (uchar *) buf ;
	*iwp = 0 ;
	*iwp |= (ubuf[0] << 24) ;
	*iwp |= (ubuf[1] << 16) ;
	*iwp |= (ubuf[2] << 8) ;
	*iwp |= (ubuf[3] << 0) ;
	return rs ;
}
/* end subroutine (netorder_rui) */

int netorder_rul(char *buf,ulong *lwp) noex {
	return netorder_rxx(buf,lwp) ;
}
/* end subroutine (netorder_rul) */

int netorder_rull(char *buf,ulonglong *llwp) noex {
	return netorder_rxx(buf,llwp) ;
}
/* end subroutine (netorder_rull) */

int netorder_wc(char *buf,int cw) noex {
	return netorder_wxx(buf,cw) ;
}
/* end subroutine (netorder_wc) */

int netorder_ws(char *buf,int sw) noex {
	return netorder_wxx(buf,sw) ;
}
/* end subroutine (netorder_ws) */

int netorder_wi(char *buf,int iw) noex {
	return netorder_wxx(buf,iw) ;
}
/* end subroutine (netorder_wi) */

int netorder_wl(char *buf,long lw) noex {
	return netorder_wxx(buf,lw) ;
}
/* end subroutine (netorder_wl) */

int netorder_wll(char *buf,longlong llw) noex {
	return netorder_wxx(buf,llw) ;
}
/* end subroutine (netorder_wll) */

int netorder_wuc(char *buf,uint ucw) noex {
	return netorder_wxx(buf,ucw) ;
}
/* end subroutine (netorder_wuc) */

int netorder_wus(char *buf,uint usw) noex {
	return netorder_wxx(buf,usw) ;
}
/* end subroutine (netorder_wus) */

int netorder_wui(char *buf,uint uiw) noex {
	return netorder_wxx(buf,uiw) ;
}
/* end subroutine (netorder_wui) */

int netorder_wul(char *buf,ulong ulw) noex {
	return netorder_wxx(buf,ulw) ;
}
/* end subroutine (netorder_wul) */

int netorder_wull(char *buf,ulonglong ullw) noex {
	return netorder_wxx(buf,ullw) ;
}
/* end subroutine (netorder_wull) */


/* older API */

#ifdef	COMMENT

int netorder_readl(char *buf,ulonglong *ulwp) noex {
	longlong	*lwp = (longlong *) ulwp ;
	return netorder_rlong(buf,lwp) ;
}

int netorder_writel(char *buf,ulonglong ulw) noex {
	return netorder_wlong(buf,ulw) ;
}

int netorder_readi(char *buf,uint *uiwp) noex {
	int	*iwp = (int *) uiwp ;
	return netorder_rint(buf,iwp) ;
}

int netorder_writei(char *buf,uint uiw) noex {
	return netorder_wint(buf,uiw) ;
}

int netorder_reads(char *buf,ushort *uswp) noex {
	short	*swp = (short *) uswp ;
	return netorder_rshort(buf,swp) ;
}

int netorder_writes(char *buf,uint usw) noex {
	return netorder_wshort(buf,usw) ;
}

#endif /* COMMENT */


