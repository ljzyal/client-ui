#include <android/log.h>
#include <stdio.h>
#include <string.h>
#include <client-core.h>

#define DEBUG_TAG "client_DEBUG_cmd"

struct USER me;
int main(){
	FILE *fp = fopen("/data/data/com.ljzyal.client/files/config.cfg","r");
	fgets(me.name,100,fp);
	me.name[strlen(me.name)-1] = 0;
	fgets(me.password,100,fp);
	me.password[strlen(me.password)-1] = 0;
	strcpy(me.nic,"wlan0");
	fgets(me.mac,50,fp);
	fclose(fp);
	return client_login(me);
}
