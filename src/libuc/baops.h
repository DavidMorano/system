/* baops HEADER */
/* lang=C20 */

/* Bit-Array Operations (BAO) */
/* last modifided %G% version %I% */


/* revision history:

	= 1998-08-27, David A­D­ Morano
	These macros were written from scratch but inspired by
	single instructions that did the same sort of thing on the
	VAX (from the late 1970s and early 1980s)!  The VAX came
	out in 1978.  For the record, the Mototola 680XX came out
	in 1979.  And it (the Motorola 680XX) also had bit-set,
	bit-clr, and bit-test instructions (which I used extensively
	back in the early 1980s). As an aside, I loved the VAX in
	its day (did a lot of assembly on it), and I loved the
	MC680XX back in its day (did tons of assembly in it).  Those
	were the days!  Those processors were hard to beat.  I have
	to almost laugh a bit here. Because the C-language (love
	it to pieces as we do) only really came into its own big-time
	pretty much after those assembly days. Although the C-language
	was invented in 1972, it did not really take off until after
	the book on it (by Kernighan and Ritchie) came out in 1978.

	= 2023-11-05, David A­D­ Morano
	I moved the subroutines that used to be located in the
	corresponding source-language support file to here (this
	file below) as inline subroutines. In practice, I think for
	performance reasons, the subroutine versions of these were
	hardly ever used.  Rather the macros were used directly.
	This was a practice (possibly misguided) from way back (some
	many decades now). But with the introduction of 'inline'
	subroutines (we will use the introductrion of it in C-lang
	with C99 for our example), some effort has been made to use
	those inline versions (yes, but only after over three
	decades now).

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

#ifndef	BAOPS_INCLUDE
#define	BAOPS_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<stdintx.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* on char(acter) (byte) organized bit arrays */

#define	BASET(array,bn)		((array)[(bn) >> 3] |= (1 << ((bn) & 7)))
#define	BACLR(array,bn)		((array)[(bn) >> 3] &= (~ (1 << ((bn) & 7))))
#define	BATST(array,bn)		(((array)[(bn) >> 3] >> ((bn) & 7)) & 1)

#define	BASETB(array,bn)	BASET((array),(bn))
#define	BACLRB(array,bn)	BACLR((array),(bn))
#define	BATSTB(array,bn)	BATST((array),(bn))

/* on short-word (16-bit) organized bit arrays */

#define	BASETS(array,bn)	((array)[(bn) >> 4] |= (1 << ((bn) & 15)))
#define	BACLRS(array,bn)	((array)[(bn) >> 4] &= (~ (1 << ((bn) & 15))))
#define	BATSTS(array,bn)	(((array)[(bn) >> 4] >> ((bn) & 15)) & 1)

/* on integers (currently 32-bit) */

#define	BASETI(array,bn)	((array)[(bn) >> 5] |= (1 << ((bn) & 31)))
#define	BACLRI(array,bn)	((array)[(bn) >> 5] &= (~ (1 << ((bn) & 31))))
#define	BATSTI(array,bn)	(((array)[(bn) >> 5] >> ((bn) & 31)) & 1)

/* on 64-bit longs */

#define	BASETL(array,bn) \
	((array)[(bn) >> 6] |= (1L << ((bn) & 63)))
#define	BACLRL(array,bn) \
	((array)[(bn) >> 6] &= (~ (1L << ((bn) & 63))))
#define	BATSTL(array,bn) \
	(((array)[(bn) >> 6] >> ((bn) & 63)) & 1L)

/* on 128-bit longlongs */

#define	BASETLL(array,bn) \
	((array)[(bn) >> 7] |= (1LL << ((bn) & 127)))
#define	BACLRLL(array,bn) \
	((array)[(bn) >> 7] &= (~ (1LL << ((bn) & 127))))
#define	BATSTLL(array,bn) \
	(((array)[(bn) >> 7] >> ((bn) & 127)) & 1LL)

/* on 256-bit intmax (IM) */

#define	BASETIM(array,bn) \
	((array)[(bn) >> 7] |= (1LL << ((bn) & 255)))
#define	BACLRIM(array,bn) \
	((array)[(bn) >> 7] &= (~ (1LL << ((bn) & 255))))
#define	BATSTIM(array,bn) \
	(((array)[(bn) >> 7] >> ((bn) & 255)) & 1LL)

#ifdef	__cplusplus

template<typename T>
inline constexpr bool baset(T *ap,int b) noex {
	return BASET(ap,b) ;
}
template<typename T>
inline constexpr bool baclr(T *ap,int b) noex {
	return BACLR(ap,b) ;
}
template<typename T>
inline constexpr bool batst(const T *ap,int b) noex {
	return BATST(ap,b) ;
}

template<>
inline constexpr bool baset(uchar *ap,int b) noex {
	return BASETB(ap,b) ;
}
template<>
inline constexpr bool baclr(uchar *ap,int b) noex {
	return BACLRB(ap,b) ;
}
template<>
inline constexpr bool batst(const uchar *ap,int b) noex {
	return BATSTB(ap,b) ;
}

