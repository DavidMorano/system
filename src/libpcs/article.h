/* article HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage an ARTICLE object */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	ARTICLE_INCLUDE
#define	ARTICLE_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<vechand.h>
#include	<retpath.h>
#include	<dater.h>
#include	<ema.h>

#include	"ng.h"


#define	ARTICLE		struct article_head
#define	ARTICLE_FL	struct article_flags


enum articleaddrs {
	articleaddr_sender,
	articleaddr_replyto,
	articleaddr_from,
	articleaddr_to,
	articleaddr_cc,
	articleaddr_bcc,
	articleaddr_errorsto,
	articleaddr_deliveredto,
	articleaddr_xoriginalto,
	articleaddr_newsgroups,
	articleaddr_overlast
} ;

enum articlestrs {
	articlestr_messageid,
	articlestr_articleid,
	articlestr_envfrom,
	articlestr_subject,
	articlestr_ngdname,
	articlestr_overlast
} ;

struct article_flags {
	uint		path:1 ;
	uint		envdates:1 ;
	uint		msgdate:1 ;
	uint		ngs:1 ;
	uint		spam:1 ;
} ;

struct article_head {
	RETPATH		path ;
	NG		ngs ;
	VECHAND		envdates ;
	dater		msgdate ;
	ema		addr[articleaddr_overlast] ;
	cchar		*strs[articlestr_overlast] ;
	ARTICLE_FL	fl ;
	uint		aoff ;
	uint		alen ;
	int		clen ;
	int		clines ;
	char		af[articleaddr_overlast] ;
} ;

EXTERNC_begin

extern int article_start(ARTICLE *) noex ;
extern int article_addenvdate(ARTICLE *,dater *) noex ;
extern int article_addmsgdate(ARTICLE *,dater *) noex ;
extern int article_addpath(ARTICLE *,cchar *,int) noex ;
extern int article_addng(ARTICLE *,cchar *,int) noex ;
extern int article_addstr(ARTICLE *,int,cchar *,int) noex ;
extern int article_addaddr(ARTICLE *,int,cchar *,int) noex ;
extern int article_ao(ARTICLE *,uint,uint) noex ;
extern int article_countenvdate(ARTICLE *) noex ;
extern int article_getenvdate(ARTICLE *,int,dater **) noex ;
extern int article_getstr(ARTICLE *,int,cchar **) noex ;
extern int article_getaddrema(ARTICLE *,int,ema **) noex ;
extern int article_finish(ARTICLE *) noex ;

EXTERNC_end


#endif /* ARTICLE_INCLUDE */


