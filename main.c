/***************************************************************************
 *
 * main.c
 *
 * Receive the command, initiate the test and output results
 *
 ***************************************************************************/

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "http_measure.h"


struct option options[] =
{
    {"help",                no_argument,       0, 'h'},
    {"number of requests",  required_argument, 0, 'n'},
    {"html header",         required_argument, 0, 'H'},
    {0, 0, 0, 0}
};

/* Print help menu */
static void printHelp()
{
    printf("================================\n");
    printf(" HTTP Measurement App Help\n");
    printf(" Options:\n");
    printf(" -n  Number of HTTP requests <int> \n");
    printf(" -H  HTTP header requests <string>\n");
    printf(" -h  Help screen\n");
    printf("================================\n");
    printf("\n");
}

/* Main function */
int main(int argc, char* argv[])
{
    int i, option;
    int index = 0;
    int numRequests = DEFAULT_REQUESTS;

    http_measure_t* handle = http_measure_create(DEFAULT_URL);
    if(handle==NULL)
    {
        printf("Failed to initialise\n");
        return EXIT_SUCCESS;
    }

    while(1)
    {
        option = getopt_long(argc, argv, "h:n:H", options, &index);

        if(option == 'H')
        {
            http_measure_include_header(handle, optarg);
        }
        else if(option == 'n')
        {
            for(i=0; i<(int)strlen(optarg); i++)
            {
                if(!isdigit(optarg[i]))
                {
                    printf("ERROR: option -n is not a legal number!\n");
                    http_measure_abort(handle);
                    return EXIT_FAILURE;
                }
            }

            numRequests = atoi(optarg);
            if(numRequests < MIN_REQUESTS || numRequests > MAX_REQUESTS)
            {
                printf("ERROR: option -n not in range \n");
                http_measure_abort(handle);
                return EXIT_FAILURE;
            }

         }
         else if(option == 'h')
         {
            printHelp();
            http_measure_abort(handle);
            return EXIT_SUCCESS;
         }
         else if(option == -1)
         {
            break;
         }
    }

    /* Execute the test based on providing params */
    http_measure_results results;
    if(http_measure_execute(handle, numRequests, &results) != 0)
    {
        printf("ERROR: Failed to run HTTP using default URL\n");
        http_measure_abort(handle);
        return EXIT_FAILURE;
    }

    /*SKTEST;<IP address of HTTP server>;<HTTP response code>;<median of CURLINFO_NAMELOOKUP_TIME>;
    <median of CURLINFO_CONNECT_TIME>;<median of CURLINFO_STARTTRANSFER_TIME>;<median of CURLINFO_TOTAL_TIME>*/
    /* Results */
    printf("SKTEST;%s;%d;%lf;%lf;%lf;%lf\n",
                   results.serverIpAddress,
                   (int)results.responseCode,
                   results.nameLookupTime,
                   results.connectTime,
                   results.startTransferTime,
                   results.totalTime);

    return EXIT_SUCCESS;
}


