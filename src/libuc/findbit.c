/* findbit */
/* lang=C++20 */

/* find bits (meeting certain criteria) */


/* revision history:

	= 1998-03-09, David A�D� Morano
        This group of subroutines was originally written here in C language but
        were inspired by single instructions on the VAX (Digital Equipment 1979)
        that did roughly the same functions.

	= 2001-02-11, David A�D� Morano
        I removed the long (32k-byte) look-up table and instead I am now using
        an algorithm that uses a 128-byte look-up table. I felt that wasting 32k
        bytes for something so relatively unused was not the best use of in-core
        memory. And yes, these things invariably end up in in-core memory due to
        use in shared objects or something else that indirectly calls one or
        more of the subroutines in here.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/******************************************************************************

	Find bits that meet a certain criteria in an integer (or long).

	ffbs[il]	find first bit set [integer, long, longlong]
	flbs[il]	find last bit set [integer, long, longlong]

	ffbc[il]	find first bit clear [integer, long, longlong]
	flbc[il]	find last bit clear [integer, long, longlong]

	Notes:

        There are (obviously) a lot of ways to implement these sorts of
        functions. Doing these functions in hardware used to be the norm in the
        olden days. But the reach for the modern RISC (we no longer count
        machines like the CDC [67]600 variations) screwed that pooch! For the
        |ffbsx()| functions we have traditionally done a bit more work since
        they are more commonly used than the others. We used to use a relatively
        larger look-up table for it, but we have now scaled back to using only a
        128 byte look-up table. We now do the same for the |flbsx()| functions
        since they are also rather commonly used (eg: finding higher powers of
        two). We feel that we are fast enough for our present needs. If you need
        faster performance, you are welcome to do your own!


******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<localmisc.h>


/* local variables */

const unsigned char	ffbstab[] = {
	0x0f, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x06, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x07, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x06, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01,
	0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01
} ;

const unsigned char	flbstab[] = {
	0x0f, 0x11, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77
} ;


/* exported subroutines */

int ffbsi(uint v) noex {
	uint		ti, tr ;
	const int	nb = sizeof(uint) ;
	int		i = -1 ;
	int		b ;
	int		s ;
	int		n ;
	for (b = 0 ; b < nb ; b += 1) {
	    s = (b<<3) ;
	    ti = ((v>>s) & UCHAR_MAX) ;
	    if (ti) {
		tr = ffbstab[ti>>1] ;
		n = (ti&1) ? (tr>>4) : (tr&15) ;
		i = n + s ;
		break ;
	    }
	} /* end for */
	return i ;
}
/* end subroutine (ffbsi) */

int ffbsl(ulong v) noex {
	ulong		ti, tr ;
	const int	nb = sizeof(ulong) ;
	int		i = -1 ;
	int		b ;
	int		s ;
	int		n ;
	for (b = 0 ; b < nb ; b += 1) {
	    s = (b<<3) ;
	    ti = ((v>>s) & UCHAR_MAX) ;
	    if (ti) {
		tr = ffbstab[ti>>1] ;
		n = (ti&1) ? (tr>>4) : (tr&15) ;
		i = n + s ;
		break ;
	    }
	} /* end for */
	return i ;
}
/* end subroutine (ffbsl) */

int ffbsll(ulonglong v) noex {
	ulonglong	ti, tr ;
	const int	nb = sizeof(ulonglong) ;
	int		i = -1 ;
	int		b ;
	int		s ;
	int		n ;
	for (b = 0 ; b < nb ; b += 1) {
	    s = (b<<3) ;
	    ti = ((v>>s) & UCHAR_MAX) ;
	    if (ti) {
		tr = ffbstab[ti>>1] ;
		n = (ti&1) ? (tr>>4) : (tr&15) ;
		i = n + s ;
		break ;
	    }
	} /* end for */
	return i ;
}
/* end subroutine (ffbsll) */

int ffbci(uint v) noex {
	const int	n = (8*sizeof(uint)) ;
	int		i ;
	for (i = 0 ; i < n ; i += 1) {
	    if (! (v & 1)) break ;
	    v = v >> 1 ;
	} /* end for */
	return (i < n) ? i : -1 ;
}
/* end subroutine (ffbci) */

int ffbcl(ulong v) noex {
	const int	n = (8*sizeof(ulong)) ;
	int		i ;
	for (i = 0 ; i < n ; i += 1) {
	    if (! (v & 1)) break ;
	    v = v >> 1 ;
	} /* end for */
	return (i < n) ? i : -1 ;
}
/* end subroutine (ffbcl) */

