/* pdb HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* printer database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PDB_INCLUDE
#define	PDB_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<svcfile.h>


#define	PDB_MAGIC	0x77446329
#define	PDB		struct pdb_head
#define	PDB_DB		struct pdb_dbase


enum pdbs {
	pdb_local,
	pdb_system,
	pdb_overlast
} ;

struct pdb_dbase {
	svcfile		*sfp ;
	time_t		ti_find ;
	time_t		ti_check ;
	time_t		ti_open ;
	time_t		ti_mtime ;
	uint		f_open:1 ;
} ;

struct pdb_head {
	cchar		*a ;		/* memory allocation */
	cchar		*pr ;		/* root program */
	cchar		*ur ;		/* root root */
	cchar		*uname ;	/* username */
	cchar		*fname ;
	PDB_DB		dbs[pdb_overlast] ;
	time_t		dt ;
	uint		magic ;
} ;

typedef	PDB		pdb ;
typedef	PDB_DB		pdb_db ;

EXTERNC_begin

extern int	pdb_open(pdb *,cchar *,cchar *,cchar *,cchar *) noex ;
extern int	pdb_fetch(pdb *,char *,int,cchar *,cchar *) noex ;
extern int	pdb_check(pdb *,time_t) noex ;
extern int	pdb_close(pdb *) noex ;

EXTERNC_end


#endif /* PDB_INCLUDE */