template<>
inline constexpr bool baset(ushort *ap,int b) noex {
	return BASETS(ap,b) ;
}
template<>
inline constexpr bool baclr(ushort *ap,int b) noex {
	return BACLRS(ap,b) ;
}
template<>
inline constexpr bool batst(const ushort *ap,int b) noex {
	return BATSTS(ap,b) ;
}

template<>
inline constexpr bool baset(uint *ap,int b) noex {
	return BASETI(ap,b) ;
}
template<>
inline constexpr bool baclr(uint *ap,int b) noex {
	return BACLRI(ap,b) ;
}
template<>
inline constexpr bool batst(const uint *ap,int b) noex {
	return BATSTI(ap,b) ;
}

template<>
inline constexpr bool baset(ulong *ap,int b) noex {
	return BASETL(ap,b) ;
}
template<>
inline constexpr bool baclr(ulong *ap,int b) noex {
	return BACLRL(ap,b) ;
}
template<>
inline constexpr bool batst(const ulong *ap,int b) noex {
	return BATSTL(ap,b) ;
}

template<>
inline constexpr bool baset(ulonglong *ap,int b) noex {
	return BASETLL(ap,b) ;
}
template<>
inline constexpr bool baclr(ulonglong *ap,int b) noex {
	return BACLRLL(ap,b) ;
}
template<>
inline constexpr bool batst(const ulonglong *ap,int b) noex {
	return BATSTLL(ap,b) ;
}

inline constexpr bool basetb(uchar *ap,int b) noex {
	return BASETB(ap,b) ;
}
inline constexpr bool baclrb(uchar *ap,int b) noex {
	return BACLRB(ap,b) ;
}
inline constexpr bool batstb(const uchar *ap,int b) noex {
	return BATSTB(ap,b) ;
}

inline constexpr bool basets(ushort *ap,int b) noex {
	return BASETS(ap,b) ;
}
inline constexpr bool baclrs(ushort *ap,int b) noex {
	return BACLRS(ap,b) ;
}
inline constexpr bool batsts(const ushort *ap,int b) noex {
	return BATSTS(ap,b) ;
}

inline constexpr bool baseti(uint *ap,int b) noex {
	return BASETI(ap,b) ;
}
inline constexpr bool baclri(uint *ap,int b) noex {
	return BACLRI(ap,b) ;
}
inline constexpr bool batsti(const uint *ap,int b) noex {
	return BATSTI(ap,b) ;
}

inline constexpr bool basetl(ulong *ap,int b) noex {
	return BASETL(ap,b) ;
}
inline constexpr bool baclrl(ulong *ap,int b) noex {
	return BACLRL(ap,b) ;
}
inline constexpr bool batstl(const ulong *ap,int b) noex {
	return BATSTL(ap,b) ;
}

inline constexpr bool basetll(ulonglong *ap,int b) noex {
	return BASETLL(ap,b) ;
}
inline constexpr bool baclrll(ulonglong *ap,int b) noex {
	return BACLRLL(ap,b) ;
}
inline constexpr bool batstll(const ulonglong *ap,int b) noex {
	return BATSTLL(ap,b) ;
}

#else /* __cplusplus */

static inline bool basetb(uchar *ap,int b) noex {
	return BASETB(ap,b) ;
}
static inline bool baclrb(uchar *ap,int b) noex {
	return BACLRB(ap,b) ;
}
static inline bool batstb(const uchar *ap,int b) noex {
	return BATSTB(ap,b) ;
}

static inline bool basets(ushort *ap,int b) noex {
	return BASETS(ap,b) ;
}
static inline bool baclrs(ushort *ap,int b) noex {
	return BACLRS(ap,b) ;
}
static inline bool batsts(const ushort *ap,int b) noex {
	return BATSTS(ap,b) ;
}

static inline bool baseti(uint *ap,int b) noex {
	return BASETI(ap,b) ;
}
static inline bool baclri(uint *ap,int b) noex {
	return BACLRI(ap,b) ;
}
static inline bool batsti(const uint *ap,int b) noex {
	return BATSTI(ap,b) ;
}

static inline bool basetl(ulong *ap,int b) noex {
	return BASETL(ap,b) ;
}
static inline bool baclrl(ulong *ap,int b) noex {
	return BACLRL(ap,b) ;
}
static inline bool batstl(const ulong *ap,int b) noex {
	return BATSTL(ap,b) ;
}

static inline bool basetll(ulonglong *ap,int b) noex {
	return BASETLL(ap,b) ;
}
static inline bool baclrll(ulonglong *ap,int b) noex {
	return BACLRLL(ap,b) ;
}
static inline bool batstll(const ulonglong *ap,int b) noex {
	return BATSTLL(ap,b) ;
}

#endif /* __cplusplus */


#endif /* BAOPS_INCLUDE */


