/* acctab HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perform access-table file related functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written (and largely forgotten).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ACCTAB_INCLUDE
#define	ACCTAB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>
#include	<vecstr.h>


#define	ACCTAB_MAGIC		0x31415926
#define	ACCTAB			struct acctab_head
#define	ACCTAB_PA		struct acctab_part
#define	ACCTAB_ENT		struct acctab_entry
#define	ACCTAB_ERR		struct acctab_errline
#define	ACCTAB_CUR		struct acctab_cursor


struct acctab_cursor {
	int		i, j ;
} ;

struct acctab_head {
	vecobj		*flp ;			/* file-list-pointer */
	vecobj		*stdalp ;		/* access-list-entries */
	vecobj		*rgxalp ;		/* access-list-entries */
	time_t		checktime ;
	uint		magic ;
} ;

struct acctab_part {
	cchar		*patstd ;
	cchar		*patrgx ;
	int		patrgxlen ;
	int		type ;
} ;

struct acctab_entry {
	ACCTAB_PA	netgroup ;
	ACCTAB_PA	machine ;
	ACCTAB_PA	username ;
	ACCTAB_PA	password ;
	int		fi ;		/* file index */
} ;

struct acctab_errline {
	cchar		*fname ;
	int		line ;
} ;

typedef ACCTAB		acctab ;
typedef	ACCTAB_PA	acctab_pa ;
typedef ACCTAB_ENT	acctab_ent ;
typedef ACCTAB_CUR	acctab_cur ;
typedef ACCTAB_ERR	acctab_err ;

EXTERNC_begin

extern int acctab_open(acctab *,cchar *) noex ;
extern int acctab_fileadd(acctab *,cchar *) noex ;
extern int acctab_curbegin(acctab *,acctab_cur *) noex ;
extern int acctab_curend(acctab *,acctab_cur *) noex ;
extern int acctab_curenum(acctab *,acctab_cur *,acctab_ent **) noex ;
extern int acctab_check(acctab *) noex ;
extern int acctab_allowed(acctab *,cchar *,cchar *,cchar *,cchar *) noex ;
extern int acctab_anyallowed(acctab *,vecstr *,vecstr *,cchar *,cchar *) noex ;
extern int acctab_close(acctab *) noex ;

#ifdef	COMMENT
extern int acctab_find(acctab *,cchar *,acctab_ent **) noex ;
#endif

EXTERNC_end


#endif /* ACCTAB_INCLUDE */


