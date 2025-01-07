/* mailmsgattent HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* mail-message attachment entry object management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGATTENT_INCLUDE
#define	MAILMSGATTENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<contypevals.h>		/* content-type values */
#include	<mimetypes.h>


#define	MAILMSGATTENT_MAGIC	0x49827261
#define	MAILMSGATTENT		struct mailmsgattent_head


struct mailmsgattent_head {
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

extern int mailmsgattent_start(mailmsgattent *,cc *,cc *,cc *,int) noex ;
extern int mailmsgattent_type(mailmsgattent *,mimetypes *) noex ;
extern int mailmsgattent_typeset(mailmsgattent *,cc *,cc *) noex ;
extern int mailmsgattent_isplaintext(mailmsgattent *) noex ;
extern int mailmsgattent_finish(mailmsgattent *) noex ;
extern int mailmsgattent_code(mailmsgattent *,cc *) noex ;
extern int mailmsgattent_setcode(mailmsgattent *,int) noex ;
extern int mailmsgattent_analyze(mailmsgattent *,cc *) noex ;

EXTERNC_end


#endif /* MAILMSGATTENT_INCLUDE */


