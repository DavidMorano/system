/* deffile HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* object to handle parameter files */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code was started for Levo related work.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEFFILE_INCLUDE
#define	DEFFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>


#define	DEFFILE_MAGIC	0x12349872
#define	DEFFILE		struct deffile_head
#define	DEFFILE_CUR	struct deffile_cursor


struct deffile_head {
	cchar		*fname ;
	vecstr		vars ;
	time_t		ti_check ;
	time_t		ti_filemod ;
	uint		magic ;
	int		intcheck ;
} ;

struct deffile_cursor {
	int		i ;
} ;

typedef	DEFFILE		deffile ;
typedef	DEFFILE_CUR	deffile_cur ;

EXTERNC_begin

extern int deffile_open(deffile *,cchar *) noex ;
extern int deffile_curbegin(deffile *,deffile_cur *) noex ;
extern int deffile_curend(deffile *,deffile_cur *) noex ;
extern int deffile_enum(deffile *,deffile_cur *,char *,int,cchar **) noex ;
extern int deffile_fetch(deffile *,cchar *,cchar **) noex ;
extern int deffile_close(deffile *) noex ;

EXTERNC_end


#endif /* DEFFILE_INCLUDE */


