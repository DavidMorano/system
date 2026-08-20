/* strtox SUPPORT (String-To-X-integer) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a c-string of various number bases to an integer type */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

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
	strtoxsi
	strtoxsl
	strtoxsll
	strtoxui
	strtoxul
	strtoxull

	Synopsis:
	{x} strtox{x}(cchar *sp,char **endp,int base) noex

	Arguments:
	{x}		one of: i, l, ll, ui, ul, ull
	sp		c-string to convert
	endp		returned pointer to last character considered
	base		base to convert, 2-36

	Returns:
	typeof({x})	resulting value in desired integer type
	errno		set on error (UNIX® error number)

	Description:
	This code converts a c-string of digit characters (of a
	given numeric base) into the integer types |slonglong| or
	|ulonglong|.  The API and semantics of this code is
	intentionally modeled after the API and semantics of the
	existing UNIX® standard C-language library conversion
	subroutines |strtol(3c)| and |strtoul(3c)|.  See the notes
	below for more information on the necessity for these.

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
	'cutoff-cutlim' overflow-prevention algorithm (which, as you
	know, is widely used now-a-days).

	Notes:
	1. This code is limited (for standards compliance) to a
	maximum base of 36.  This is the standard for the standard
	C-language library subroutines.  Digit character conversions
	to integers for bases up to 128 are available elsewhere in
	my code bases.
	2. When the standard C library subroutine |strtol(3c)|
	is given a NULL string, it seg-faults.  So I here below,
	check for that (a NULL pointer).  But at least when the 
	|strtol(3c)| subroutine is given an empty string, it 
	returns an EINVAL (as desired).

	Questions-Answers:

	Q. Why are you using these old weirdo UNIX® standard
	C-language library subroutine function signatures?

	A. Because before the CFX subroutines (in the LIBUC) library
	are available (in a code load sequence), I need some local
	subroutines (here within the LIBU library) to perform some
	of these conversions.  Although |strtol(3c)| and |strtoul(3c)|
	are generally available across all UNIXi implementations
	(and are called directly below for much of the provided
	interfaces), the standard C-language library does not supply
	any of: |strtoi|, |strtoui|, |strtoll|, |strtoull|, |strtoim|
	or |strtouim|.  I leverage the fact that |strtol| and
	|strtoul| were already written to provide conversions for
	types equal in size or smaller than a |long| and |ulong|.
	But for the integers sized larger than that (128-bit and
	larger), I had to write compatible (similar function signature
	and semantic) subroutines from scratch, since there are no
	simple standard lirbary calls that are avilable in regular
	C-language that could be used.

	Q. Are subroutine converters availble now for 256-bit and
	512-bit integers?

	A. No.  I kind-of think they are not needed here (in LIBU)
	because there are currently no required use cases for
	integers larger than 128-bit.  Larger integers are used
	after LIBUC is loaded and there the CTX and CFX subroutines
	provide for the larger integer type conversions (also up
	to 128-bit and beyond also).

	Q. Why are the names of these subroutines prefixed with
	|strtox| rather than simply |strto| like the standard
	C-language subroutine are?

	A. Because the standard C-language library already provides
	subroutines named |strtoll| and |strtoull|, but they do the
	wrong thing.  The 'll' postfix in the subroutine name
	indicates (or is supposed to indicate) that the 'long long'
	type is being used.  But the 'long long' type on most systems
	today is still only 64 bits, where I wanted the 'll' postfix
	to mean conversion to a 128-bit integer.  Further, I want
	the postfix 'im' or 'imax' to indicate a conversion to a
	256-bit integer (not yet implemented).  I have implemented
	the 'im' or 'imax' variations elsewhere.

	Q. What is the basic algorithm used below for conversions 
	(signed and unsigned)?

	A. I use the "cutoff-cutlim" algorithm; the same is used
	in most (perhaps all) known current UNIXi (or POSIXi)
	implementations.

	Q. Do you optimize even-power-of-two bases for faster
	conversions.

	A. No.  Often (usually), the fastest algorithms (for anything)
	are not in this library (LIBU) but rather in higher-level
	libraries (LIBUC and higher).

	Q. Do you provide conversions for integer types smaller than
	a 32-bit integer?

	A. No.  Use a conversion for a 32-bit integer and cast the
	result as you like.

	Q. What number bases are supported?

	A. Bases 2 through 36 are supported (the old standard).
	For other weirdo number bases (strange creatures not
	elaborated on further here), or for more normal number bases
	from 2 through 128, go check out my CFX family of subroutines.

	Q. Have you written enough of these "number conversion"
	functions yet?

	A. Apparently not.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |strtol(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |getsign(3u)| */
#include	<stdintx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU debugging */

#include	"strtox.h"

import bitmanip ;			/* LIBU bit-manipulations */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
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
		cutlim[b] = conv<int>(ullmax % b) ;
	    } /* end for */
	} ; /* end method (mkcuts) */
	consteval void mkpos() noex {
	    for (uint b = 2 ; b <= uint(maxbase) ; b += 1) {
                pcutoff[b] = (llmax / b) ;
                pcutlim[b] = conv<int>(llmax % b) ;
	    } /* end for */
	} ; /* end method (mkpos) */
	consteval void mkneg() noex {
	    longlong	nco ;
	    int		ncl ;
	    for (uint b = 2 ; b <= uint(maxbase) ; b += 1) {
                nco = (llmin / b) ;
                ncl = conv<int>(llmin % b) ;
                if (ncl > 0) {
                    ncl -= b ;
                    nco += 1 ;
                }
                ncl = (neg ncl) ;
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
	} ; /* end ctor */
	void suber() noex ;
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
	virtual void cookprep()	noex = 0 ;
	virtual void cvt(int)	noex = 0 ;
	virtual void negator()	noex { } ;
	virtual void reterr()	noex = 0 ;
    } ; /* end struct */
} /* end namespace */

