/***************************************************************************
 *
 * http_measure.c
 *
 * Perform the measurement of HTTP GET 's
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "http_measure.h"

struct http_measure
{
    char*              url;
    CURL*              curl;
    struct curl_slist* chunk;
};

/* Write function callback */
static size_t write_cb( void *ptr, size_t size, size_t nmemb, void *stream)
{
    (void) ptr;
    (void) stream;

    return size* nmemb;
}

/* Function to sort the array */
static void sortArray(double grade[], int n)
{
    int i, j;
    double temp;
    for(i = 0; i < n; ++i)
    {
        for(j = i + 1; j < n; ++j)
        {
            if(grade[i] < grade[j])
            {
                temp = grade[i];
                grade[i] = grade[j];
                grade[j] = temp;
            }
        }
    }
}

/* Function to calculate the median of the array */
static double getMedian(double array[], int n)
{
    double median=0;

    if(n%2 == 0) /* if number of elements are even */
    {
        median = (array[(n-1)/2] + array[n/2])/2.0;
    }
    else /* if number of elements are odd */
    {
        median = array[n/2];
    }
    return median;
}
/* Set up */
http_measure_t* http_measure_create(const char* url)
{
    http_measure_t* handle = calloc(1, sizeof(http_measure_t));
    if(handle==NULL)
    {
        return NULL;
    }

    handle->url = strdup(url);
    if(handle->url)
    {
        handle->curl = curl_easy_init();
        if(handle->curl)
        {
            curl_easy_setopt(handle->curl, CURLOPT_WRITEFUNCTION, &write_cb);
            curl_easy_setopt(handle->curl, CURLOPT_URL, handle->url );
            curl_easy_setopt(handle->curl, CURLOPT_FOLLOWLOCATION, 1);
            return handle;
        }
    }
    /* we have dropped to here if there is an issue so free up */
    curl_easy_cleanup(handle->curl);
    free(handle->url);
    free(handle);
    return NULL;
}

/* Tidy up */
int http_measure_abort(http_measure_t* handle)
{
    int returnvalue=EXIT_FAILURE;

    if(handle)
    {
        curl_slist_free_all(handle->chunk);
        curl_easy_cleanup(handle->curl);
        free(handle->url);
        free(handle);
        returnvalue=EXIT_SUCCESS;
    }

    return returnvalue;
}

/* Add header to list */
int http_measure_include_header(http_measure_t* handle, const char* headerEntry)
{
    int returnvalue=EXIT_SUCCESS;

    if((handle==NULL) || (headerEntry==NULL))
    {
		returnvalue=EXIT_FAILURE;
	}
	else
	{
        handle->chunk = curl_slist_append(handle->chunk, headerEntry);
    }

	return returnvalue;
}

