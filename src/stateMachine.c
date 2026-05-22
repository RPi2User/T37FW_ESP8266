#include <stdint.h>


/*
 This state machine will execute each step in the following order.
    1. Listen
    2. Process
    3. Print

    Some interfaces are faster than others. We go from slowest to fastest and then from fastest to slowest

    1. Listen
        1.1 readTTY()
        1.2 readSerial()
        1.3 readAPI()
    2. Process
        - this generates the main string & symbol buffer
    3. Print
        3.1 printAPI()
        3.2 printSerial()
        3.3 printTTY()
    
 */