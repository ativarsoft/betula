#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <netdb.h>

int inetaddr(const char *name_ptr, size_t name_length, char **ip_ptr, size_t *ip_length)
{
    struct hostent *host = NULL;
    struct in_addr inaddr = {0};
    char ipstr[INET_ADDRSTRLEN] = "";
    char *name = NULL;

    name = strndup(name_ptr, name_length);

    host = gethostbyname(name);
    if (host == NULL)
        return 1;
    inaddr.s_addr = *((uint32_t *)host->h_addr_list[0]);
    inet_ntop(AF_INET, &inaddr, ipstr, INET_ADDRSTRLEN);
    *ip_ptr = strndup(ipstr, INET_ADDRSTRLEN);
    *ip_length = strlen(*ip_ptr);
    free(name);
    return 0;
}
