#include "logger/logger.h"

void ossFetchCurrentIdle()
{
   LogEvent("c = %d", 10 + 11);
}
/*
int main(int argc, char** argv)
{

   LogError("Program begin...");
   LogEvent("a = %d", 10);
   LogEvent("b = %d", 11);
   LogEvent("c = %d", 10 + 11);
   ossFetchCurrentIdle();
   LogError("Program ending");

   return 0;
}*/