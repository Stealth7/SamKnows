
/***************************************************************************
 *
 * http_measure.h
 *
 * Perform the measurement of HTTP GET header
 *
 ***************************************************************************/

#ifndef _HTTP_MEASURE_H_
#define _HTTP_MEASURE_H_

#define DEFAULT_URL "http://www.google.com"

#define MIN_REQUESTS     (5)
#define MAX_REQUESTS     (100)
#define DEFAULT_REQUESTS (10)


typedef struct http_measure http_measure_t;

typedef struct
{
    char    serverIpAddress[50];
    long    responseCode;
    double  nameLookupTime;
    double  connectTime;
    double  startTransferTime;
    double  totalTime;
} http_measure_results;


http_measure_t* http_measure_create(const char* url);

int http_measure_include_header(http_measure_t* handle, const char* headerEntry);

int http_measure_execute(http_measure_t* handle, int numRequests, http_measure_results* results);

int http_measure_abort(http_measure_t* handle);


#endif /* _HTTP_MEASURE_H_ */
