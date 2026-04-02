/* strtox SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* conversion of a decimal c-string to the type |longlong| */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debuging */

/* revision history:

	= 1998-09-03, David A­D­ Morano
	This code was written to provide string-to-integer conversions
	in the style (similar function signatures and semantics)
	of the existing UNIX® standard-C library subroutines like
	|strtol(3c)| and |strtoul(3c)|.  But the idea was to provide
	subroutines that can perform 128-bit (and larger) conversions
	for the types |{x}longlong| (signed and unsigned).  This
	code is partially modeled after some very old previous
	subroutines that I wrote first in assembly and then in C
	language.  See my description below for more on why I wrote
	these subroutines here.

	= 2020-05-24, David A­D­ Morano
	I added some code to make constant-expression compile-time
	computations.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	strto{x}

	Names:
	strtoxi
	strtoxl
	strtoxll
	strtoxui
	strtoxul
	strtoxull

	Synopsis:
	{x} strtox{x}(cchar *sp,char **endp,int base) noex

	Arguments:
	{x}		one of: i, l, ll, ui, ul, ull
	sp		c-string to convert
	endp		returned pointer last character considered
	base		base to convert, 2-36

	Returns:
	typeof({x})	resulting value in destired integer type
	errno		set on error (UNIX® error number)

	Description:
	This code converts a c-string of decimal digits into the
	integer types |longlong| and |ulonglong|.  The API and
	semantics of this code is intentionally modeled after the
	API and semantics of the existing UNIX® conversion subroutines
	|strtol(3c)| and |strtoul(3c)|.  See the notes below for
	more information on the necessity for these.

	Acknowledgements:
	1. Thanks are due to P.J. Plauger for his origial creation
	of the |strtol(3c)| subroutine.  Everyone has copied him
	(to some lesser or greater extent) ever since.
	2. Secondary thanks are also due to Ken Thompson for his
	original creation of the |atoi(3c)| subroutine (even predating
	the creation of the C programming language itself).  Although
	not so much used today, it was the inspiration for Plauger
	to create the |strtol(3c)| subroutine.
	3. Further thanks are due to Gwynne "Gwyn" Morgan (of BSD
	fame) for possibly being the inventor of the specific
	'cutoff-curlim' overflow-prevntion algorithm (which, as you
	know, is widely used now-a-days).

	Notes:
	1. This code is limited (by the coding used) to a maximum
	base of 36.  This is the standard for the standard library
	subroutines.

	Questions-Answers:

	Q. Why are you using these old weirdo UNIX® standard
	C-language library subroutine function signatures?

	A. Because before the CTX subroutines (in the LIBUC) lobrary
	are available, I need some local subroutines (here within
	the LIBU library) to perform some of these conversions.
	Although |strtol(3c)| and |strtoul(3c)| are generally
	available across all UNIXi implementations (and are called
	directly below for much of the provided interfaces), the
	standard C-language library does not supply any of: |strtoi|,
	|strtoui|, |strtoll|, |strtoull|, |strtoim| or |strtouim|.
	I leverage the fact that |strtol| and |strtoul| were already
	written to provide conversion for types smaller than a
	|long| and |ulong|.  But for the integers sized larger than
	that (128-bit and larger), I had to write compatible (similar
	function signature and semantic) subroutines from scratch,
	since there are no simple standard lirbary calls that are
	avilable in regular C-language that could be used.

	Q. Are subroutine converters availble now for 256-bit and
	512-bit integers?

	A. No.  I kind-of think they are not needed here (in LIBU)
	because there are currently no required use cases for
	integers larger than 128-bit.  Larger integers are used
	after LIBUC is loaded and there the CTX and CFX subroutines
	provide for the larger integer type conversions (also up
	to 128-bit and beyond also).

	Q. Why are the names of these subroutine prefixed with |strtox|
	rather than simply |strto| like the standard C-language
	subroutine are?  
	
	A. Because the standard C-language library aready provdes
	subroutines named |strtoll| and |strtoull|, but they do the
	wrong thing.  The 'll' postfix in the subroutine name
	indicates (or is supposed to indicate) that the 'long long'
	type is being used.  But the 'long long' type on most systems
	today is still only 64 bits, where I wanted the 'll' postfix
	to mean conversion to a 128-bit integer.  Further, I want
	the postfix 'im' or 'imax' to indicate a conversion to a
	256-bit integer (not yet implemented).  I have implemented
	the 'im' or 'imax' variations elsewhere, now hidden inside
	a company that will not be named but does have the initials
	"Hewlett Package Enterprise."

	Q. What is the basic algorithm used below for conversions 
	(signed and unsigned)?

	A. I use the "cutoff-curlim" algorithm; the same is used
	almost in all (known) UNIXi (or POSIXi) uses.

	Q. Do you optimize even-power-of-two bases for faster
	conversions.

	A. No.  Often (usually), the fastest algorithms (for anything)
	is not in this library (LIBU) but rather in higher-level
	libraries (LIBUC and higher).

	Q. Do you provide conversions for integer types smaller than
	a 32-bit integer?

	A. No.  Use a conversion foe a 32-bit integer and cast the
	result as you like.

	Q. What number bases are supported?

	A. Bases 2 through 36 are supported (the old standard).
	For other weirdo number bases (strange creatures not
	elaborated on further here), or for more normal number bases
	from 2 through 128, go check out my CFX family of subroutines.

	Q. Have you written enough of these "number conversion"
	functions?

	A. Apparently not.  It does seem like writing these are a
	major part of getting onto a new platform.  Especially when
	you have total bare metal and you need to write a new ROM
	monitor.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |strtol(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>
#include	<stdintx.h>
#include	<localmisc.h>
#include	<dprintf.hh>		/* debugging */