int ffbcll(ulonglong v) noex {
	const int	n = (8*sizeof(ulonglong)) ;
	int		i ;
	for (i = 0 ; i < n ; i += 1) {
	    if (! (v & 1)) break ;
	    v = v >> 1 ;
	} /* end for */
	return (i < n) ? i : -1 ;
}
/* end subroutine (ffbcll) */

int flbsi(uint v) noex {
	uint		ti, tr ;
	const int	nb = sizeof(uint) ;
	int		i = -1 ;
	int		b ;
	int		s ;
	int		n ;
	for (b = (nb-1) ; b >= 0 ; b -= 1) {
	    s = (b<<3) ;
	    ti = ((v>>s) & UCHAR_MAX) ;
	    if (ti) {
		tr = flbstab[ti>>1] ;
		n = (ti&1) ? (tr>>4) : (tr&15) ;
		i = n + s ;
		break ;
	    }
	} /* end for */
	return i ;
}
/* end subroutine (flbsi) */

int flbsl(ulong v) noex {
	ulong		ti, tr ;
	const int	nb = sizeof(ulong) ;
	int		i = -1 ;
	int		b ;
	int		s ;
	int		n ;
	for (b = (nb-1) ; b >= 0 ; b -= 1) {
	    s = (b<<3) ;
	    ti = ((v>>s) & UCHAR_MAX) ;
	    if (ti) {
		tr = flbstab[ti>>1] ;
		n = (ti&1) ? (tr>>4) : (tr&15) ;
		i = n + s ;
		break ;
	    }
	} /* end for */
	return i ;
}
/* end subroutine (flbsl) */

int flbsll(ulonglong v) noex {
	ulonglong	ti, tr ;
	const int	nb = sizeof(ulonglong) ;
	int		i = -1 ;
	int		b ;
	int		s ;
	int		n ;
	for (b = (nb-1) ; b >= 0 ; b -= 1) {
	    s = (b<<3) ;
	    ti = ((v>>s) & UCHAR_MAX) ;
	    if (ti) {
		tr = flbstab[ti>>1] ;
		n = (ti&1) ? (tr>>4) : (tr&15) ;
		i = n + s ;
		break ;
	    }
	} /* end for */
	return i ;
}
/* end subroutine (flbsll) */

int flbci(uint v) noex {
	const int	n = (8*sizeof(uint)) ;
	int		i ;
	for (i = (n - 1) ; i >= 0 ; i -= 1) {
	    if (! ((v >> i) & 1)) break ;
	} /* end for */
	return (i >= 0) ? i : -1 ;
}
/* end subroutine (flbci) */

int flbcl(ulong v) noex {
	const int	n = (8*sizeof(ulong)) ;
	int		i ;
	for (i = (n - 1) ; i >= 0 ; i -= 1) {
	    if (! ((v >> i) & 1)) break ;
	} /* end for */
	return (i >= 0) ? i : -1 ;
}
/* end subroutine (flbcl) */

int flbcll(ulonglong v) noex {
	const int	n = (8*sizeof(ulonglong)) ;
	int		i ;
	for (i = (n - 1) ; i >= 0 ; i -= 1) {
	    if (! ((v >> i) & 1)) break ;
	} /* end for */
	return (i >= 0) ? i : -1 ;
}
/* end subroutine (flbcll) */

int fbscounti(uint v) noex {
	const int	n = (8*sizeof(uint)) ;
	int		i ;
	int		c = 0 ;
	for (i = 0 ; i < n ; i += 1) {
	    if (v & 1) c += 1 ;
	    v = v >> 1 ;
	} /* end for */
	return c ;
}
/* end subroutine (fbscounti) */

int fbscountl(ulong v) noex {
	const int	n = (8*sizeof(ulong)) ;
	int		i ;
	int		c = 0 ;
	for (i = 0 ; i < n ; i += 1) {
	    if (v & 1) c += 1 ;
	    v = v >> 1 ;
	} /* end for */
	return c ;
}
/* end subroutine (fbscountl) */

int fbscountll(ulonglong v) noex {
	const int	n = (8*sizeof(ulonglong)) ;
	int		i ;
	int		c = 0 ;
	for (i = 0 ; i < n ; i += 1) {
	    if (v & 1) c += 1 ;
	    v = v >> 1 ;
	} /* end for */
	return c ;
}
/* end subroutine (fbscountll) */


