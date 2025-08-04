
# Oroto Mobile Operating System

Advanced Kernel-Based Mobile Operating System for Security Professionals

## Build & Run

### Local Development
```bash
make clean
make release
./oroto-kernel
```

### Headless Mode (CI/Replit)
```bash
OROTO_HEADLESS=1 ./oroto-kernel
```

### Available Commands
- `oroto help` - Show available commands
- `oroto status` - System status
- `oroto tool list` - List security tools
- `ping [host]` - Network connectivity test
- `nmap [host]` - Port scanning
- `oroto poweroff` - Shutdown system
- `oroto reboot` - Restart system

## Environment Variables

- `OROTO_HEADLESS=1` - Run without TTY requirements (recommended for CI/cloud environments)

## Architecture

- **Kernel Core**: Main system management
- **Command Parser**: Command line interface
- **Device Interface**: Hardware abstraction layer  
- **Security Tools**: Network scanning and penetration testing
- **Logging System**: Comprehensive error tracking

## Requirements

- C++17 compatible compiler
- POSIX compatible system
- Make build system