#include	"strtox.h"

import bitmanip ;			/* LIBU bit-manipulations */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	1		/* debuging */
#endif


/* imported namespaces */

using libu::getsign ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr int	maxbase = 36 ;		/* must be classic value */
constexpr int   chtablen = (UCHAR_MAX + 1) ;

namespace {
    struct llhelper {
	ulonglong	ullmax = 0 ;
	longlong	llmin = 0 ;
	longlong	llmax = 0 ;
	ulonglong	cutoff	[maxbase+1] = {} ;
	longlong	pcutoff	[maxbase+1] = {} ;
	longlong	ncutoff	[maxbase+1] = {} ;
	int		cutlim	[maxbase+1] = {} ;
	int		pcutlim	[maxbase+1] = {} ;
	int		ncutlim	[maxbase+1] = {} ;
	consteval void mklims() noex {
	    clonglong	one = 1 ;
	    cint	nb = nbits<longlong> ;
	    ullmax = (compl ullmax) ;
	    llmin = (one << (nb - 1)) ;
	    llmax = longlong(ullmax >> 1) ;
	} ; /* end method (mklims) */
	consteval void mkcuts() noex {
	    for (uint b = 2 ; b <= uint(maxbase) ; b += 1) {
		cutoff[b] = (ullmax / b) ;
		cutlim[b] = int(ullmax % b) ;
	    } /* end for */
	} ; /* end method (mkcuts) */
	consteval void mkpos() noex {
	    for (uint b = 2 ; b <= uint(maxbase) ; b += 1) {
                pcutoff[b] = (llmax / b) ;
                pcutlim[b] = cutlim[b] ;
	    } /* end for */
	} ; /* end method (mkpos) */
	consteval void mkneg() noex {
	    longlong	nco ;
	    int		ncl ;
	    for (uint b = 2 ; b <= uint(maxbase) ; b += 1) {
                nco = llmin / b ;
                ncl = cutlim[b] ;
                if (ncl > 0) {
                    ncl -= b ;
                    nco += 1 ;
                }
                ncl = (- ncl) ;
                ncutoff[b] = nco ;
                ncutlim[b] = ncl ;
	    } /* end for */
	} ; /* end method (mkneg) */
	consteval llhelper() noex {
	    mklims() ;
	    mkcuts() ;
	    mkpos() ;
	    mkneg() ;
	} ; /* end ctor */
	constexpr longlong	getcutoff(int b,bool f) const noex {
	    return (f) ? ncutoff[b] : pcutoff[b] ;
	} ; /* end method (getcutoff) */
	constexpr int		getcutlim(int b,bool f) const noex {
	    return (f) ? ncutlim[b] : pcutlim[b] ;
	} ; /* end method (getcutlim) */
    } ; /* end subroutine (llhelper) */
} /* end namespace */

namespace {
    struct basepairs {
	uchar	ch ;
	uchar	num ;
    } ; /* end struct */
} /* end namespace */

constexpr basepairs	pairs[] = {
	{ 'B', 2 },
	{ 'O', 8 },
	{ 'D', 10 },
	{ 'X', 16 },
	{ 0, 0 }
} ; /* end array */

namespace {
    struct charbase {
	uchar		num[chtablen] = {} ;
	uchar		val[chtablen] ;
	consteval void mknums() noex {
	    for (int ch, i = 0 ; ((ch = pairs[i].ch)) ; ++i) {
		num[ch] = pairs[i].num ;
		num[ch + 0x20] = pairs[i].num ; /* case conversion: U -> L */
	    } /* end for */
	} ; /* end ctor (mknums) */
	consteval void mkvals() noex {
	    for (int i = 0 ; i < chtablen ; i += 1) {
	        if ((i >= '0') && (i <= '9')) {
		    val[i] = uchar(i - '0' + 0) ;
		} else if ((i >= 'A') && (i <= 'Z')) {
		    val[i] = uchar(i - 'A' + 10) ;
		} else if ((i >= 'a') && (i <= 'z')) {
		    val[i] = uchar(i - 'a' + 10) ;
		} else {
		    val[i] = uchar(UCHAR_MAX) ;
		}
	    } /* end for */
	} ; /* end method (mkvals) */
	consteval charbase() noex {
	    mknums() ;
	    mkvals() ;
	} ; /* end ctor */
    } ; /* end struct (charbase) */
} /* end namespace */

