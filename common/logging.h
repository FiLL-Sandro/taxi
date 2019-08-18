#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <stdlib.h>

#define LOG_INFO_IS_ON
#ifdef LOG_INFO_IS_ON
#define log_info(_format, ...)				\
	do {									\
		printf(_format "\n", ##__VA_ARGS__);\
	} while(0);
#else
#define log_info(_format, ...)
#endif

#define LOG_DEBUG_IS_ON
#ifdef LOG_DEBUG_IS_ON
#define log_debug(_format, ...)						\
	do{												\
		printf("%s: %s: %d: " _format "\n",				\
			   __FILE__, __func__, __LINE__, ##__VA_ARGS__);	\
	} while(0);
#else
#define log_debug(_format, ...)
#endif

#define LOG_ERR_WITH_PERROR_IS_ON
#ifdef LOG_ERR_WITH_PERROR_IS_ON
#define log_err_with_perror(_format, ...)	\
	do {									\
		perror(NULL);						\
		log_debug(_format, ##__VA_ARGS__)	\
	} while(0);
#else
#define log_err_with_perror(_format, ...)
#endif

#endif