/* Execute the test */
int http_measure_execute(http_measure_t* handle, int numRequests, http_measure_results* results)
{
    char*   ipAddress;
    double  nameLookupTime, connectTime, startTransferTime, totalTime;
    //double* nameLookupTimes, connectTimes, startTransferTimes, totalTimes;
    double* nameLookupTimes;
    double* connectTimes;
    double* startTransferTimes;
    double* totalTimes;
    int i;
    long    responseCode;
    int returnvalue=EXIT_SUCCESS;

    if((handle==NULL) || (results==NULL))
    {
        return EXIT_FAILURE;
    }

    if((numRequests < MIN_REQUESTS) || (numRequests > MAX_REQUESTS))
    {
        return EXIT_FAILURE;
    }

    nameLookupTimes = calloc(1, sizeof(double) * numRequests);
    connectTimes = calloc(1, sizeof(double) * numRequests);
    startTransferTimes = calloc(1, sizeof(double) * numRequests);
    totalTimes = calloc(1, sizeof(double) * numRequests);

    memset(results, 0, sizeof(*results));

    if((nameLookupTimes==NULL)     ||
        (connectTimes==NULL)        ||
        (startTransferTimes==NULL)  ||
        (totalTimes==NULL))
    {
        printf("ERROR: Failed to allocate time measurement arrays\n");
        free(nameLookupTimes);
        free(connectTimes);
        free(startTransferTimes);
        free(totalTimes);
        return EXIT_FAILURE;
    }

    /* Tell curl about custom header list */
    if(handle->chunk)
    {
        if(curl_easy_setopt(handle->curl, CURLOPT_HTTPHEADER, handle->chunk) != CURLE_OK)
        {
            printf("ERROR: Failed to set custom headers\n");
            returnvalue=EXIT_FAILURE;
        }
    }

    /* Perform measurements */

    for(i=0; i < numRequests; i++)
    {
#ifdef DEBUG
        printf("Sending HTTP GET Request [%d]\n", i+1);
#endif
        /* Send HTTP GET */
        if((curl_easy_perform(handle->curl) != CURLE_OK))
        {
            printf("ERROR: curl_easy_perform failed \n");
            returnvalue=EXIT_FAILURE;
        }

        /* Collect server info on first pass */
        if(i == 0)
        {
            if(curl_easy_getinfo(handle->curl, CURLINFO_PRIMARY_IP , &ipAddress) !=  CURLE_OK)
            {
                printf("ERROR: Failed to get CURLINFO_PRIMARY_IP\n");
                returnvalue=EXIT_FAILURE;
            }

            snprintf(results->serverIpAddress, sizeof(results->serverIpAddress), "%s", ipAddress);

            if(curl_easy_getinfo(handle->curl, CURLINFO_RESPONSE_CODE , &responseCode) !=  CURLE_OK)
            {
                printf("ERROR: Failed to get CURLINFO_RESPONSE_CODE\n");
                returnvalue=EXIT_FAILURE;
            }

            results->responseCode = responseCode;
        }

        if(curl_easy_getinfo(handle->curl, CURLINFO_NAMELOOKUP_TIME , &nameLookupTime) !=  CURLE_OK)
        {
            printf("ERROR: Failed to get CURLINFO_NAMELOOKUP_TIME\n");
            returnvalue=EXIT_FAILURE;
        }

        nameLookupTimes[i] = nameLookupTime;

        if(curl_easy_getinfo(handle->curl, CURLINFO_CONNECT_TIME  , &connectTime) !=  CURLE_OK)
        {
            printf("ERROR: Failed to get CURLINFO_CONNECT_TIME\n");
            returnvalue=EXIT_FAILURE;
        }

        connectTimes[i] = connectTime;

        if(curl_easy_getinfo(handle->curl, CURLINFO_STARTTRANSFER_TIME  , &startTransferTime) !=  CURLE_OK)
        {
            printf("ERROR: Failed to get CURLINFO_STARTTRANSFER_TIME\n");
            returnvalue=EXIT_FAILURE;
        }

        startTransferTimes[i] = startTransferTime;

        if(curl_easy_getinfo(handle->curl, CURLINFO_TOTAL_TIME  , &totalTime) !=  CURLE_OK)
        {
            printf("ERROR: Failed to get CURLINFO_TOTAL_TIME\n");
            returnvalue=EXIT_FAILURE;
        }

        totalTimes[i] = totalTime;
    }

    /* Calculate median values */
    sortArray(nameLookupTimes, numRequests);
    results->nameLookupTime  = getMedian(nameLookupTimes, numRequests);

    sortArray(connectTimes, numRequests);
    results->connectTime = getMedian(connectTimes, numRequests);

    sortArray(startTransferTimes, numRequests);
    results->startTransferTime = getMedian(startTransferTimes, numRequests);

    sortArray(totalTimes, numRequests);
    results->totalTime = getMedian(totalTimes, numRequests);

    /* Free local dynamic resources */
    free(nameLookupTimes);
    free(connectTimes);
    free(startTransferTimes);
    free(totalTimes);

    return returnvalue;
}

