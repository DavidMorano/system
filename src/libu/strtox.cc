/* strtox SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* conversion of a decimal c-sting to the type |longlong| */
/* version %I% last-modified %G% */


/*	$OpenBSD: strtol.c,v 1.7 2005/08/08 08:05:37 espie Exp $ */

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* revision history:

	= 2023-05-14, David A­D­ Morano
	I adapted this to serve the |{x}longlong| types.

*/

/* Copyright © 2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	strto{x}

	Description:
	This code converts a c-string of decimal digits into the
	integer type |longlong|.  It is obviously a derivative work
	of the original |strtol(3c)| from the Berkley BSD edition
	of its UNIX® variant.  Ant locale information is ignored.

	Notes:
	1. This code is limited (by the coding used) to a maximum
	base of 36.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<climits>		/* for |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |strtol(3c)| */
#include	<cctype>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>

#include	"strtox.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */

constexpr uint	maxbase = 36 ;		/* must be classic value */


/* local structures */

struct llhelper {
	longlong	llmin = 0 ;
	longlong	llmax = 0 ;
	ulonglong	ullmax = 0 ;
	ulonglong	cutoff[maxbase+1] = {} ;
	int		cutlim[maxbase+1] = {} ;
	constexpr llhelper() noex {
	    const longlong	one = 1 ;
	    const int		n = (CHAR_BIT * szof(longlong)) ;
	    ullmax = (~ ullmax) ;
	    llmin = (one << (n - 1)) ;
	    llmax = longlong(ullmax >> 1) ;
	    for (uint b = 2 ; b <= maxbase ; b += 1) {
		cutoff[b] = (ullmax / b) ;
		cutlim[b] = (ullmax & b) ;
	    } /* end for */
	} ; /* end constructor */
} ; /* end subroutine (llhelper) */


/* forward references */

template<typename T> constexpr static inline int nbits(T) noex {
    	return (szof(T) * CHAR_BIT) ;
}

template<typename T>
constexpr static inline bool bit(T v,int n) noex {
	return !!((v >> n) & 1) ;
}

template<typename T>
inline void strtox(cchar *,char **,int,T *rp) noex {
	*rp = 0 ;
}

template<>
inline void strtox(cchar *sp,char **epp,int b,int *rp) noex {
	clong	v = strtol(sp,epp,b) ;
	*rp = int(v) ;
	if (errno == 0) {
	    cint	n = nbits(v) ;
	    {
	        cbool	f_neg = bit(v,(n-1)) ;
		if (f_neg) {	/* test negative value */
	    	    ulong	uv = ulong(v) ;
		    uv = (~ uv) ;
	            uv >>= (n/2) ;
		    if (uv || (! bit(v,((n/2)-1)))) {
			errno = ERANGE ;
		    }
		} else {	/* test poitive value */
	    	    ulong	uv = ulong(v) ;
	            uv >>= (n/2) ;
		    if (uv || bit(v,((n/2)-1))) {
			errno = ERANGE ;
		    }
		}
	    } /* end block */
	} /* end if (no error so far) */
}

template<>
inline void strtox(cchar *sp,char **epp,int b,uint *rp) noex {
	ulong	uv = strtoul(sp,epp,b) ;
	*rp = uint(uv) ;
	if (errno == 0) {
	    cint	n = nbits(uv) ;
	    uv >>= (n/2) ;
	    if (uv) {
		errno = ERANGE ;
	    }
	}
}


/* local variables */

constexpr llhelper	llhelp ;


/* exported variables */


/* exported subroutines */

int strtoxi(cchar *sp,char **epp,int b) noex {
    	int	v{} ;
	strtox(sp,epp,b,&v) ;
	return v ;
}
/* end subroutine (uc_strtoi) */

uint strtoxui(cchar *sp,char **epp,int b) noex {
    	uint	uv{} ;
	strtox(sp,epp,b,&uv) ;
	return uv ;
}
/* end subroutine (uc_strtoui) */

long strtoxl(cchar *sp,char **epp,int b) noex {
	return strtol(sp,epp,b) ;
}
/* end subroutine (uc_strtol) */

ulong strtoxul(cchar *sp,char **epp,int b) noex {
    	return strtoul(sp,epp,b) ;
}
/* end subroutine (uc_strtoul) */

longlong strtoxll(cchar *nptr,char **endptr,int base) noex {
	longlong acc, cutoff;
	const char *s;
	int c;
	int neg, vany, cutlim;
	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	s = nptr;
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+') {
			c = *s++;
		}
	}
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0) {
		base = ((c == '0') ? 8 : 10) ;
	}
	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set 'vany' if vany `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? llhelp.llmin : llhelp.llmax ;
	cutlim = cutoff % base;
	cutoff /= base;
	if (neg) {
		if (cutlim > 0) {
			cutlim -= base;
			cutoff += 1;
		}
		cutlim = -cutlim;
	}
	for (acc = 0, vany = 0 ; ; c = (unsigned char) *s++) {
		if (isdigit(c)) {
			c -= '0';
		} else if (isalpha(c)) {
			c -= isupper(c) ? ('A' - 10) : ('a' - 10) ;
		} else {
			break;
		}
		if (c >= base)
			break;
		if (vany < 0)
			continue;
		if (neg) {
			if (acc < cutoff || (acc == cutoff && c > cutlim)) {
				vany = -1;
				acc = llhelp.llmin ;
				errno = ERANGE;
			} else {
				vany = 1;
				acc *= base;
				acc -= c;
			}
		} else {
			if (acc > cutoff || (acc == cutoff && c > cutlim)) {
				vany = -1;
				acc = llhelp.llmax ;
				errno = ERANGE;
			} else {
				vany = 1;
				acc *= base;
				acc += c;
			}
		}
	} /* end for */
	if (endptr != nullptr) {
	    *endptr = (char *) (vany ? (s - 1) : nptr) ;
	}
	return acc ;
}
/* end subroutine (strtoxll) */

/*
 * Convert a string to a |ulonglong|.
 *
 * Ignores 'locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */

ulonglong strtoxull(cchar *nptr, char **endptr, int base) noex {
	ulonglong	acc, cutoff;
	const char *s;
	int c;
	int neg, vany, cutlim;
	/*
	 * See strtoq for comments as to the logic used.
	 */
	s = nptr;
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else { 
		neg = 0;
		if (c == '+') {
			c = *s++;
		}
	}
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1] ;
		s += 2 ;
		base = 16 ;
	}
	if (base == 0) {
		base = ((c == '0') ? 8 : 10) ;
	}

	    cutoff = llhelp.cutoff[base] ;
	    cutlim = llhelp.cutlim[base] ;
	for (acc = 0, vany = 0 ; ; c = (unsigned char) *s++) {
		if (isdigit(c)) {
			c -= '0';
		} else if (isalpha(c)) {
			c -= isupper(c) ? ('A' - 10) : ('a' - 10) ;
		} else
			break;
		if (c >= base)
			break;
		if (vany < 0)
			continue;
		if (acc > cutoff || (acc == cutoff && c > cutlim)) {
			vany = -1;
			acc = llhelp.ullmax ;
			errno = ERANGE;
		} else {
			vany = 1;
			acc *= base ;
			acc += c ;
		}
	} /* end for */
	if (neg && vany > 0) {
	    acc = (-acc) ;
	}
	if (endptr != nullptr) {
	    *endptr = (char *) (vany ? (s - 1) : nptr) ;
	}
	return acc ;
}
/* end subroutine (strtoxull) */


