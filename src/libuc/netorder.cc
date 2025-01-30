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

	These routines are used to manipulate data that may be in
	the portable "network ordering." This ordering is used on
	network-type transfers. We will be implementing the IP
	protocol stack (UDP but maybe not TCP) on the ETHCON (TN2060)
	circuit pack computer.

	Subroutines are provided to read and write host native:
	+ shorts
	+ ints
	+ longs (64-bit)
	+ longlongs (128-bit)

	out of or into a character buffer. The buffer bytes are
	what are usually read or written to the network.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"netorder.h"


/* exported subroutines */

int netorder_rchar(char *buf,char *cwp) noex {
	cint		rs = sizeof(char) ;
	uchar		*ubuf = (uchar *) buf ;
	*cwp = ubuf[0] ;
	return rs ;
}
/* end subroutine (netorder_rchar) */

int netorder_rshort(char *buf,short *swp) noex {
	cint		rs = sizeof(short) ;
	uchar		*ubuf = (uchar *) buf ;
	*swp = 0 ;
	*swp |= (ubuf[0] << 8) ;
	*swp |= (ubuf[1] << 0) ;
	return rs ;
}
/* end subroutine (netorder_rshort) */

int netorder_rint(char *buf,int *iwp) noex {
	cint		rs = sizeof(int) ;
	uchar		*ubuf = (uchar *) buf ;
	*iwp = 0 ;
	*iwp |= (ubuf[0] << 24) ;
	*iwp |= (ubuf[1] << 16) ;
	*iwp |= (ubuf[2] << 8) ;
	*iwp |= (ubuf[3] << 0) ;
	return rs ;
}
/* end subroutine (netorder_rint) */

int netorder_rlong(char *buf,long *lwp) noex {
	cint		rs = sizeof(long) ;
	uchar		*ubuf = (uchar *) buf ;
	*lwp = 0 ;
	*lwp |= (((long) ubuf[0]) << 56) ;
	*lwp |= (((long) ubuf[1]) << 48) ;
	*lwp |= (((long) ubuf[2]) << 40) ;
	*lwp |= (((long) ubuf[3]) << 32) ;
	*lwp |= (((long) ubuf[4]) << 24) ;
	*lwp |= (((long) ubuf[5]) << 16) ;
	*lwp |= (((long) ubuf[6]) << 8) ;
	*lwp |= (((long) ubuf[7]) << 0) ;
	return rs ;
}
/* end subroutine (netorder_rlong) */

int netorder_rll(char *buf,longlong *lwp) noex {
	cint		rs = sizeof(longlong) ;
	uchar		*ubuf = (uchar *) buf ;
	*lwp = 0 ;
	*lwp |= (((longlong) ubuf[0]) << 56) ;
	*lwp |= (((longlong) ubuf[1]) << 48) ;
	*lwp |= (((longlong) ubuf[2]) << 40) ;
	*lwp |= (((longlong) ubuf[3]) << 32) ;
	*lwp |= (((longlong) ubuf[4]) << 24) ;
	*lwp |= (((longlong) ubuf[5]) << 16) ;
	*lwp |= (((longlong) ubuf[6]) << 8) ;
	*lwp |= (((longlong) ubuf[7]) << 0) ;
	return rs ;
}
/* end subroutine (netorder_rll) */

int netorder_ruchar(char *buf,uchar *cwp) noex {
	cint		rs = sizeof(uchar) ;
	uchar		*ubuf = (uchar *) buf ;
	*cwp = ubuf[0] ;
	return rs ;
}
/* end subroutine (netorder_ruchar) */

int netorder_rushort(char *buf,ushort *swp) noex {
	cint		rs = sizeof(ushort) ;
	uchar		*ubuf = (uchar *) buf ;
	*swp = 0 ;
	*swp |= (ubuf[0] << 8) ;
	*swp |= (ubuf[1] << 0) ;
	return rs ;
}
/* end subroutine (netorder_rushort) */

int netorder_ruint(char *buf,uint *iwp) noex {
	cint		rs = sizeof(uint) ;
	uchar		*ubuf = (uchar *) buf ;
	*iwp = 0 ;
	*iwp |= (ubuf[0] << 24) ;
	*iwp |= (ubuf[1] << 16) ;
	*iwp |= (ubuf[2] << 8) ;
	*iwp |= (ubuf[3] << 0) ;
	return rs ;
}
/* end subroutine (netorder_ruint) */

