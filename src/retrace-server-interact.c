#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <string.h>

int main (int argc, char *argv[])
{
    // Get all users in the retrace group
    struct group * retrace_gr = getgrnam("retrace");

    // Get the current user
    struct passwd *pw = getpwuid(getuid());

    // Test if the real user is in the retrace group
    bool in = false;
    for (int i = 0; retrace_gr->gr_mem[i] != NULL; i++)
    {
        if (!strcmp(retrace_gr->gr_mem[i], pw->pw_name))
        {
            in = true;
            break;
        }
    }

    if (!in)
    {
        printf("%s\n","Must be in the retrace group to execute retrace-mock");
        exit(1);
    }

    //Is in the retrace group, continue
    //Clear environment
    static const char *whitelist[] = {
        "LANG",
    };
    const size_t wlsize = sizeof(whitelist)/sizeof(char*);
    char *setlist[sizeof(whitelist)/sizeof(char*)] = { 0 };
    char *p = NULL;
    for (size_t i = 0; i < wlsize; i++)
        if ((p = getenv(whitelist[i])) != NULL)
            setlist[i] = strdup(p);

    //Now we can clear the environment
    clearenv();

    // And once again set whitelisted variables
    for (size_t i = 0; i < wlsize; i++)
        if (setlist[i] != NULL)
        {
            setenv(whitelist[i], setlist[i], 1);
            free(setlist[i]);
        }

    //Set gid to mock and execute retrace-server-interact
    setgid(getgrnam("mock")->gr_gid);
    const char *mock = "/usr/libexec/retrace-server-interact-real";
    argv[0] = (char*)mock;
    execv("/usr/libexec/retrace-server-interact-real", argv);
    return 0;
}
