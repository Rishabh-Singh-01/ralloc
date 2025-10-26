#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <string.h>
#include <time.h>

// --- Configuration ---

/**
 * @brief Define the minimum logging level that will be compiled and printed.
 *
 * Change this value to control verbosity globally. Logs with a level greater
 * than LOG_MIN_LEVEL will be discarded at compilation.
 * 0: FATAL, 1: ERROR, 2: WARNING, 3: INFO, 4: DEBUG
 */
/*#define LOG_MIN_LEVEL 3 // Set to 3 (INFO) by default*/

// --- Log Level Definitions ---

/**
 * @brief Enum defining the severity of the log messages.
 */
typedef enum {
  LOG_LEVEL_FATAL = 0, // Critical application failure
  LOG_LEVEL_ERROR = 1, // Serious error, may affect functionality
  LOG_LEVEL_WARN = 2,  // Potential issue or unusual circumstance
  LOG_LEVEL_INFO = 3,  // General application status and progress
  LOG_LEVEL_DEBUG = 4  // Detailed tracing information (most verbose)
} LogLevel;

/**
 * @brief Array of strings corresponding to the LogLevel enum for printing.
 */
static const char *LOG_LEVEL_STRINGS[] = {"FATAL", "ERROR", "WARN ", "INFO ",
                                          "DEBUG"};

// --- Core Logging Macro ---

/**
 * @brief The core logging macro. It checks if the requested level is low enough
 * to be printed (level <= LOG_MIN_LEVEL) before processing.
 *
 * @param level The log level (e.g., LOG_LEVEL_ERROR).
 * @param fmt The format string for the message.
 * @param ... The variable arguments for the format string.
 *
 * The ##__VA_ARGS__ is a GCC extension that allows the comma to be dropped if
 * the variable arguments are empty, making it safe for use without arguments.
 */
#define LOG_PRINT(level, fmt, ...)                                             \
  do {                                                                         \
    time_t t = time(NULL);                                                     \
    struct tm tm = *localtime(&t);                                             \
    char time_str[20];                                                         \
    strftime(time_str, sizeof(time_str), "%H:%M:%S", &tm);                     \
                                                                               \
    /* Print to standard error stream for better visibility */                 \
    fprintf(stderr, "[%s] %s (%s:%d): " fmt "\n", time_str,                    \
            LOG_LEVEL_STRINGS[level], __FILE__, __LINE__, ##__VA_ARGS__);      \
  } while (0)

// --- Convenience Macros ---

/**
 * @brief Logs a message at the FATAL level (0).
 */
#define LOG_FATAL(fmt, ...) LOG_PRINT(LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

/**
 * @brief Logs a message at the ERROR level (1).
 */
#define LOG_ERROR(fmt, ...) LOG_PRINT(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

/**
 * @brief Logs a message at the WARNING level (2).
 */
#define LOG_WARN(fmt, ...) LOG_PRINT(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/**
 * @brief Logs a message at the INFO level (3).
 */
#define LOG_INFO(fmt, ...) LOG_PRINT(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/**
 * @brief Logs a message at the DEBUG level (4).
 */
#define LOG_DEBUG(fmt, ...) LOG_PRINT(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#endif // LOGGER_H
