/* article HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage an ARTICLE object */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	ARTICLE_INCLUDE
#define	ARTICLE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
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
#include	<ng.h>


#define	ARTICLE		struct article_head
#define	ARTICLE_FL	struct article_flags
#define	ARTICLE_MAGIC	0x02746258


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
} ; /* end enum (articleaddrs) */

enum articlestrs {
	articlestr_messageid,
	articlestr_articleid,
	articlestr_envfrom,
	articlestr_subject,
	articlestr_ngdname,
	articlestr_overlast
} ; /* end enum (articlestrs) */

struct article_flags {
	uint		path:1 ;
	uint		envdates:1 ;
	uint		msgdate:1 ;
	uint		ngs:1 ;
	uint		spam:1 ;
} ; /* end struct */

struct article_head {
	retpath		*pathp ;
	ng		*ngp ;
	vechand		*envp ;
	dater		*msgp ;
	ema		addr[articleaddr_overlast] ;
	cchar		*strs[articlestr_overlast] ;
	ARTICLE_FL	fl ;
	uint		aoff ;
	uint		alen ;
	uint		magic ;
	int		clen ;
	int		clines ;
	char		af[articleaddr_overlast] ;
} ; /* end struct */

typedef ARTICLE		article ;
typedef ARTICLE_FL	article_fl ;

EXTERNC_begin

extern int article_start(article *) noex ;
extern int article_addenvdate(article *,dater *) noex ;
extern int article_addmsgdate(article *,dater *) noex ;
extern int article_addpath(article *,cchar *,int) noex ;
extern int article_addng(article *,cchar *,int) noex ;
extern int article_addstr(article *,int,cchar *,int) noex ;
extern int article_addaddr(article *,int,cchar *,int) noex ;
extern int article_ao(article *,uint,uint) noex ;
extern int article_countenvdate(article *) noex ;
extern int article_getenvdate(article *,int,dater **) noex ;
extern int article_getstr(article *,int,cchar **) noex ;
extern int article_getaddrema(article *,int,ema **) noex ;
extern int article_finish(article *) noex ;

EXTERNC_end


#endif /* ARTICLE_INCLUDE */


