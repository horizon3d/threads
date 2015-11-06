#include "logger/logger.h"

int main(int argc, char** argv)
{

   LogError("Program begin...");
   LogEvent("a = %d", 10);
   LogEvent("b = %d", 11);
   LogEvent("c = %d", 10 + 11);
   LogError("Program ending");

   return 0;
}