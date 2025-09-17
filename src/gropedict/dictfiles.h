/* dictfiles HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	DICTFILES_INCLUDE
#define	DICTFILES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bfile.h>


#define	DICTFILES		struct dictfiles_head
#define	DICTFILES_ENT		struct dictfiles_entry
#define	DICTFILES_FI		struct dictfiles_file
#define	DICTFILES_NLETTERS	256


struct dictfiles_entty {
	ulong		len ;
	ulong		usage ;
	int		fi ;
	int		f_open:1 ;
	int		f_trunced:1 ;
} ;

struct dictfiles_file {
	bfile		df ;
	uint		c ;		/* parent leading letter */
	uint		f_open:1 ;
} ;

struct dictfiles_head {
	DICT^FILES_FI	*files ;
	char		*dictdname ;
	char		*prefix ;
	DICTFILES_ENT	e[DICTFILES_NLETTERS] ;
	uint		magic ;
	int		maxopen ;
	int		nopen ;
	int		usage ;		/* counter */
} ;

typedef	DICTFILES	dictfiles ;
typedef	DICTFILES_ENT	dictfiles_ent ;
typedef	DICTFILES_FI	dictfiles_fi ;

EXTERNC_begin

extern int dictfiles_open(dictfiles *,int,cchar *,cchar *) noex ;
extern int dictfiles_write(dictfiles *,cchar *,int) noex ;
extern int dictfiles_close(dictfiles *) noex ;

EXTERNC_end


#endif /* DICTFILES_INCLUDE */



