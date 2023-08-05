#include "kernel/types.h"
#include "user/user.h"


int main(int argc, char *argv[]){
	if(argc != 3){
	  fprintf(2, "usage: setpriority priority pid\n");
  	  exit(1);
	}

	int new_priority = atoi(argv[0]);
	int pid = atoi(argv[1]);



	if(!set_priority(new_priority, pid)){
        fprintf(2, "setpriority failed\n");
    }
	exit(0);
}
