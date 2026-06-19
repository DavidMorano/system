/* biblebook HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* BIBLEBOOK object-load management */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEBOOK_INCLUDE
#define	BIBLEBOOK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LINU */
#include	<usysbase.h>		/* LINU */
#include	<modload.h>		/* LINUC */


#define	BIBLEBOOK		struct biblebook_head
#define	BIBLEBOOK_FL		struct biblebook_flags
#define	BIBLEBOOK_MAGIC		0x99447242
#define	BIBLEBOOK_LEN		80 /* bible-book-name length */
#define	BIBLEBOOK_NBOOKS	66


struct biblebook_flags {
        uint		modload:1 ;	/* dynamic load module */
	uint		localdb:1 ;	/* using local DB */
} ; /* end struct */

struct biblebook_head {
	modload		*mlp ;		/* module-loeader-pointer */
	voidp		callp ;		/* call-pointer */
	voidp		obj ;		/* object pointer */
	cchar		**names ;
	cchar		*namestrs ;
	BIBLEBOOK_FL	fl ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* object size */
	int		namesz ;	/* names-size */
} ; /* end struct (biblebook_head) */

typedef	BIBLEBOOK		biblebook ;
typedef	BIBLEBOOK_FL		biblebook_fl ;

EXTERNC_begin

extern int	biblebook_open	(biblebook *,cchar *,cchar *) noex ;
extern int	biblebook_count	(biblebook *) noex ;
extern int	biblebook_max	(biblebook *) noex ;
extern int	biblebook_read	(biblebook *,char *,int,int) noex ;
extern int	biblebook_look	(biblebook *,char *,int,int) noex ;
extern int	biblebook_get	(biblebook *,int,char *,int) noex ;
extern int	biblebook_match	(biblebook *,cchar *,int) noex ;
extern int	biblebook_size	(biblebook *) noex ;
extern int	biblebook_audit	(biblebook *) noex ;
extern int	biblebook_close	(biblebook *) noex ;

EXTERNC_end


#endif /* BIBLEBOOK_INCLUDE */