int netorder_rulong(char *buf,ulong *lwp) noex {
	cint		rs = sizeof(ulong) ;
	uchar		*ubuf = (uchar *) buf ;
	*lwp = 0 ;
	*lwp |= (((ulong) ubuf[0]) << 56) ;
	*lwp |= (((ulong) ubuf[1]) << 48) ;
	*lwp |= (((ulong) ubuf[2]) << 40) ;
	*lwp |= (((ulong) ubuf[3]) << 32) ;
	*lwp |= (((ulong) ubuf[4]) << 24) ;
	*lwp |= (((ulong) ubuf[5]) << 16) ;
	*lwp |= (((ulong) ubuf[6]) << 8) ;
	*lwp |= (((ulong) ubuf[7]) << 0) ;
	return rs ;
}
/* end subroutine (netorder_rulong) */

int netorder_rull(char *buf,ulonglong *lwp) noex {
	cint		rs = sizeof(ulonglong) ;
	uchar		*ubuf = (uchar *) buf ;
	*lwp = 0 ;
	*lwp |= (((ulonglong) ubuf[0]) << 56) ;
	*lwp |= (((ulonglong) ubuf[1]) << 48) ;
	*lwp |= (((ulonglong) ubuf[2]) << 40) ;
	*lwp |= (((ulonglong) ubuf[3]) << 32) ;
	*lwp |= (((ulonglong) ubuf[4]) << 24) ;
	*lwp |= (((ulonglong) ubuf[5]) << 16) ;
	*lwp |= (((ulonglong) ubuf[6]) << 8) ;
	*lwp |= (((ulonglong) ubuf[7]) << 0) ;
	return rs ;
}
/* end subroutine (netorder_rull) */

int netorder_wchar(char *buf,int cw) noex {
	cint		rs = sizeof(char) ;
	uchar		*ubuf = (uchar *) buf ;
	ubuf[0] = (uchar) cw ;
	return rs ;
}
/* end subroutine (netorder_wchar) */

int netorder_wshort(char *buf,int sw) noex {
	cint		rs = sizeof(short) ;
	uchar		*ubuf = (uchar *) buf ;
	ubuf[0] = (uchar) ((sw >> 8) & 0xff) ;
	ubuf[1] = (uchar) ((sw >> 0) & 0xff) ;
	return rs ;
}
/* end subroutine (netorder_wshort) */

int netorder_wint(char *buf,int iw) noex {
	cint		rs = sizeof(int) ;
	uchar		*ubuf = (uchar *) buf ;
	ubuf[0] = (uchar) ((iw >> 24) & 0xff) ;
	ubuf[1] = (uchar) ((iw >> 16) & 0xff) ;
	ubuf[2] = (uchar) ((iw >> 8) & 0xff) ;
	ubuf[3] = (uchar) ((iw >> 0) & 0xff) ;
	return rs ;
}
/* end subroutine (netorder_wint) */

int netorder_wlong(char *buf,long lw) noex {
	cint		rs = sizeof(long) ;
	uchar		*ubuf = (uchar *) buf ;
	ubuf[0] = ((lw >> 56) & 0xff) ;
	ubuf[1] = ((lw >> 48) & 0xff) ;
	ubuf[2] = ((lw >> 40) & 0xff) ;
	ubuf[3] = ((lw >> 32) & 0xff) ;
	ubuf[4] = ((lw >> 24) & 0xff) ;
	ubuf[5] = ((lw >> 16) & 0xff) ;
	ubuf[6] = ((lw >> 8) & 0xff) ;
	ubuf[7] = ((lw >> 0) & 0xff) ;
	return rs ;
}
/* end subroutine (netorder_wlong) */

int netorder_wll(char *buf,longlong lw) noex {
	cint		rs = sizeof(longlong) ;
	uchar		*ubuf = (uchar *) buf ;
	ubuf[0] = ((lw >> 56) & 0xff) ;
	ubuf[1] = ((lw >> 48) & 0xff) ;
	ubuf[2] = ((lw >> 40) & 0xff) ;
	ubuf[3] = ((lw >> 32) & 0xff) ;
	ubuf[4] = ((lw >> 24) & 0xff) ;
	ubuf[5] = ((lw >> 16) & 0xff) ;
	ubuf[6] = ((lw >> 8) & 0xff) ;
	ubuf[7] = ((lw >> 0) & 0xff) ;
	return rs ;
}
/* end subroutine (netorder_wll) */


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


