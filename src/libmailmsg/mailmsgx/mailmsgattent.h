/* mailmsgattent HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* mail-message attachment entry object management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGATTENT_INCLUDE
#define	MAILMSGATTENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<contypevals.h>		/* LIBMAILMSG content-type values */
#include	<mimetypes.h>		/* LIBUC */


#define	MAILMSGATTENT		struct mailmsgattent_head
#define	MAILMSGATTENT_MAGIC	0x49827261


struct mailmsgattent_head {
	cchar		*type ;		/* content-type */
	cchar		*subtype ;
	cchar		*attfname ;	/* attachment-filename */
	cchar		*auxfname ;	/* auxiliary-filename */
	cchar		*ext ;
	cchar		*encoding ;	/* content-encoding */
	cchar		*description ;
	uint		magval ;
	int		clen ;		/* content-length */
	int		clines ;	/* content-lines */
	int		cte ;		/* content-transfer-encoding */
	int		f_plaintext ;
} ; /* end struct */

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


