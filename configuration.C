#include "configuration.h"


int model=5730;
int polarization=0; //in uA
char buf[200];
//char filename[200]="predeterminado";


int ReadUntil(int fd, char tok){
        if(fd<2){
                perror("ERROR fd to null file\n");exit(1);
        }
        int retread=1;
        int i=0;
        char data;
//      printf("leyendo archivo\n");
        while(retread){
                if((retread=read(fd,&data,1))==-1){
                        perror("ERROR reading input\n"); exit(1);
                }
                if(data==tok)
                        break;
                else
                        buf[i++]=data;
                if(i>200){
                        perror("ERROR buf overflow\n"); exit(1);
                }
        }
        buf[i]=0;
        if(retread!=1){
//              printf("fin de archivo\n");
                return 0;
        }else
                return i;
}

int readconfig(char *file){
        int fdin;
        char *param;
        if((fdin=open(file,O_RDONLY))==-1){
                perror("ERROR reading configuration file"); exit(1);
        }

        while(ReadUntil(fdin,'\n')!=0){
                if(buf[0]!='#'){
                        param=strtok(buf," ");
//                      std::cout << param <<std::endl;
                        if(0==strcmp(param,"model"))
                                model=atof(strtok(NULL," "));
                        if(0==strcmp(param,"polarization"))
                                polarization=atof(strtok(NULL," "));
                }
        }
        close(fdin);
        return 0;
}
