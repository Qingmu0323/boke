#include <stdio.h>
#include "wrap.h"
#include "parse.h"

int main(void) 
{
    char *buf, *p;
    char cmd1[MAXLINE], cmd2[MAXLINE],content[MAXLINE];

    /* Extract the two arguments */
    if ((buf = getenv("QUERY_STRING")) != NULL) {
	p = strchr(buf, '&');
	*p = '\0';
	strcpy(cmd1, buf);
	strcpy(cmd2, p+1);
    }

    FILE * stream = popen(cmd1, "r");

	if (!stream) {
		printf("error: popen(%s) returned %p\n", cmd1, stream);
	}
	else{
		char result[1024+1] = {0};
		while (!feof(stream)){
			if (fgets(result, 1024, stream) != NULL){
                /* Make the response body */
                sprintf(content, "Welcome to shell.com:%s and %s\r\n<p>",cmd1,cmd2);
                sprintf(content, "%s\r\n", content);
                sprintf(content, "%sThanks for visiting!\r\n", content); 
                strcat (content, "\r\n");
                strcat (content, result);
                strcat (content, "\r\n");
                printf("Content-length: %d\r\n", strlen(content));
                printf("Content-type: text/html\r\n\r\n");
                printf("%s", content);
                fflush(stdout);
                pclose(stream);
                stream = NULL;
                exit(0);
			}
		}
	}
}
    