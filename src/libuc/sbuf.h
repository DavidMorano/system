/* sbuf HEADER */
/* lang=C20 */

/* storage buffer (SBuf) object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SBUF_INCLUDE
#define	SBUF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<stdintx.h>


#define	SBUF		struct sbuf_head


struct sbuf_head {
	char		*rbuf ;		/* result buffer base (constant) */
	int		rlen ;		/* result buffer length (constant) */
	int		index ;		/* current buffer index (changes) */
} ;

#ifdef	__cplusplus
enum sbufmems {
	sbufmem_deci,
	sbufmem_hexc,
	sbufmem_hexi,
	sbufmem_chr,
	sbufmem_blanks,
	sbufmem_rem,
	sbufmem_getlen,
	sbufmem_getprev,
	sbufmem_finish,
	sbufmem_overlast
} ;
struct sbuf_iter {
	cchar		*cp = nullptr ;
	sbuf_iter() = default ;
	sbuf_iter(cchar *p) noex : cp(p) { } ;
	sbuf_iter(const sbuf_iter &oit) noex {
	    if (this != &oit) {
		cp = oit.cp ;
	    }
	} ;
	sbuf_iter &operator = (const sbuf_iter &oit) noex {
	    if (this != &oit) {
		cp = oit.cp ;
	    }
	    return *this ;
	} ;
	bool operator != (const sbuf_iter &) noex ;
	bool operator == (const sbuf_iter &) noex ;
	char operator * () noex {
	    return *cp ;
	} ;
	sbuf_iter operator + (int) const noex ;
	sbuf_iter operator += (int) noex ;
	sbuf_iter operator ++ () noex ; /* pre */
	sbuf_iter operator ++ (int) noex ; /* post */
	void increment(int = 1) noex ;
} ; /* end struct sbuf_iter) */
struct sbuf ;
struct sbuf_co {
	sbuf		*op = nullptr ;
	int		w = -1 ;
	void operator () (sbuf *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 1) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (sbuf_co) */
struct sbuf : sbuf_head {
	sbuf_co		deci ;
	sbuf_co		hexc ;
	sbuf_co		hexi ;
	sbuf_co		chr ;
	sbuf_co		blanks ;
	sbuf_co		rem ;
	sbuf_co		getlen ;
	sbuf_co		getprev ;
	sbuf_co		finish ;
	sbuf() noex {
	    deci(this,sbufmem_deci) ;
	    hexc(this,sbufmem_hexc) ;
	    hexi(this,sbufmem_hexi) ;
	    chr(this,sbufmem_chr) ;
	    blanks(this,sbufmem_blanks) ;
	    rem(this,sbufmem_rem) ;
	    getlen(this,sbufmem_getlen) ;
	    getprev(this,sbufmem_getprev) ;
	    finish(this,sbufmem_finish) ;
	} ;
	sbuf(const sbuf &) = delete ;
	sbuf &operator = (const sbuf &) = delete ;
	int start(char *,int) noex ;
	int nchr(int,int = 1) noex ;
	sbuf_iter begin() noex {
	    sbuf_iter		it(rbuf) ;
	    return it ;
	} ;
	sbuf_iter end() noex {
	    sbuf_iter		it(rbuf+index) ;
	    return it ;
	} ;
	void dtor() noex ;
	~sbuf() noex {
	    dtor() ;
	} ;
} ; /* end struct (sbuf) */
#else	/* __cplusplus */
typedef struct sbuf_head	sbuf ;
#endif /* __cplusplus */

#ifdef	__cplusplus
extern "C" {
#endif

extern int	sbuf_start(sbuf *,char *,int) noex ;
extern int	sbuf_finish(sbuf *) noex ;
extern int	sbuf_buf(sbuf *,cchar *,int) noex ;
extern int	sbuf_strw(sbuf *,cchar *,int) noex ;
extern int	sbuf_strs(sbuf *,int,cchar **) noex ;
extern int	sbuf_deci(sbuf *,int) noex ;
extern int	sbuf_decl(sbuf *,long) noex ;
extern int	sbuf_decll(sbuf *,longlong) noex ;
extern int	sbuf_decui(sbuf *,uint) noex ;
extern int	sbuf_decul(sbuf *,ulong) noex ;
extern int	sbuf_decull(sbuf *,ulonglong) noex ;
extern int	sbuf_hexc(sbuf *,int) noex ;
extern int	sbuf_hexi(sbuf *,int) noex ;
extern int	sbuf_hexl(sbuf *,long) noex ;
extern int	sbuf_hexll(sbuf *,longlong) noex ;
extern int	sbuf_hexuc(sbuf *,uint) noex ;
extern int	sbuf_hexui(sbuf *,uint) noex ;
extern int	sbuf_hexul(sbuf *,ulong) noex ;
extern int	sbuf_hexull(sbuf *,ulonglong) noex ;
extern int	sbuf_char(sbuf *,int) noex ;
extern int	sbuf_nchar(sbuf *,int,int) noex ;
extern int	sbuf_blanks(sbuf *,int) noex ;
extern int	sbuf_adv(sbuf *,int,char **) noex ;
extern int	sbuf_rem(sbuf *) noex ;
extern int	sbuf_getlen(sbuf *) noex ;
extern int	sbuf_getbuf(sbuf *,cchar **) noex ;
extern int	sbuf_getpoint(sbuf *,cchar **) noex ;
extern int	sbuf_getprev(sbuf *) noex ;
extern int	sbuf_printf(sbuf *,cchar *,...) noex ;
extern int	sbuf_termconseq(sbuf *,int,cchar *,int,...) noex ;
extern int	sbuf_addquoted(sbuf *,cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif

#ifdef	__cplusplus

inline int sbuf_dec(sbuf *op,int v) noex {
	return sbuf_deci(op,v) ;
}
inline int sbuf_dec(sbuf *op,long v) noex {
	return sbuf_decl(op,v) ;
}
inline int sbuf_dec(sbuf *op,longlong v) noex {
	return sbuf_decll(op,v) ;
}

inline int sbuf_dec(sbuf *op,uint v) noex {
	return sbuf_decui(op,v) ;
}
inline int sbuf_dec(sbuf *op,ulong v) noex {
	return sbuf_decul(op,v) ;
}
inline int sbuf_dec(sbuf *op,ulonglong v) noex {
	return sbuf_decull(op,v) ;
}

inline int sbuf_hex(sbuf *op,int v) noex {
	return sbuf_hexi(op,v) ;
}
inline int sbuf_hex(sbuf *op,long v) noex {
	return sbuf_hexl(op,v) ;
}
inline int sbuf_hex(sbuf *op,longlong v) noex {
	return sbuf_hexll(op,v) ;
}

inline int sbuf_hex(sbuf *op,uint v) noex {
	return sbuf_hexui(op,v) ;
}
inline int sbuf_hex(sbuf *op,ulong v) noex {
	return sbuf_hexul(op,v) ;
}
inline int sbuf_hex(sbuf *op,ulonglong v) noex {
	return sbuf_hexull(op,v) ;
}

#endif /* __cplusplus */


#endif /* SBUF_INCLUDE */


