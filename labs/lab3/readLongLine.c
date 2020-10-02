#include "readLongLine.h"
#include <string.h>

void *safe_calloc(size_t nmemb, int size){
	void *ret;
	if((ret=calloc(nmemb, size)) < 0){
		perror("calloc error");
		exit(EXIT_FAILURE);
	}
	return ret;
}
void *safe_realloc(void *src, int size){
	void *ret;
	if((ret=realloc(src, size)) < 0){
		perror("realloc error");
		exit(EXIT_FAILURE);
	}
	return ret;
}
void safe_free(char **buff)
{
    if(*buff != NULL){
        free(*buff);
		*buff=NULL;
	}
    return;
}
void safe_close(int *fd){
	if(*fd >= 0){
		close(*fd);
	}
	*fd = -1;
}

char *read_long_line(int inFd, int max_chars)
{
	int place = 0;
	char *pbuff;
	int num_resizes=1;
	char c; /*temp is for moving allong pbuff*/

	int onebuff = 0;
	
	pbuff = (char*)safe_calloc(MAXCHAR, sizeof(char));
	memset(pbuff, '\0', MAXCHAR);
	while((read(inFd, &c, sizeof(char))) > 0)
	{
		
       // Step 1 - check to see if were done reading input
	   if(c == '\n')
		   break;

		// Step 2 - check if our place is at the null char(resize if needed)
		if ( place >= (num_resizes*MAXCHAR)-1 ) 
		{
			num_resizes++;
			pbuff = safe_realloc(pbuff, num_resizes*MAXCHAR);
		}
		// Step 3 - if a user set a max_char then dont exceed that
		if(place >= max_chars-1)
			break;

		pbuff[place++] = c;
		
	}
	// Step 3 - check to see if no chars are filled
	if(strlen(pbuff)==0){
		safe_free(&pbuff);
		return NULL;
	}
	pbuff = safe_realloc(pbuff, strlen(pbuff)+1);
	return pbuff;
}




/*
int main(int argc, char *argv[])
{

	//int inFd = open(argv[1], O_RDONLY);
	int inFd = STDIN_FILENO;

	char *buff_ptr = read_long_line(inFd);


	printf("%s\n", buff_ptr);


	close(inFd);

	free(buff);

	return 0;
}
*/
