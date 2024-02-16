/* mailmsgattent HEADER */
/* lang=C20 */

/* mail-message attachment entry object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGATTENT_INCLUDE
#define	MAILMSGATTENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<contypevals.h>		/* content-type values */
#include	<mimetypes.h>
#include	<localmisc.h>


#define	MAILMSGATTENT_MAGIC	0x49827261
#define	MAILMSGATTENT		struct mailmsgattent


struct mailmsgattent {
	cchar		*type ;		/* content-type */
	cchar		*subtype ;
	cchar		*attfname ;	/* attachment-filename */
	cchar		*auxfname ;	/* auxiliary-filename */
	cchar		*ext ;
	cchar		*encoding ;	/* content-encoding */
	cchar		*description ;
	uint		magic ;
	int		clen ;		/* content-length */
	int		clines ;	/* content-lines */
	int		cte ;		/* content-transfer-encoding */
	int		f_plaintext ;
} ;

typedef MAILMSGATTENT	mailmsgattent ;

EXTERNC_begin

extern int mailmsgattent_start(MAILMSGATTENT *,cchar *,cchar *,cchar *,int) ;
extern int mailmsgattent_type(MAILMSGATTENT *,MIMETYPES *) ;
extern int mailmsgattent_typeset(MAILMSGATTENT *,cchar *,cchar *) ;
extern int mailmsgattent_isplaintext(MAILMSGATTENT *) ;
extern int mailmsgattent_finish(MAILMSGATTENT *) ;
extern int mailmsgattent_code(MAILMSGATTENT *,cchar *) ;
extern int mailmsgattent_setcode(MAILMSGATTENT *,int) ;
extern int mailmsgattent_analyze(MAILMSGATTENT *,cchar *) ;

EXTERNC_end


#endif /* MAILMSGATTENT_INCLUDE */


