/* svcfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* service- file manager */
/* version %I% last-modified %G% */


/* revision history:

	- 2004-05-25, David A­D­ Morano
	This subroutine was adopted for use as a service-table file
	processor.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	SVCFILE_INCLUDE
#define	SVCFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<limits.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<hdb.h>			/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */


#define	SVCFILE			struct svcfile_head
#define	SVCFILE_CUR		struct svcfile_cursor
#define	SVCFILE_ENT		struct svcfile_entry
#define	SVCFILE_MAGIC		0x31415926


struct svcfile_cursor {
	hdb_cur		*ecp ;		/* memory-allocated */
	int		i ;
} ; /* end struct */

struct svcfile_head {
	vecobj		*flp ;		/* file-list-pointer */
	vecobj		*slp ;		/* service-list-pointer */
	hdb		*elp ;		/* entry-list-pointer */
	time_t		checktime ;
	uint		magval ;
	int		ncursors ;
} ; /* end struct */

struct svcfile_entry {
	cchar		*(*keyvals)[2] ;
	cchar		*svc ;
	int		nkeys ;
	int		sz ;
	int		fi ;		/* file index */
} ; /* end struct */

typedef SVCFILE		svcfile ;
typedef SVCFILE_CUR	svcfile_cur ;
typedef SVCFILE_ENT	svcfile_ent ;

EXTERNC_begin

extern int svcfile_open(svcfile *,cchar *) noex ;
extern int svcfile_fileadd(svcfile *,cchar *) noex ;
extern int svcfile_curbegin(svcfile *,svcfile_cur *) noex ;
extern int svcfile_curend(svcfile *,svcfile_cur *) noex ;
extern int svcfile_curenumsvc(svcfile *,svcfile_cur *,char *,int) noex ;
extern int svcfile_curenum(svcfile *,svcfile_cur *,
		svcfile_ent *,char *,int) noex ;
extern int svcfile_fetch(svcfile *,cchar *,svcfile_cur *,
		svcfile_ent *,char *,int) noex ;
extern int svcfile_match(svcfile *,cchar *) noex ;
extern int svcfile_check(svcfile *,time_t) noex ;
extern int svcfile_close(svcfile *) noex ;

EXTERNC_end


#endif /* SVCFILE_INCLUDE */


