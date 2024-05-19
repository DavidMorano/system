/* ssh HEADER */
/* lang=C20 */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

#ifndef	SSH_INCLUDE
#define	SSH_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#define	SSH		struct ssh_head
#define	SSH_ENT		struct ssh_recentry
#define	SSH_INFO	struct ssh_info

#define	SSH_FILEMAGIC	"HAMMOCKS"
#define	SSH_FILEVERSION	0

/* branch types */
#define	SSH_BTFWD	1	/* forward */
#define	SSH_BTSSH	2	/* Simple Single-sided Hammock */


struct ssh_info {
	time_t	write ;		/* time DB written */
	uint	entries ;	/* total number of entries */
	uint	c_cf ;		/* control flow */
	uint	c_rel ;		/* relative branch */
	uint	c_con ;		/* conditional branch */
	uint	c_fwd ;		/* forward conditional branch */
	uint	c_ssh ;		/* SS-hammock */
} ;

struct ssh_recentry {
	uint	ia ;		/* instruction address */
	uint	ta ;		/* branch target address */
	uint	domainsize ;	/* domain size (instructions) */
	uint	type ;		/* branch type */
} ;

struct ssh_head {
	caddr_t			filemap ;
	struct ssh_recentry	*rectab ;
	uint			(*recind)[2] ;
	uint			magic ;
	uint			filesize ;
	uint			rtlen ;
	uint			rilen ;
} ;

EXTERNC_begin

extern int	ssh_init(SSH *,char *) ;
extern int	ssh_free(SSH *) ;
extern int	ssh_lookup(SSH *,uint,SSH_ENT **) ;
extern int	ssh_info(SSH *,SSH_INFO *) ;
extern int	ssh_get(SSH *,int,SSH_ENT **) ;

EXTERNC_end


#endif /* SSH_INCLUDE */