namespace {
    struct strer {
	cchar	*startp ;
	cchar	*sp ;
	char	**endpp ;
	int	base ;
	int	verr = 0 ;
	int	cutlim ;
	bool	fneg{} ;
	strer(cc *s,char **e,int b) noex : startp(s), endpp(e), base(b) { 
	    sp = startp ;
	} ;
	void signer() noex {
	    if (cint si = getsign(sp,-1,&fneg) ; si > 0) {
		sp += si ;
	    } /* end if (getsign) */
	} ; /* end method */
	void baser() noex ;
	void cooker() noex ;
	void ender() noex {
            if (endpp) {
                *endpp = charp(verr ? sp : startp) ;
            }
	} ; /* end method */
	virtual void cookprep() noex = 0 ;
	virtual void cvt(int) noex = 0 ;
    } ; /* end struct */
} /* end namespace */

namespace {
    struct strer_sig : strer {
	longlong	res{} ;
	longlong	cutoff ;
	strer_sig(cc *s,char **e,int b) noex : strer(s,e,b) { } ;
	operator longlong ()	noex ;
	void cookprep()		noex override final ;
	void cvt(int)		noex override final ;
	void cvtpos(int)	noex ;
	void cvtneg(int)	noex ;
    } ; /* end struct */
    struct strer_uns : strer {
	ulonglong	ures{} ;
	ulonglong	cutoff ;
	strer_uns(cc *s,char **e,int b) noex : strer(s,e,b) { } ;
	operator ulonglong ()	noex ;
	void cookprep()		noex override final ;
	void cvt(int)		noex override final ;
    } ; /* end struct */
} /* end namespace */


/* forward references */

template<typename T>
local inline void strtox(cchar *,char **,int,T *rp) noex {
	*rp = 0 ;
}

template<>
inline void strtox(cchar *sp,char **epp,int b,int *rp) noex {
	clong	v = strtol(sp,epp,b) ;
	*rp = int(v) ;
	if (errno == 0) {
	    ulong	uv = ulong(v) ;
	    cint	nb = nbits<long> ;
	    {
	        cbool	fneg = bit(v,(nb - 1)) ;
		if (fneg) {	/* test negative value */
		    uv = (compl uv) ;
	            uv >>= (nb / 2) ;
		    if (uv || (! bit(v,((nb / 2) - 1)))) {
			*rp = INT_MIN ;
			errno = ERANGE ;
		    }
		} else {	/* test poitive value */
	            uv >>= (nb / 2) ;
		    if (uv || bit(v,((nb / 2) - 1))) {
			*rp = INT_MAX ;
			errno = ERANGE ;
		    }
		} /* end if */
	    } /* end block */
	} /* end if (no error so far) */
} /* end subroutine-template (strtox) */

template<>
inline void strtox(cchar *sp,char **epp,int b,uint *rp) noex {
	ulong	uv = strtoul(sp,epp,b) ;
	*rp = uint(uv) ;
	if (errno == 0) {
	    cint nb = nbits<ulong> ;
	    uv >>= (nb / 2) ;
	    if (uv) {
		errno = ERANGE ;
	    }
	} /* end if (no-error) */
} /* end subroutine-template (strtox) */

local inline int getbase(int) noex attrpure ;
 
local inline bool isbaseval(int b) noex attrconst {
    return ((b >= 2) && (b <= maxbase)) ;
}

template<typename TS>
local bool iserrneg(TS co,int cl,TS res,int val) noex attrconst {
	bool f = false ;
	f = f || (res < co) ;
	f = f || (res == co && val > cl) ;
	return f ;
} /* end subroutine */

template<typename TS>
local bool iserrpos(TS co,int cl,TS res,int val) noex attrconst {
	bool f = false ;
	f = f || (res > co) ;
	f = f || (res == co && val > cl) ;
	return f ;
} /* end subroutine */

template<typename TU>
local bool iserr(TU co,int cl,TU ures,int val) noex attrconst {
	bool f = false ;
	f = f || (ures > co) ;
	f = f || (ures == co && val > cl) ;
	return f ;
} /* end subroutine (iserr) */


/* local variables */

constexpr llhelper	llhelp ;

constexpr charbase	chbase ;

