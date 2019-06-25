


SamKnows Technical Test (C/C++)   25/6/19

This should have taken 2 uninterupted hours but it probably took around 3 hours staggered over three days. I did have to read up on libcurl before starting and also had to move to a different laptop as my current laptop is just a basic one I use for browsing which couldnt handle my Linux virtual machine environment.

The application was built using Ubuntu 18.04 environment using the GNU C compiler. The main.c has the entry point, key parsing, test setup and the measurement display at the end of the test. The http_measure.c houses the actual test execution functionality and the corresponding http_measure.h contains the prototypes and shared data for the application.

The testing was performed using various legal/illegal ‘number of request’ and ‘header’ parameters to verify the performance. It would have been good to have maybe produced an automated unit test with many permutations but time did not permit.

The application is ‘ http_measure’ and can be run from the command line using ./http_measure and with the options for number of requests and/or header following. When complete you will see the results as specified on the output:

eg: SKTEST;216.58.210.228;200;0.000030;0.000030;0.053662;0.064515

Testing of the dynamic memory was not performed, I would normally do this and this is one thing which I definiteley would have done given its usage.

Thanks for giving me the opportunity!!

Ian Rogan


