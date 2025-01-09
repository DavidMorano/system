/* ourmsginfo HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load up the process IDs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OURMSGINFO_INCLUDE
#define	OURMSGINFO_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>
#include	<dater.h>


#define	OURMSGINFO		struct ourmsginfo_head


enum ourmsginfo_heads {
	ourmsginfohead_messageid,
	ourmsginfohead_errorsto,
	ourmsginfohead_sender,
	ourmsginfohead_from,
	ourmsginfohead_replyto,
	ourmsginfohead_to,
	ourmsginfohead_overlast
} ;

struct ourmsginfo_head {
	char	*efrom ;		/* envelope FROM */
	char	*mid ;			/* message-id */
	char	*h_returnpath ;
	char	*h_subject ;
	DATER	edate ;			/* envelope date */
	vecstr	head[ourmsginfohead_overlast] ;
	time_t	etime ;			/* envelope time */
	time_t	mtime ;			/* message time */
	uint	hif ;			/* header initialization flags */
	uint	offset ;		/* file offset */
	uint	mlen ;			/* message length */
	int	clen ;			/* content length */
	int	f_messageid ;		/* had a message-id */
	int	f_spam ;		/* was a spam */
} ;

typedef	OURMSGINFO	ourmsginfo ;

EXTERNC_begin

extern int	ourmsginfo_start(ourmsginfo *,dater *) noex ;
extern int	ourmsginfo_setefrom(ourmsginfo *,cchar *,int) noex ;
extern int	ourmsginfo_setedate(ourmsginfo *,cchar *,int) noex ;
extern int	ourmsginfo_setmlen(ourmsginfo *,uint) noex ;
extern int	ourmsginfo_setclen(ourmsginfo *,int) noex ;
extern int	ourmsginfo_setoffset(ourmsginfo *,uint) noex ;
extern int	ourmsginfo_setspam(ourmsginfo *) noex ;
extern int	ourmsginfo_setsubject(ourmsginfo *,cchar *,int) noex ;
extern int	ourmsginfo_setreturnpath(ourmsginfo *,cchar *,int) noex ;
extern int	ourmsginfo_addhead(ourmsginfo *,int,cchar *,int) noex ;
extern int	ourmsginfo_gethead(ourmsginfo *,int,int,cchar **) noex ;
extern int	ourmsginfo_finish(ourmsginfo *) noex ;

EXTERNC_end


#endif /* OURMSGINFO_INCLUDE */


