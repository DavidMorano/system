/* SETUID helper program */

#define _DARWIN_C_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>

#define MAX_ALLOWED_UIDS 4

// Whitelist of allowed target UIDs
uid_t allowed_uids[MAX_ALLOWED_UIDS] = { 501, 502 }; // example: only allow chown to users 501, 502

int is_allowed_uid(uid_t uid) {
    for (int i = 0; i < MAX_ALLOWED_UIDS; ++i) {
        if (allowed_uids[i] == uid)
            return 1;
    }
    return 0;
}

void usage(const char *progname) {
    fprintf(stderr, "Usage: %s <target-username> <filename>\n", progname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3)
        usage(argv[0]);

    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();

    if (effective_uid != 0) {
        fprintf(stderr, "Error: This program must be setuid root.\n");
        exit(EXIT_FAILURE);
    }

    const char *target_user = argv[1];
    const char *filename = argv[2];

    // Resolve username to UID
    struct passwd *pw = getpwnam(target_user);
    if (!pw) {
        fprintf(stderr, "Error: Unknown user '%s'\n", target_user);
        exit(EXIT_FAILURE);
    }

    uid_t target_uid = pw->pw_uid;

    if (!is_allowed_uid(target_uid)) {
        fprintf(stderr, 
		"Error: UID %d is not in the allowed list.\n", 
		target_uid);
        exit(EXIT_FAILURE);
    }

    // Stat the file
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    if (st.st_uid != real_uid) {
        fprintf(stderr, "Error: You do not own this file.\n");
        exit(EXIT_FAILURE);
    }

    // Only allow regular files
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "Error: Only regular files are supported.\n");
        exit(EXIT_FAILURE);
    }

    // Do the chown
    if (chown(filename, target_uid, -1) != 0) {
        perror("chown");
        exit(EXIT_FAILURE);
    }

    printf("Ownership of '%s' changed to user '%s' (UID %d)\n", filename, target_user, target_uid);
    return EXIT_SUCCESS;
}


/*******************************************************************************

Installation:

$ gcc -o chown-helper chown-helper.c
$ sudo chown root:wheel chown-helper
$ sudo chmod 4755 chown-helper

*******************************************************************************/


