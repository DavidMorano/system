/* ns HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	NS_INCLUDE
#define	NS_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<unistd.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>
#include	<vecstr.h>


#define	NS		struct ns_head
#define	NS_AR		struct ns_args


struct ns_args {
	cchar		*pr ;
	int		timeout ;	/* connection timeout */
	int		options ;
} ;

struct ns_head {
	void		*addr ;		/* remote address */
	void		*con ;		/* connection handle */
	uint		magic ;
	int		itype ;
	int		fd ;
	int		addrlen ;
} ;

EXTERNC_begin

extern int	ns_open(NS *,NS_ARGS *, cchar *,cchar *) noex ;
extern int	ns_read(NS *,char *,int,int) noex ;
extern int	ns_write(NS *,cchar *,int) noex ;
extern int	ns_poll(NS *,int) noex ;
extern int	ns_close(NS *) noex ;
extern int	ns_localname(NS *,char *,int) noex ;
extern int	ns_peername(NS *,char *,int) noex ;

EXTERNC_end


#endif /* NS_INCLUDE */


