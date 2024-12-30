/* dbi HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	DBI_INCLUDE
#define	DBI_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<nodedb.h>
#include	<clusterdb.h>
#include	<vecstr.h>


#define	DBI		struct dbi_head
#define	DBI_FL		struct dbi_flags
#define	DBI_MAGIC	0x18371157


struct dbi_flags {
	uint		node:1 ;
	uint		clu:1 ;
} ;

struct dbi_head {
	nodedb		*nlp ;		/* node-list-pointer */
	clusterdb	*clp ;		/* cluster-list-pointer */
	DBI_FL		fl ;
	uint		magic ;
} ;


typedef	DBI		dbi ;
typedef	DBI_FL		dbi_fl ;

EXTERNC_begin

extern int dbi_open(dbi *,cchar *) noex ;
extern int dbi_getclusters(dbi *,vecstr *,cchar *) noex ;
extern int dbi_getnodes(dbi *,vecstr *,vecstr *) noex ;
extern int dbi_close(dbi *) noex ;

EXTERNC_end


#endif /* DBI_INCLUDE */