cbool			f_debug = CF_DEBUG ;

/* exported variables */


/* exported subroutines */

int strtoxi(cchar *sp,char **epp,int b) noex {
	int		res{} ;
	if (sp) {
	    strtox(sp,epp,b,&res) ;
	} else {
	    errno = EFAULT ;
	}
	return res ;
}
/* end subroutine (strtoxi) */

long strtoxl(cchar *sp,char **epp,int b) noex {
    	long		res{} ;
	if (sp) {
	    res = strtol(sp,epp,b) ;
	} else {
	    errno = EFAULT ;
	}
	return res ;
}
/* end subroutine (strtoxl) */

longlong strtoxll(cchar *startp,char **endpp,int base) noex {
    	strer_sig so(startp,endpp,base) ;
	return so ;
} /* end subroutine (strtoxoll) */

uint strtoxui(cchar *sp,char **epp,int b) noex {
	uint		ures{} ;
	if (sp) {
	    strtox(sp,epp,b,&ures) ;
	} else {
	    errno = EFAULT ;
	}
	return ures ;
}
/* end subroutine (strtoxui) */

ulong strtoxul(cchar *sp,char **epp,int b) noex {
    	ulong		ures{} ;
	if (sp) {
	    ures = strtoul(sp,epp,b) ;
	} else {
	    errno = EFAULT ;
	}
	return ures ;
}
/* end subroutine (strtouxl) */

ulonglong strtoxull(cchar *startp,char **endpp,int base) noex {
    	strer_uns so(startp,endpp,base) ;
	return so ;
} /* end subroutine (strtoxoll) */


/* local subroutines */

local inline int getbase(int ch) noex attrpure {
	return chbase.num[ch & UCHAR_MAX] ;
} /* end subroutine (getbase) */
 
void strer::baser() noex {
    	int ch = *sp ;
	if ((base == 0 || base == 16) && ch == '0') {
	    if (cint b = getbase(sp[1]) ; b > 0) {
		base = b ;
		sp += 2 ;
		ch = *sp ;
	    } /* end if (getbase) */
	} /* end if */
	if (base == 0) {
	    base = ((ch == '0') ? 8 : 10) ;
	}
} /* end method (strer::baser) */

void strer::cooker() noex {
	if (isbaseval(base)) {
	    cookprep() ;
	    for (int ch ; ((ch = uchar(*sp))) ; sp += 1) {
		cint val = chbase.val[ch] ;
                if (val >= base) break ;
                if (verr < 0) continue ;
		cvt(val) ;
            } /* end for */
	} else {
	    errno = ENOTSUP ;
	} /* end if (valid base) */
} /* end method (strer::cooker) */

strer_sig::operator longlong () noex {
    	if (startp) {
    	    signer() ;
	    baser() ;
	    cooker() ;
	    ender() ;
	} else {
	    errno = EFAULT ;
	}
    	return res ;
} /* end method (strer_sig::operator) */

void strer_sig::cookprep() noex {
	cutoff = llhelp.getcutoff(base,fneg) ;
	cutlim = llhelp.getcutlim(base,fneg) ;
}

void strer_sig::cvt(int val) noex {
	if (fneg) {
	    cvtneg(val) ;
	} else {
	    cvtpos(val) ;
	}
} /* end method */

void strer_sig::cvtneg(int val) noex {
	if (iserrneg(cutoff,cutlim,res,val)) {
	    verr = -1 ;
	    res = llhelp.llmin ;
	    errno = ERANGE ;
	} else {
	    verr = +1 ;
	    res *= base ;
	    res -= val ;
	} /* end if */
} /* end method */

void strer_sig::cvtpos(int val) noex {
	if (iserrpos(cutoff,cutlim,res,val)) {
	    verr = -1 ;
	    res = llhelp.llmax ;
	    errno = ERANGE ;
	} else {
	    verr = +1 ;
	    res *= base ;
	    res += val ;
	} /* end if */
} /* end method */

strer_uns::operator ulonglong () noex {
    	if (startp) {
    	    signer() ;
	    baser() ;
	    cooker() ;
	    if (fneg && verr > 0) {
	        ures = (- ures) ;
	    }
	    ender() ;
	} else {
	    errno = EFAULT ;
	}
    	return ures ;
} /* end method */

void strer_uns::cookprep() noex {
	cutoff = llhelp.cutoff[base] ;
	cutlim = llhelp.cutlim[base] ;
}

void strer_uns::cvt(int val) noex {
	if (iserr(cutoff,cutlim,ures,val)) {
	    verr = -1 ;
	    ures = llhelp.ullmax ;
	    errno = ERANGE ;
	} else {
	    verr = +1 ;
	    ures *= base ;
	    ures += val ;
	} /* end if */
} /* end method */


