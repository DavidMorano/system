/* pingtab HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to handle the user's PINGTAB file */
/* version %I% last-modified %G% */

#ifndef	PINGTAB_INCLUDE
#define	PINGTAB_INCLUDE	


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */


#define	PINGTAB		struct pingtab_head
#define	PINGTAB_ENT	struct pingtab_ent
#define	PINGTAB_MAGIC	0x9876dcba


struct pingtab_head {
	uint		magval ;
	bfile		*pfp ;
} ; /* end struct */

struct pingtab_ent {
	int		intminping ;
	int		timeout ;
} ; /* end struct */

typedef	PINGTAB		pingtab ;
typedef	PINGTAB_ENT	pingtab_ent ;

EXTERNC_begin

extern int pingtab_open		(pingtab *,const char *) noex ;
extern int pingtab_close	(pingtab *) noex ;
extern int pingtab_read		(pingtab *,pingtab_ent *,charp,int) noex ;
extern int pingtab_rewind	(pingtab *) noex ;

EXTERNC_end


#endif /* PINGTAB_INCLUDE */



