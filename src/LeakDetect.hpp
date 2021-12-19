#include <stdio.h>
#include <iostream>
#include <typeinfo>
#include <tuple>
#include <map>

#define STRINGIFY(n) #n
#define TOSTRING(n) STRINGIFY(n)
#define PREFIX __FILE__ ":" TOSTRING(__LINE__) ": "
