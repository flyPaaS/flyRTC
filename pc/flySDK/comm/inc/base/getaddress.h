#ifndef GET_ADDRESS_H_
#define GET_ADDRESS_H_

#ifdef __cplusplus
extern "C"{
#endif
  
int get_host_name(const char* uname, char* hname);

#ifdef _WIN32
int get_local_address(char* local_addr);
#else
#define MAXADDRS    32 

void init_addresses(void);

void free_addresses(void);

int get_ip_addresses(int ipv6Enable);
int get_ip_address_by_ifname(int ipv6Enable, const char *if_name, char* ipaddress);

#endif

#ifdef __cplusplus
}
#endif
#endif