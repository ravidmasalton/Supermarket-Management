#pragma once

#define  CHECK_RETURN_0(p){if(p==NULL)return 0;}
#define  CHECK_MSG_RETURN_0(p,msg){if(p==NULL){printf(msg);return 0;}}
#define FREE_CLOSE_FILE_RETURN_0(file,p){free(p);fclose(file);return 0;}
#define CLOSE_RETURN_0(file){fclose(file);return 0;}