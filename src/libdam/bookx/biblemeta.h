/* biblemeta HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector string operations */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written for some previous
	purpose and was adapted for the present use.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */
/* version %I% last-modified %G% */

#ifndef	BIBLEMETA_INCLUDE
#define	BIBLEMETA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LINUC */


#define	BIBLEMETA		struct biblemeta_head
#define	BIBLEMETA_OBJ		struct biblemeta_object
#define	BIBLEMETA_MAGIC		0x99447244


enum biblemetas {
 	biblemeta_chapter,
 	biblemeta_psalm,
 	biblemeta_bookindex,
 	biblemeta_page,
 	biblemeta_booktitle,
 	biblemeta_thebookof,
 	biblemeta_book,
	biblemeta_overlast
} ; /* end enum */

struct biblemeta_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct biblemeta_head {
	vecstr		*dbp ;		/* data-base-pointer */
	uint		magval ;
} ; /* end struct */

typedef	BIBLEMETA	biblemeta ;
typedef	BIBLEMETA_OBJ	biblemeta_obj ;

EXTERNC_begin

extern int	biblemeta_open		(biblemeta *,cchar *,cchar *) noex ;
extern int	biblemeta_count		(biblemeta *) noex ;
extern int	biblemeta_nummax	(biblemeta *) noex ;
extern int	biblemeta_get		(biblemeta *,int,char *,int) noex ;
extern int	biblemeta_audit		(biblemeta *) noex ;
extern int	biblemeta_close		(biblemeta *) noex ;

EXTERNC_end


#endif /* BIBLEMETA_INCLUDE */


