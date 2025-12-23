#ifndef LOG_C
#define LOG_C

static void
log_impl(char* string, char* file, i32 line)
{
	/* printf("%s in %s at %d"); */
}

#define log(string) log_impl((string), __FILE__, __LINE__)

#endif // LOG_C
