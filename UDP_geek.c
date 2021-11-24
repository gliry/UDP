#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <complex.h>
#include <fftw3.h>
   
#define PORT     4999
#define MAXLINE 1024
   
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;      
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
       
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
       
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    double* received_data = NULL;
    received_data = (double*)malloc(1024 * 320 * 2 * sizeof(double));

    double* send_data = NULL;
    send_data = (double*)malloc(1024 * 320 * 2 * sizeof(double));
    
    double* received_data_double = NULL;
    received_data_double = (double*)malloc(32 * 2 * sizeof(double));

    double* send_data_double = NULL;
    send_data_double = (double*)malloc(32 * 2 *sizeof(double));

    fftw_complex* data = NULL;
    data = (fftw_complex*)fftw_malloc(1024 * 320 * sizeof(fftw_complex));   
    int len, n, ii;
    len = sizeof(cliaddr);  //len is value/resuslt
    printf("Start receive\n");
    FILE* fp_2;
    fp_2 = fopen("/home/pi/Desktop/text.txt", "w");
	printf("opened\n");
    int k = 0;
    for (int i = 0; i < 10240; ++i)
    {
        n = recvfrom(sockfd, received_data_double, 32 * 2 * sizeof(double), 
                MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
	memcpy(received_data + (k * 64) , received_data_double, 32 * 2 * sizeof(double));
	++k;
	printf("i = %d Received: %d \n", i, n);
    }
    printf("Recieved\n");

    for (int i = 0; i < 1024; ++i)
    {
        for (int j = 0; j < 320; ++j)
        {
	    ii = 1024 * j + i;
	    data[ii] = received_data[ii * 2] + received_data[ii * 2 + 1] * I;
	   // printf("i = %d, j = %d   ", i, j);
	   // printf("%.20f + %.20fi\n", received_data[ii * 2], received_data[ii * 2 + 1]); 
        }
    }

for (int i = 0; i < 1024; ++i)
{
    for (int j = 0; j < 320; ++j)
    {
	//fprintf(fp_2, "i = %d  j = %d ", i, j);
	//fprintf(fp_2, "  %.20f\n", received_data);
        ii = 1024 * j + i;
	fprintf(fp_2, "i = %d j = %d  ", i, j);
	fprintf(fp_2, "%.20f + %.20fi\n", creal(data[ii]), cimag(data[ii])); 
    }
}
FILE* fp_3;
fp_3 = fopen("/home/pi/Desktop/text_2.txt", "w");
for (int i = 0; i < 1024; ++i)
{
    for (int j = 0; j < 320; ++j)
    {
	ii = (1024 * j + i);
	send_data[ii*2] = creal(data[ii]);
	send_data[ii*2 + 1] = cimag(data[ii]);
	fprintf(fp_3, "i = %d  j = %d  ", i, j);
	fprintf(fp_3, "% .20f + %.20fi\n", send_data[ii], send_data[ii+1]);
    }
}
int v = 0;
int h = 0;
for (int i = 0; i < 10240; ++i)
{
    // Здесь поделить на 10240 частей и sendto
    memcpy(send_data_double, send_data + (h * 64), 32 * 2 * sizeof(double));
    v = sendto(sockfd, send_data_double, 32 * 2 * sizeof(double), 0, (struct sockaddr *) &cliaddr, len);
    ++h;
    printf("%d  Sended:  %d\n", i, v);

}
    printf("Sended\n"); 
      
    return 0;
}