namespace {
    struct strer_sll : strer { /* "signed-longlong" */
	longlong	res{} ;
	longlong	cutoff ;
	strer_sll(cc *s,char **e,int b) noex : strer(s,e,b) { } ;
	operator longlong ()	noex ;
	void cookprep()		noex override final ;
	void cvt(int)		noex override final ;
	void cvtpos(int)	noex ;
	void cvtneg(int)	noex ;
	void reterr()		noex override final ;
    } ; /* end struct */
    struct strer_ull : strer { /* "unsigned-longlong" */
	ulonglong	ures{} ;
	ulonglong	cutoff ;
	strer_ull(cc *s,char **e,int b) noex : strer(s,e,b) { } ;
	operator ulonglong ()	noex ;
	void cookprep()		noex override final ;
	void cvt(int)		noex override final ;
	void negator()		noex override final ;
	void reterr()		noex override final ;
    } ; /* end struct */
} /* end namespace */


/* forward references */

template<typename T>
local inline void strtox(cchar *,char **,int,T *rp) noex {
	*rp = 0 ;
} /* end subroutine-template (strtox) */

template<>
inline void strtox(cchar *sp,char **epp,int b,int *rp) noex {
	cint		nb = nbits<long> ;
	int		res = 0 ;
	errno_t		ec = 0 ;
	bool		fneg ;
	if (sp) ylikely {
	    clong	v = strtol(sp,epp,b) ;
	    fneg = bit(v,(nb - 1)) ;
	    if (errno == 0) {
	        ulong	uv = conv<ulong>(v) ;
	        res = conv<int>(v) ;
		if (fneg) {	/* test negative value */
		    uv = (compl uv) ;
	            uv >>= (nb / 2) ;
		    if (uv || (! bit(v,((nb / 2) - 1)))) {
			res = INT_MIN ;
			ec = ERANGE ;
		    }
		} else {	/* test poitive value */
	            uv >>= (nb / 2) ;
		    if (uv || bit(v,((nb / 2) - 1))) {
			res = INT_MAX ;
			ec = ERANGE ;
		    }
		} /* end if */
	    } else {
                if (fneg) {
                    res = UINT_MIN ;
                } else {
                    res = UINT_MAX ;
                }
	    } /* end if (no error so far) */
	} else {
	    ec = EFAULT ;
	}
	if (ec) errno = ec ;
	*rp = res ;
} /* end subroutine-template (strtox) */

template<>
inline void strtox(cchar *sp,char **epp,int b,uint *rp) noex {
	uint		res = 0 ;
	errno_t		ec = 0 ;
	if (sp) ylikely {
	    ulong	uv = strtoul(sp,epp,b) ;
	    if (errno == 0) {
	        cint nb = nbits<ulong> ;
	        res = conv<uint>(uv) ;
	        uv >>= (nb / 2) ;
	        if (uv) {
		    res = UINT_MAX ;
		    ec = ERANGE ;
	        }
	    } else {
	        res = UINT_MAX ;
	    } /* end if (no-error) */
	} else {
	    ec = EFAULT ;
	} /* end if */
	if (ec) errno = ec ;
	*rp = res ;
} /* end subroutine-template (strtox) */

local inline int getbase(int) noex attrpure ;
 
local inline bool isbaseval(int b) noex attrconst {
    return ((b >= 2) && (b <= maxbase)) ;
} /* end subroutine (isbaseval) */

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
constexpr int		base8	= 8 ;		/* base-8 */
constexpr int		base10	= 10 ;		/* base-10 */
constexpr int		base16	= 16 ;		/* base-16 */
constexpr bool		f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

sint		strtoxsi(cchar *sp,char **epp,int b) noex {
	sint		res{} ;
	if (sp) {
	    strtox(sp,epp,b,&res) ;
	} else {
	    errno = EFAULT ;
	}
	return res ;
} /* end subroutine (strtoxsi) */

