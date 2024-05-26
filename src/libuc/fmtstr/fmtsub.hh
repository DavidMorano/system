/* fmtsub HEADER */
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

typedef	FMTSUB		fmtsub ;
typedef	FMTSUB_FL	fmtsub_fl ;

EXTERNC_begin

extern int fmtsub_start(fmtsub *,char *,int,int) noex ;
extern int fmtsub_finish(fmtsub *) noex ;
extern int fmtsub_strw(fmtsub *,cchar *,int) noex ;
extern int fmtsub_chr(fmtsub *,int) noex ;
extern int fmtsub_blanks(fmtsub *,int) noex ;
extern int fmtsub_cleanstrw(fmtsub *,cchar *,int) noex ;
extern int fmtsub_emit(fmtsub *,fmtspec *,cchar *,int) noex ;
extern int fmtsub_fmtstr(fmtsub *,fmtspec *,fmtstrdata *) noex ;
extern int fmtsub_reserve(fmtsub *,int) noex ;
extern int fmtsub_float(fmtsub *,int,double,int,int,int,char *) noex ;

EXTERNC_end


#endif /* __cplusplus */
#endif /* FMTSUB_INCLUDE */


