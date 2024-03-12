/* stdorder SUPPORT */
/* lang=C++20 */

/* standard byte-order serializations */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These routines provide for serialization of various variables
	(of essentially the standard scalar-integer types) for use
	in various messaging applications.

	Subroutines are provided to read and write (from and to a
	byte-serial buffer) host native variables of the following types:
	+ char
	+ short
	+ int
	+ long
	+ longlong

	Both signed and unsigned variables are supported.
	The buffer bytes are usually read or written to the network
	or some other messaging channel.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<climits>
#include	<cstdint>
#include	<stdintx.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"stdorder.h"


/* local defines */


/* imported namespaces */


/* subroutine templates */

template<typename T>
int stdorder_rxx(cchar *buf,T *wp) noex {
	static constexpr int	nb = CHAR_BIT ;
	static constexpr int	n = sizeof(T) ;
	T		stage = 0 ;
	const uchar	*ubuf = (uchar *) buf ;
	for (int i = 0 ; i < n ; i += 1) {
	     T	v = ubuf[i] ;
	     stage |= (v << (i * nb)) ;
	}
	*wp = stage ;
	return n ;
}
/* end subroutine-template (stdorder_rxx) */

template<typename T>
int stdorder_wxx(char *buf,T w) noex {
	static constexpr int	nb = CHAR_BIT ;
	static constexpr int	n = sizeof(T) ;
	uchar	*ubuf = (uchar *) buf ;
	for (int i = 0 ; i < n ; i += 1) {
	    ubuf[i] = uchar(w >> (i * nb)) ;
	}
	return n ;
}
/* end subroutine-template (stdorder_wxx) */


/* exported subroutines */

int stdorder_ruc(cchar *buf,uchar *wp) noex {
	const uchar	*ubuf = (const uchar *) buf ;
	*wp = ubuf[0] ;
	return 1 ;
}

int stdorder_rus(cchar *buf,ushort *wp) noex {
	return stdorder_rxx(buf,wp) ;
}

int stdorder_rui(cchar *buf,uint *wp) noex {
	return stdorder_rxx(buf,wp) ;
}

int stdorder_rul(cchar *buf,ulong *wp) noex {
	return stdorder_rxx(buf,wp) ;
}

int stdorder_rull(cchar *buf,ulonglong *wp) noex {
	return stdorder_rxx(buf,wp) ;
}

int stdorder_rc(cchar *buf,char *wp) noex {
	uchar		*ubuf = (uchar *) buf ;
	*wp = ubuf[0] ;
	return 1 ;
}

int stdorder_rs(cchar *buf,short *sp) noex {
	ushort		*up = (ushort *) sp ;
	return stdorder_rus(buf,up) ;
}

int stdorder_ri(cchar *buf,int *sp) noex {
	uint		*up = (uint *) sp ;
	return stdorder_rui(buf,up) ;
}

int stdorder_rl(cchar *buf,long *sp) noex {
	ulong		*up = (ulong *) sp ;
	return stdorder_rul(buf,up) ;
}

int stdorder_rll(cchar *buf,longlong *sp) noex {
	ulonglong	*up = (ulonglong *) sp ;
	return stdorder_rull(buf,up) ;
}

int stdorder_wuc(char *buf,uint uw) noex {
	uchar		*ubuf = (uchar *) buf ;
	ubuf[0] = uchar(uw) ;
	return 1 ;
}

int stdorder_wus(char *buf,uint uw) noex {
	static constexpr int	nb = CHAR_BIT ;
	uchar		*ubuf = (uchar *) buf ;
	ubuf[0] = uchar(uw >> (0 * nb)) ;
	ubuf[1] = uchar(uw >> (1 * nb)) ;
	return 2 ;
}

int stdorder_wui(char *buf,uint uw) noex {
	return stdorder_wxx(buf,uw) ;
}

int stdorder_wul(char *buf,ulong uw) noex {
	return stdorder_wxx(buf,uw) ;
}

int stdorder_wull(char *buf,ulonglong uw) noex {
	return stdorder_wxx(buf,uw) ;
}

int stdorder_wc(char *buf,int sw) noex {
	uchar		*ubuf = (uchar *) buf ;
	ubuf[0] = (uchar) sw ;
	return 1 ;
}

int stdorder_ws(char *buf,int sw) noex {
	static constexpr int	nb = CHAR_BIT ;
	uchar		*ubuf = (uchar *) buf ;
	ubuf[0] = uchar(sw >> (0 * nb)) ;
	ubuf[1] = uchar(sw >> (1 * nb)) ;
	return 2 ;
}

int stdorder_wi(char *buf,int sw) noex {
	uint		uw = (uint) sw ;
	return stdorder_wui(buf,uw) ;
}

int stdorder_wl(char *buf,long sw) noex {
	ulong		uw = (ulong) sw ;
	return stdorder_wul(buf,uw) ;
}

int stdorder_wll(char *buf,longlong sw) noex {
	ulonglong	uw = (ulonglong) sw ;
	return stdorder_wull(buf,uw) ;
}


