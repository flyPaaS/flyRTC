#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>  
#include <sys/ioctl.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <netdb.h>  
#include <arpa/inet.h>  
#ifdef __APPLE__
#include <sys/sockio.h>  
#endif
#include <net/if.h>  
#include <errno.h>  
//#include <net/if_dl.h>
#endif
#include "getaddress.h"  

int get_host_name(const char* uname, char* hname)
{
	char computer_name[128] = {0};

	if (hname == NULL)
		return -1;
	if (gethostname(computer_name, sizeof(computer_name)) != 0)
		strcpy(computer_name, "host");

	sprintf(hname, "%s_%s", uname, computer_name);

	return 0;
}

#ifdef _WIN32
const char kDefaultLocalAddr[] = "127.0.0.1";

int get_local_address(char* local_addr)
{
	struct addrinfo *result = NULL;
	int error = 0;  
	struct addrinfo hint;
	char computer_name[256] = {0};

	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	if (local_addr == NULL)
		return -1;
	
	if (gethostname(computer_name, sizeof(computer_name)) != 0)
		strcpy(computer_name, "host");

	error = getaddrinfo(computer_name, NULL, &hint, &result);
	if (0 != error)
	{
		sprintf(local_addr, "%s", kDefaultLocalAddr);
	}
	else
	{
		//this we need first one
		sprintf(local_addr, "%hd.%hd.%hd.%hd",\
		((SOCKADDR_IN*)(result->ai_addr))->sin_addr.S_un.S_un_b.s_b1,\
		((SOCKADDR_IN*)(result->ai_addr))->sin_addr.S_un.S_un_b.s_b2,\
		((SOCKADDR_IN*)(result->ai_addr))->sin_addr.S_un.S_un_b.s_b3,\
		((SOCKADDR_IN*)(result->ai_addr))->sin_addr.S_un.S_un_b.s_b4);
		freeaddrinfo(result);
	}

	return 0;
}

#else
#define min(a,b)    ((a) < (b) ? (a) : (b))  
#define max(a,b)    ((a) > (b) ? (a) : (b))  
  
#define BUFFERSIZE  4000  
  
char *if_names[MAXADDRS];
char *ip_names[MAXADDRS];   
  
static int   nextAddr = 0;
  
void init_addresses(void)
{  
    int i = 0;  
	
    for (i=0; i<MAXADDRS; ++i)  
    {  
        if_names[i] = ip_names[i] = NULL;  
    }  

	nextAddr = 0;
}
  
void free_addresses(void)
{  
    int i = 0;  
	
    for (i=0; i<MAXADDRS; ++i)  
    {  
        if (if_names[i] != NULL) 
        {
			free(if_names[i]);
			if_names[i] = NULL;
		}
			  
        if (ip_names[i] != NULL) 
		{
			free(ip_names[i]);
			ip_names[i] = NULL;
		}  
    }  
	
    nextAddr = 0;  
}  
  
int get_ip_addresses(int ipv6Enable)
{  
    int                 len, flags;  
    char                buffer[BUFFERSIZE], *ptr, lastname[IFNAMSIZ], *cptr;  
    struct ifconf       ifc;  
    struct ifreq        *ifr, ifrcopy;
    struct sockaddr     *sa;
    char temp[80];  
    int sockfd = -1;
    int af = AF_INET;

    if ( ipv6Enable )
    {
        af = AF_INET6;
    }
    
    free_addresses(); 
    
    sockfd = socket(af, SOCK_DGRAM, 0);
    
    if (sockfd < 0)  
    {  
        perror("socket failed");  
        return -1;
    }  
      
    ifc.ifc_len = BUFFERSIZE;  
    ifc.ifc_buf = buffer;  
      
    if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)  
    {  
        perror("ioctl error");  
        return -1;
    }  
    
    lastname[0] = 0;
      
    for (ptr = buffer; ptr < buffer + ifc.ifc_len; )  
    {
        ifr = (struct ifreq *)ptr;  
#ifdef __APPLE__
        len = max(sizeof(struct sockaddr), ifr->ifr_addr.sa_len);  
#else
		len = sizeof(struct sockaddr);
#endif
        ptr += sizeof(ifr->ifr_name) + len;  // for next one in buffer  
          
        if (ifr->ifr_addr.sa_family != af)  
        {  
            continue;   // ignore if not desired address family  
        }  

        if ((cptr = (char *)strchr(ifr->ifr_name, ':')) != NULL)  
        {  
            *cptr = 0;  // replace colon will null  
        }  
          
        if ((strncmp(lastname, ifr->ifr_name, IFNAMSIZ) == 0))
        {  
            continue;   /* filter already processed  */  
        }  

        memcpy(lastname, ifr->ifr_name, IFNAMSIZ);  
          
        ifrcopy = *ifr;  
        ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy);  
        flags = ifrcopy.ifr_flags;  
        if ((flags & IFF_UP) == 0)  
        {  
            continue;   // ignore if interface not up  
        } 

        if_names[nextAddr] = (char *)malloc(strlen(ifr->ifr_name)+1);  
        if (if_names[nextAddr] == NULL)  
        {  
            return -2;
        }  
        strcpy(if_names[nextAddr], ifr->ifr_name);
        
        sa = (struct sockaddr*)&ifr->ifr_addr;  
        switch(sa->sa_family)
        {
            case AF_INET6:
            {
                struct sockaddr_in6 *sin6;
                sin6 = (struct sockaddr_in6*)sa;
                inet_ntop(AF_INET6, (struct in6_addr*)&(sin6->sin6_addr), temp, INET6_ADDRSTRLEN);
            }
            break;

            default:
            {
                struct sockaddr_in *sin;                
                sin = (struct sockaddr_in*)sa;
                inet_ntop(AF_INET, ((struct in_addr*)&(sin->sin_addr)), temp, INET_ADDRSTRLEN);
            }
            break;
        }
        
        ip_names[nextAddr] = (char *)malloc(strlen(temp)+1);  
        if (ip_names[nextAddr] == NULL)  
        {  
            return -2;
        }
        strcpy(ip_names[nextAddr], temp);
        ++nextAddr;
    }  
      
    close(sockfd);
    
    return 0;
}

int get_ip_address_by_ifname(int ipv6Enabled, const char *if_name, char* ipaddress)
{
    int i = 0;
    unsigned char* bytes = NULL;
    
    if (NULL == if_name || ipaddress == NULL)
        return -1;
    
    if (get_ip_addresses(ipv6Enabled) != 0)
        return -1;

	if (nextAddr == 0)
		return -2;
    
    for (i = 0; i < nextAddr; i++)
    {
        if ((strcmp(if_name, if_names[i]) == 0)
            && (ip_names[i] != NULL))
        {
            strcpy(ipaddress, ip_names[i]);
            return 0;
        }
    }
    
	i = nextAddr-1;	
	while ( i >= 0 )
	{
	    if ( ip_names[i] )
	    {
	        strcpy(ipaddress, ip_names[i]);
	        break;
	    }
	    i--;
	}
    
    return 0;
}
#endif 

