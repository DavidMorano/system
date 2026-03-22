/* ustream HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* support some buffered file operations */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-04-01, David A­D­ Morano
	This code was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ustream

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#ifndef	USTREAM_INCLUDE
#define	USTREAM_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<unistd.h>		/* system types */
#include	<cstddef>
#include	<cstdlib>
#include	<cstdarg>		/* |va_list(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>


#define	USTREAM_MAGIC	0x21387462
#define	USTREAM_RCNET	3		/* read-count for network */


enum ustreammems {
	ustreammem_reserve,
	ustreammem_invalidate,
	ustreammem_flush,
	ustreammem_adv,
	ustreammem_poll,
	ustreammem_lockend,
	ustreammem_close,
	ustreammem_writeblanks,
	ustreammem_writealign,
	ustreammem_writezero,
	ustreammem_overlast
} ; /* end enum (ustreammems) */

constexpr uint		ustream_magicval = USTREAM_MAGIC ;

struct ustream ;

struct ustream_fl {
	uint		net:1 ;		/* network FD */
	uint		write:1 ;	/* we are writing (otherwise reading) */
} ; /* end struct (ustream_flags) */

struct ustream_ma {
	ustream	*op = nullptr ;
	void operator () (ustream *p,int) noex {
	    op = p ;
	} ;
	template<typename ... Args> int operator () (Args ... ) noex ;
	operator int () noex ;
} ; /* end struct (ustream_ma) */

struct ustream_co {
	ustream		*op = nullptr ;
	int		w = -1 ;
	void operator () (ustream *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int) noex ;
	operator int () noex {
	    return operator () (0) ;
	} ;
} ; /* end struct (ustream_co) */

struct ustream {
    	friend		ustream_ma ;
    	friend		ustream_co ;
	ustream_ma	magic ;
	ustream_co	reserve ;
	ustream_co	invalidate ;
	ustream_co	flush ;
	ustream_co	adv ;
	ustream_co	poll ;
	ustream_co	lockend ;
	ustream_co	close ;
	ustream_co	writeblanks ;
	ustream_co	writealign ;
	ustream_co	writezero ;
	char		*dbuf ;		/* data-bufer pointer (constant) */
	char		*bptr ;		/* goes up with use */
	off_t		foff ;		/* virtual file pointer */
	ustream_fl	fl ;
	uint		magval ;
	int		fd ;		/* file-descriptor */
	int		oflags ;	/* open-flags */
	int		dt ;		/* file-type ("DT_XXX" values)*/
	int		dlen ;		/* data-buffer length (constant) */
	int		blen ;		/* length of valid-data in buffer */
	ustream() noex {
	    magic	(this,0) ;
	    reserve	(this,ustreammem_reserve) ;
	    invalidate	(this,ustreammem_invalidate) ;
	    flush	(this,ustreammem_flush) ;
	    adv		(this,ustreammem_adv) ;
	    poll	(this,ustreammem_poll) ;
	    lockend	(this,ustreammem_lockend) ;
	    close	(this,ustreammem_close) ;
	    writeblanks	(this,ustreammem_writeblanks) ;
	    writealign	(this,ustreammem_writealign) ;
	    writezero	(this,ustreammem_writezero) ;
	    magval = 0 ;
	} ; /* end ctor */
	ustream(const ustream &) = delete ;
	ustream &operator = (const ustream &) = delete ;
	int open	(cchar *,int = 0,mode_t = 0) noex ;
	int read	(void *,int,int = -1) noex ;
	int readp	(void *,int,off_t,int = -1) noex ;
	int readln	(char *,int,int = -1) noex ;
	int readlns	(char *,int,int = -1,int * = nullptr) noex ;
	int write	(cvoid *,int) noex ;
	int println	(cchar *,int) noex ;
	int printf	(cchar *,...) noex ;
	int vprintf	(cchar *,va_list) noex ;
    	int update	(off_t ,cchar *,int) noex ;
	int seek	(off_t,int) noex ;
	int tell	(off_t *) noex ;
	int stat	(ustat *) noex ;
	int lockbegin	(int,int) noex ;
	int writefill	(cchar *,int) noex ;
	int writefd	(char *,int,int,int) noex ;
	destruct ustream() {
	    if (magval) dtor() ;
	} ;
	int iwrite(cvoid *,int) noex ;
    private:
	int iclose() noex ;
	void dtor() noex ;
} ; /* end struct (ustream) */

template<typename ... Args>
local inline int ustream_magic(ustream *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == ustream_magicval) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (ustream_magic) */

template<typename ... Args> 
int ustream_ma::operator () (Args ... args) noex {
        int             rs = SR_FAULT ;
        if ((... && args)) {
            rs = (op->magval == ustream_magicval) ? SR_OK : SR_NOTOPEN ;
        }
        return rs ;
} /* end method (ustream_ma::operator) */

namespace ustream_ns {
    extern int	ustream_close		(ustream *) noex ;
    extern int	ustream_read		(ustream *,void *,int,int) noex ;
    extern int	ustream_readp		(ustream *,void *,int,off_t,int) noex ;
    extern int	ustream_readln		(ustream *,char *,int,int) noex ;
    extern int	ustream_readlns		(ustream *,char *,int,int,int *) noex ;
    extern int	ustream_write		(ustream *,cvoid *,int) noex ;
    extern int	ustream_println		(ustream *,cchar *,int) noex ;
    extern int	ustream_vprintf		(ustream *,cchar *,va_list) noex ;
    extern int	ustream_reserve		(ustream *,int) noex ;
    extern int	ustream_update		(ustream *,off_t,cchar *,int) noex ;
    extern int	ustream_invalidate	(ustream *) noex ;
    extern int	ustream_flush		(ustream *) noex ;
    extern int	ustream_adv		(ustream *,int) noex ;
    extern int	ustream_seek		(ustream *,off_t,int) noex ;
    extern int	ustream_tell		(ustream *,off_t *) noex ;
    extern int	ustream_poll		(ustream *,int) noex ;
    extern int	ustream_stat		(ustream *,ustat *) noex ;
    extern int	ustream_lockbegin	(ustream *,int,int) noex ;
    extern int	ustream_lockend		(ustream *) noex ;
    extern int	ustream_close		(ustream *) noex ;
    extern int	ustream_writeblanks	(ustream *,int) noex ;
    extern int	ustream_writealign	(ustream *,int) noex ;
    extern int	ustream_writezero	(ustream *,int) noex ;
} /* end namespace (ustream_na) */


#endif /* __cplusplus (C++ only) */
#endif /* USTREAM_INCLUDE */


