
#ifndef COLORS_H
#define COLORS_H

// ANSI Color codes for terminal output
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define ITALIC  "\033[3m"
#define UNDERLINE "\033[4m"
#define BLINK   "\033[5m"
#define REVERSE "\033[7m"
#define STRIKETHROUGH "\033[9m"

// Foreground colors
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

// Bright foreground colors
#define BRIGHT_BLACK   "\033[90m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_BLUE    "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN    "\033[96m"
#define BRIGHT_WHITE   "\033[97m"

// Background colors
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"

// Bright background colors
#define BG_BRIGHT_BLACK   "\033[100m"
#define BG_BRIGHT_RED     "\033[101m"
#define BG_BRIGHT_GREEN   "\033[102m"
#define BG_BRIGHT_YELLOW  "\033[103m"
#define BG_BRIGHT_BLUE    "\033[104m"
#define BG_BRIGHT_MAGENTA "\033[105m"
#define BG_BRIGHT_CYAN    "\033[106m"
#define BG_BRIGHT_WHITE   "\033[107m"

// Semantic color definitions for Oroto system
#define OROTO_SUCCESS    GREEN
#define OROTO_ERROR      RED
#define OROTO_WARNING    YELLOW
#define OROTO_INFO       CYAN
#define OROTO_DEBUG      MAGENTA
#define OROTO_PROMPT     BLUE
#define OROTO_HEADER     BOLD CYAN
#define OROTO_SYSTEM     BRIGHT_WHITE

// Special effects
#define OROTO_BLINK_ERROR    BLINK RED
#define OROTO_HIGHLIGHT      BOLD YELLOW BG_BLACK
#define OROTO_CRITICAL       BOLD WHITE BG_RED

// Function-like macros for consistent formatting
#define PRINT_SUCCESS(msg) std::cout << OROTO_SUCCESS << msg << RESET << std::endl
#define PRINT_ERROR(msg) std::cout << OROTO_ERROR << msg << RESET << std::endl
#define PRINT_WARNING(msg) std::cout << OROTO_WARNING << msg << RESET << std::endl
#define PRINT_INFO(msg) std::cout << OROTO_INFO << msg << RESET << std::endl

#endif // COLORS_H
