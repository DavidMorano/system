/* fmtsub HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b. to customize it to
	what I want!

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	This (FMTSUB) is a helper object for the FMTSTR facility.

*******************************************************************************/

#ifndef	FMTSUB_INCLUDE
#define	FMTSUB_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	<fmtspec.hh>
#include	<fmtstrdata.h>


#define	FMTSUB		struct fmtsub_head
#define	FMTSUB_FL	struct fmtsub_flags


struct fmtsub_flags {
	uint		ov:1 ;		/* overflow */
	uint		mclean:1 ;	/* mode: clean-up */
	uint		mnooverr:1 ;	/* mode: return-error */
} ;

struct fmtsub_head {
	char		*ubuf ;		/* user buffer */
	FMTSUB_FL	f ;		/* flags */
	int		ulen ;		/* buffer length */
	int		len ;		/* current usage count */
	int		mode ;		/* format mode */
} ;

enum fmtsubmems {
	fmtsubmem_audit,
	fmtsubmem_finish,
	fmtsubmem_overlast
} ;

struct fmtsub ;

struct fmtsub_co {
	fmtsub		*op = nullptr ;
	int		w = -1 ;
	void operator () (fmtsub *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (fmtsub_co) */

struct fmtsub : fmtsub_head {
	fmtsub_co	audit ;
	fmtsub_co	finish ;
	fmtsub() noex : fmtsub_head{} {
	    audit(this,fmtsubmem_audit) ;
	    finish(this,fmtsubmem_finish) ;
	} ;
	fmtsub(const fmtsub &) = delete ;
	fmtsub &operator = (const fmtsub &) = delete ;
	int start(char *,int,int) noex ;
	int cleanstrw(cchar *,int = -1) noex ;
	int strw(cchar *,int = -1) noex ;
	int chr(int) noex ;
	int emit(fmtspec *,cchar *,int) noex ;
	int formstr(fmtspec *,fmtstrdata *) noex ;
	void dtor() noex ;
	~fmtsub() {
	    dtor() ;
	} ;
} ; /* end struct (fmtsub) */

typedef	FMTSUB_FL	fmtsub_fl ;

EXTERNC_begin

extern int fmtsub_start(fmtsub *,char *,int,int) noex ;
extern int fmtsub_finish(fmtsub *) noex ;
extern int fmtsub_strw(fmtsub *,cchar *,int) noex ;
extern int fmtsub_chr(fmtsub *,int) noex ;
extern int fmtsub_blanks(fmtsub *,int) noex ;
extern int fmtsub_cleanstrw(fmtsub *,cchar *,int) noex ;
extern int fmtsub_emit(fmtsub *,fmtspec *,cchar *,int) noex ;
extern int fmtsub_formstr(fmtsub *,fmtspec *,fmtstrdata *) noex ;
extern int fmtsub_reserve(fmtsub *,int) noex ;
extern int fmtsub_audit(fmtsub *) noex ;
extern int fmtsub_float(fmtsub *,int,double,int,int,int,char *) noex ;

EXTERNC_end


#endif /* __cplusplus */
#endif /* FMTSUB_INCLUDE */


