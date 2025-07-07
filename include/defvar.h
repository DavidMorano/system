/* defvar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* object to handle parameter files */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-05, David A­D­ Morano
	This code was started for Levo related work.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEFVAR_INCLUDE
#define	DEFVAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* |time_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>


#define	DEFVAR_MAGIC	0x12349872
#define	DEFVAR		struct defvar_head
#define	DEFVAR_CUR	struct defvar_cursor


struct defvar_head {
	cchar		*fname ;
	vecstr		*vlp ;
	time_t		ti_check ;
	time_t		ti_filemod ;
	uint		magic ;
	int		intcheck ;
} ;

struct defvar_cursor {
	int		i ;
} ;

typedef	DEFVAR		defvar ;
typedef	DEFVAR_CUR	defvar_cur ;

EXTERNC_begin

extern int defvar_open(defvar *,cchar *) noex ;
extern int defvar_curbegin(defvar *,defvar_cur *) noex ;
extern int defvar_curend(defvar *,defvar_cur *) noex ;
extern int defvar_curenum(defvar *,defvar_cur *,char *,int,cc **) noex ;
extern int defvar_fetch(defvar *,cchar *,cchar **) noex ;
extern int defvar_close(defvar *) noex ;

EXTERNC_end


#endif /* DEFVAR_INCLUDE */