slong		strtoxsl(cchar *sp,char **epp,int b) noex {
    	slong		res{} ;
	if (sp) {
	    res = strtol(sp,epp,b) ;
	} else {
	    errno = EFAULT ;
	}
	return res ;
} /* end subroutine (strtoxsl) */

slonglong	strtoxsll(cchar *sp,char **epp,int b) noex {
    	strer_sll so(sp,epp,b) ;
	return so ;
} /* end subroutine (strtoxsll) */

uint		strtoxui(cchar *sp,char **epp,int b) noex {
	uint		ures{} ;
	if (sp) {
	    strtox(sp,epp,b,&ures) ;
	} else {
	    errno = EFAULT ;
	}
	return ures ;
} /* end subroutine (strtoxui) */

ulong		strtoxul(cchar *sp,char **epp,int b) noex {
    	ulong		ures{} ;
	if (sp) {
	    ures = strtoul(sp,epp,b) ;
	} else {
	    errno = EFAULT ;
	}
	return ures ;
} /* end subroutine (strtoxul) */

ulonglong	strtoxull(cchar *sp,char **epp,int b) noex {
    	strer_ull so(sp,epp,b) ;
	return so ;
} /* end subroutine (strtoxull) */


/* local subroutines */

local inline int getbase(int ch) noex attrpure {
	return chbase.num[ch & UCHAR_MAX] ;
} /* end subroutine (getbase) */
 
void strer::suber() noex {
    	if (startp) {
    	    signer() ;
	    baser() ;
	    cooker() ;
	    negator() ;
	    ender() ;
	    reterr() ;
	} else {
	    errno = EFAULT ;
	}
} /* end method (strer::suber) */

void strer::baser() noex {
    	int ch = *sp ;
	if ((ch == '0') && (base == 0 || base == base16)) {
	    if (cint b = getbase(sp[1]) ; b > 0) {
		base = b ;
		sp += 2 ; /* advance two characters */
		ch = *sp ;
	    } /* end if (getbase) */
	} /* end if */
	if (base == 0) {
	    base = ((ch == '0') ? base8 : base10) ;
	}
} /* end method (strer::baser) */

local inline int getval(int b,cchar *sp) noex {
    	int val = -1 ;
	if (cint ch = uchar(*sp) ; ch > 0) {
	    if (cint v = chbase.val[ch] ; v < b) {
		val = v ;
	    }
	} /* end if */
	return val ;
} /* end subroutine (getval) */

void strer::cooker() noex {
	if (isbaseval(base)) {
	    cookprep() ;
	    for (int val ; (val = getval(base,sp)) >= 0 ; sp += 1) {
                if (verr >= 0) {
		    cvt(val) ;
		}
            } /* end for */
	} else {
	    errno = ENOTSUP ;
	} /* end if (valid base) */
} /* end method (strer::cooker) */

strer_sll::operator longlong () noex {
    	suber() ;
    	return res ;
} /* end method (strer_sll::operator) */

void strer_sll::cookprep() noex {
	cutoff = llhelp.getcutoff(base,fneg) ;
	cutlim = llhelp.getcutlim(base,fneg) ;
} /* end method */

void strer_sll::cvt(int val) noex {
	if (fneg) {
	    cvtneg(val) ;
	} else {
	    cvtpos(val) ;
	}
} /* end method */

void strer_sll::cvtneg(int val) noex {
	if (iserrneg(cutoff,cutlim,res,val)) {
	    verr = -1 ;
	} else {
	    verr = +1 ;
	    res *= base ;
	    res -= val ;
	} /* end if */
} /* end method */

void strer_sll::cvtpos(int val) noex {
	if (iserrpos(cutoff,cutlim,res,val)) {
	    verr = -1 ;
	} else {
	    verr = +1 ;
	    res *= base ;
	    res += val ;
	} /* end if */
} /* end method */

void strer_sll::reterr() noex {
    	if (verr < 0) {
	    res = (fneg) ? llhelp.llmin : llhelp.llmax ;
	    errno = ERANGE ;
	}
} /* end method (strer_sll::reterr) */

strer_ull::operator ulonglong () noex {
    	suber() ;
    	return ures ;
} /* end method */

void strer_ull::cookprep() noex {
	cutoff = llhelp.cutoff[base] ;
	cutlim = llhelp.cutlim[base] ;
} /* end method */

void strer_ull::cvt(int val) noex {
	if (iserr(cutoff,cutlim,ures,val)) {
	    verr = -1 ;
	} else {
	    verr = +1 ;
	    ures *= base ;
	    ures += val ;
	} /* end if */
} /* end method */

void strer_ull::negator() noex {
	if (fneg && verr > 0) {
	    ures = (neg ures) ;
	}
} /* end method (strer_ull::negator) */

void strer_ull::reterr() noex {
    	if (verr < 0) {
	    ures = llhelp.ullmax ;
	    errno = ERANGE ;
	}
} /* end method (strer_ull::reterr) */


