LIBNSS (Library of Name-Server-Switch)

This is part of the NSL (Netowrk-Socket-Layer) library implementation
from Illumos.  This is from the Name-Server-Switch (NSS) sub-facility
of the Illumos LIBNSL implementation.  The code used from Illumos
has not been used precisely directly, but rather has been repackaged
into the files below (as unchanged from the originals as possible).

Q. Why are we using code from the Illumos project?
A. We are using this code as an homage to the spirit of Illumos.
   I could have written code myself that performs the functions that
   the code below performs. But as an homage and courtesy to Illumos,
   I am going to actually use their code (essentially unchanged).

Q. What about the fact that the original Illumos code does not
   meet the quality standards of the enclosing projct?
A. This is not a unique problem associated with the Illumos code
   used here.  Other parts of the enclosing project also do not
   (in themselves) meet the quality standards of the enclosing project.
   Steps are taken to mitigate any damage that could occur from using
   lesser quality code. So these same mitigation techniqueus are
   availble for use with the Illumos code below.



file		description
-------------------------------------------------------------------------------

nss_parse.h		Name-Service-Switch public-part (header)
nss_parse.c		Name-Service-Switch private-part


