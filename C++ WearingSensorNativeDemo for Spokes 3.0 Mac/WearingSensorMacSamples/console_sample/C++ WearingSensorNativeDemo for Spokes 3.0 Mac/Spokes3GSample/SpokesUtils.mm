#include <Foundation/Foundation.h>

void runSpokes(double seconds)
{
    // Let run loop to do some staff
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, seconds, false);
}