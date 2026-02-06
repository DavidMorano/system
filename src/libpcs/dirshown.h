/* dirshown HEADER */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* module to handle the "shown" status of directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	DIRSHOWN_INCLUDE
#define	DIRSHOWN_INCLUDE

#include	<envstandards.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vechand.h>
#include	<mkdirlist.h>


#define	DIRSHOWN	vechand
#define	DIRSHOWN_ENT	mkdirlist_ent


typedef	DIRSHOWN	dirshown ;
typedef	DIRSHOWN_ENT	dirshown_ent ;

EXTERNC_begin

extern int dirshown_start(dirshown *) noex ;
extern int dirshown_finish(dirshown *) noex ;
extern int dirshown_set(dirshown *,dirshown_ent *) noex ;
extern int dirshown_already(dirshown *,dirshown_ent *,dirshown_ent **) noex ;

EXTERNC_end


#endif /* DIRSHOWN_INCLUDE */


