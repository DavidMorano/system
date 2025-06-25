/* sysnamedb HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* object to access both NODEDB and CLUSTERDB objects */
/* version %I% last-modified %G% */


/* revision history:

	= 1988-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1988 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	sysnamedb

	Description:
	This hack just keeps some interactions with a NODEDB object
	and a CLUSTERDB object together.  These two objects are often
	accessed together when attempting to determine a current
	default cluster name.

*******************************************************************************/

#ifndef	SYSNAMEDB_INCLUDE
#define	SYSNAMEDB_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<usystem.h>
#include	<nodedb.h>
#include	<clusterdb.h>
#include	<vecstr.h>


#define	SYSNAMEDB	struct sysnamedb_head
#define	SYSNAMEDB_FL	struct sysnamedb_flags
#define	SYSNAMEDB_MAGIC	0x18371157


struct sysnamedb_flags {
	uint		node:1 ;
	uint		clu:1 ;
} ;

struct sysnamedb_head {
	nodedb		*nlp ;		/* node-list-pointer */
	clusterdb	*clp ;		/* cluster-list-pointer */
	SYSNAMEDB_FL	fl ;
	uint		magic ;
} ;


typedef	SYSNAMEDB	sysnamedb ;
typedef	SYSNAMEDB_FL	sysnamedb_fl ;

EXTERNC_begin

extern int sysnamedb_open(sysnamedb *,cchar *) noex ;
extern int sysnamedb_getclusters(sysnamedb *,vecstr *,cchar *) noex ;
extern int sysnamedb_getnodes(sysnamedb *,vecstr *,vecstr *) noex ;
extern int sysnamedb_close(sysnamedb *) noex ;

EXTERNC_end


#endif /* SYSNAMEDB_INCLUDE */


