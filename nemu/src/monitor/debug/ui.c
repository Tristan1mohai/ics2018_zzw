#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
extern CPU_state cpu;
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  printf("Continuing.\n");
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
	char *arg = strtok(NULL," ");
	if (arg==NULL){
		cpu_exec(1);
	} else {
		char *pos;
		long exec_num = strtol(arg, &pos, 10);
		if (exec_num<=0 || *pos!='\0'){
		   	printf("Invalid input!\n");
		}
		else {
		   	cpu_exec(exec_num);
		}
	}
	return 0;
}

static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	if (strcmp(arg,"r")==0){
		printf("eax%#20x%20u\n",cpu.eax,cpu.eax);
		printf("ebx%#20x%20u\n",cpu.ebx,cpu.ebx);
		printf("ecx%#20x%20u\n",cpu.ecx,cpu.ecx);
		printf("edx%#20x%20u\n",cpu.edx,cpu.edx);
		printf("esi%#20x%20u\n",cpu.esi,cpu.esi);
		printf("edi%#20x%20u\n",cpu.edi,cpu.edi);
		printf("ebp%#20x%#20x\n",cpu.ebp,cpu.ebp);	
		printf("esp%#20x%#20x\n",cpu.esp,cpu.esp);
		printf("eip%#20x%#20x\n",cpu.eip,cpu.eip);
	} else if (strcmp(arg,"w")==0){
		wp_info();
	} else 
		printf("Invalid input!\n");
	return 0;
}

static int cmd_x(char *args){
	char *pos1, *pos2;
    char *arg1 = strtok(NULL," ");
	char *arg2 = strtok(NULL," ");
	if (arg1==NULL && arg2==NULL) { 
		printf("Invalid input!\n");
		return 0;
	}
	if (arg2==NULL) {
		arg2 = arg1;
		arg1 = NULL;
	}
	long x_num = arg1==NULL? 1:strtol(arg1,&pos1,10);
	long x_pos = strtol(arg2,&pos2,16);
	if ((arg1!=NULL&&*pos1!='\0')||*pos2!='\0'||x_num<0||x_pos<0){
	   	printf("Invalid input!\n");
	}
	else {
		uint32_t size = 4;
		for (uint32_t i=0;i<x_num;++i){
		  if (x_pos+i*size>=0x8000000) {
			printf("Out of range 0x8000000, not availabel!\n");
			break;
		  }		  
	      printf("%#x:%#20x\n",(uint32_t)x_pos+i*size,vaddr_read(x_pos+i*size,4));
	}
	}
	return 0;
}

static int cmd_p(char *args){
	bool flag=true;
	if (args==NULL){
		printf("p must have parameters\n");
		return 0;
	}
	uint32_t val = expr(args,&flag);
	if (flag)
		printf("%u%#20x\n",val,val);
 	else printf("Invalid Input!\n");
	return 0;
} 

static int cmd_w(char *args){
	if (args==NULL) {
		printf("w must have parameter which is an expression!\n");
	} else
		new_wp(args);
	return 0;
}

static int cmd_d(char *args){
	if (args==NULL) {
		printf("d must have parameter which is NO of watchpoint!\n");
	} else {
		int i=0;
		char *arg1;
		do {
			i++;
			arg1 = strtok(NULL," ");
			if (arg1!=NULL){
				char *tmp;
				uint32_t num = strtol(arg1,&tmp,10);
				if (*tmp!='\0'){
					printf("Invalid input: %d parameter",i);
					return 0;
				}
				free_wp(num);
			}
		} while (arg1!=NULL);
	}
	return 0;
}


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step over exactly one instruction", cmd_si },
  { "info", "Examine specific area in machine", cmd_info },
  { "x", "Examine memory", cmd_x },
  { "p", "Expression Calculation", cmd_p },
  { "w", "Add a watchpoint", cmd_w },
  { "d", "Delete a watchpoint", cmd_d }
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  //char store_cmd[256];
  while (1) {
    char *str = rl_gets();
	
	/* Empty command means repetition of previous command*/
	Log("%s\n",str);
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    //strcpy(store_cmd,cmd);

	if (cmd == NULL) {continue;}

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
