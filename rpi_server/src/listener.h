#ifndef __listener_h__
#define __listener_h__

#include "arm.h"
#include "tankTracks.h"

void *get_in_addr(struct sockaddr *sa);
char** str_split(char* a_str, const char a_delim);
void listen_t(Arm &arm, TankTracks &tankTracks);

#endif
