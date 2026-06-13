/* hname HEADER */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* address type headers */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HNAME_INCLUDE
#define	HNAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum hnames {
	hname_errorsto,
	hname_replyto,
	hname_sender,
	hname_from,
	hname_to,
	hname_cc,
	hname_bcc,
	hname_org,
	hname_msgid,
	hname_subject,
	hname_overlast
} ; /* end enum (hname) */


extern const char	*hname[] ;


#endif /* HNAME_INCLUDE */


