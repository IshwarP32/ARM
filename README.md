# ARM RTOS Scheduler Project

## 📋 Project Overview

This project implements a **software-simulated Real-Time Operating System (RTOS) scheduler** designed for ARM Cortex-M microcontrollers. The project is compatible with **Keil uVision IDE** (free version) and serves as an educational platform for understanding RTOS concepts and embedded systems programming.

### 🎯 Project Objectives

- Implement a priority-based preemptive scheduler
- Provide task management with multiple priority levels
- Support inter-task communication via message queues
- Implement semaphores for synchronization
- Include dynamic memory management
- Provide timer services and interrupt handling
- Create a beginner-friendly, well-documented codebase

## 👥 Team Structure (6 Members)

| Team Member | Responsibility | Files |
|-------------|----------------|-------|
| **Member 1** | Task Management | `task_manager.h/c` |
| **Member 2** | Scheduler Implementation | `scheduler.h/c` |
| **Member 3** | Queue/Semaphore Management | `queue_manager.h/c` |
| **Member 4** | Memory Management | `memory_manager.h/c` |
| **Member 5** | Timer/Interrupt Management | `timer_manager.h/c` |
| **Member 6** | System Integration | `main.c`, examples, documentation |

## 🏗️ Project Structure

```
ARM_RTOS_Scheduler/
├── src/                    # Source files
│   ├── main.c             # Main application entry point
│   ├── task_manager.c     # Task creation and management
│   ├── scheduler.c        # Priority-based scheduler
│   ├── queue_manager.c    # Message queues and semaphores
│   ├── memory_manager.c   # Dynamic memory allocation
│   └── timer_manager.c    # Timer and interrupt services
├── include/               # Header files
│   ├── rtos_config.h      # Configuration and common definitions
│   ├── task_manager.h     # Task management interface
│   ├── scheduler.h        # Scheduler interface
│   ├── queue_manager.h    # Queue/semaphore interface
│   ├── memory_manager.h   # Memory management interface
│   └── timer_manager.h    # Timer management interface
├── examples/              # Example applications
│   ├── led_blink_example.c
│   └── producer_consumer_example.c
├── docs/                  # Documentation
├── config/                # Configuration files
├── ARM_RTOS_Scheduler.uvprojx  # Keil uVision project file
└── README.md             # This file
```

## 🚀 Getting Started

### ⚠️ Important Note - Simplified Version

This project uses a **simplified cooperative scheduling approach** to make it easier for beginners to understand. See [SIMPLIFIED_APPROACH.md](SIMPLIFIED_APPROACH.md) for detailed explanation.

**Key differences from production RTOS:**
- Tasks execute one iteration at a time (no infinite loops in task functions)
- Cooperative scheduling using a main loop (not preemptive with interrupts)
- Program completes after 1000 iterations (for demonstration)
- Designed for learning, not production use

### Prerequisites

1. **Keil uVision IDE** (free version)
   - Download from: https://www.keil.com/download/product/
   - ARM Cortex-M3 support included in free version

2. **Hardware Requirements**
   - None required - runs in simulator
   - Optional: ARM development board for real hardware testing

### Setup Instructions

1. **Clone the Repository**
   ```bash
   git clone [repository-url]
   cd ARM
   ```

2. **Open in Keil uVision**
   - Launch Keil uVision IDE
   - Open project: `ARM_RTOS_Scheduler.uvprojx`
   - Select target: `ARM_RTOS_Simulator`

3. **Build the Project**
   - Press `F7` or use `Project → Build Target`
   - Ensure no compilation errors

4. **Run Simulation**
   - Press `Ctrl+F5` or use `Debug → Start/Stop Debug Session`
   - Use simulator to observe RTOS behavior

## 🔧 Configuration

### RTOS Configuration (rtos_config.h)

Key configuration parameters:

```c
#define MAX_TASKS               8       // Maximum number of tasks
#define TIME_SLICE_MS          10       // Time slice for round-robin
#define HEAP_SIZE            4096       // Dynamic memory heap size
#define TICK_RATE_HZ         1000       // System tick frequency
#define MAX_QUEUES              4       // Maximum message queues
```

### Priority Levels

- `PRIORITY_IDLE` (0) - Idle task
- `PRIORITY_LOW` (1) - Background tasks
- `PRIORITY_MEDIUM` (2) - Normal tasks
- `PRIORITY_HIGH` (3) - Important tasks
- `PRIORITY_CRITICAL` (4) - Critical tasks

## 📚 Core Components

### 1. Task Manager (Member 1)
**Files:** `task_manager.h/c`

**Responsibilities:**
- Task creation and deletion
- Task Control Block (TCB) management
- Task state transitions
- Stack management

**Key Functions:**
```c
uint8_t task_create(void (*function)(void), const char* name, uint8_t priority, uint32_t stack_size);
rtos_result_t task_delete(uint8_t task_id);
void task_delay(uint32_t delay_ticks);
```

### 2. Scheduler (Member 2)
**Files:** `scheduler.h/c`

**Responsibilities:**
- Priority-based preemptive scheduling
- Round-robin for equal priorities
- Context switching
- Ready queue management

**Key Functions:**
```c
void scheduler_start(void);
tcb_t* scheduler_get_next_task(void);
void scheduler_switch_context(void);
```

### 3. Queue/Semaphore Manager (Member 3)
**Files:** `queue_manager.h/c`

**Responsibilities:**
- Message queue implementation
- Semaphore management
- Inter-task communication
- Synchronization primitives

**Key Functions:**
```c
queue_result_t queue_send(uint8_t queue_id, const void* data, uint32_t timeout);
queue_result_t queue_receive(uint8_t queue_id, void* data, uint32_t timeout);
rtos_result_t semaphore_take(uint8_t semaphore_id, uint32_t timeout);
rtos_result_t semaphore_give(uint8_t semaphore_id);
```

### 4. Memory Manager (Member 4)
**Files:** `memory_manager.h/c`

**Responsibilities:**
- Dynamic memory allocation
- Heap management
- Memory fragmentation handling
- Memory statistics

**Key Functions:**
```c
void* memory_alloc(uint32_t size);
rtos_result_t memory_free(void* ptr);
uint32_t memory_get_free_size(void);
```

### 5. Timer Manager (Member 5)
**Files:** `timer_manager.h/c`

**Responsibilities:**
- System tick generation
- Software timers
- Interrupt handling
- Timing services

**Key Functions:**
```c
uint8_t timer_create(timer_type_t type, uint32_t period_ms, timer_callback_t callback, void* user_data);
rtos_result_t timer_start_timer(uint8_t timer_id);
uint32_t timer_get_uptime_ms(void);
```

### 6. System Integration (Member 6)
**Files:** `main.c`, examples, documentation

**Responsibilities:**
- System initialization
- Example applications
- Integration testing
- Documentation

## 💡 Example Applications

### LED Blinking Example
Demonstrates basic task creation and timing:
- Multiple LEDs with different blink rates
- Priority-based task execution
- System monitoring task

### Producer-Consumer Example
Shows inter-task communication:
- Producer task generates data
- Consumer task processes data
- Message queues for communication
- Semaphores for synchronization

## 🔍 Debugging and Testing

### Debug Features

1. **Debug Output**
   - Enable with `#define DEBUG` in rtos_config.h
   - View debug messages in Keil simulator console

2. **Task Information**
   ```c
   task_print_info(0xFF);    // Print all tasks
   scheduler_print_info();   // Print scheduler state
   memory_print_info();      // Print memory usage
   ```

3. **Statistics**
   - Task execution statistics
   - Memory usage tracking
   - Queue/semaphore status
   - Timer performance metrics

### Testing Checklist

- [ ] All modules compile without errors
- [ ] Tasks create and execute correctly
- [ ] Scheduler switches between tasks
- [ ] Queues send/receive messages
- [ ] Semaphores provide synchronization
- [ ] Memory allocation/deallocation works
- [ ] Timers expire and call callbacks
- [ ] System runs stably in simulation

## 📖 Learning Objectives

### For Students/Beginners

1. **RTOS Concepts**
   - Task management and scheduling
   - Inter-task communication
   - Synchronization mechanisms
   - Real-time constraints

2. **Embedded Programming**
   - ARM Cortex-M architecture
   - Interrupt handling
   - Memory management
   - Resource constraints

3. **Software Engineering**
   - Modular design
   - API design
   - Documentation
   - Team collaboration

## 🛠️ Extending the Project

### Possible Enhancements

1. **Additional Features**
   - Priority inheritance for semaphores
   - Event flags/groups
   - Software watchdog
   - Power management

2. **Hardware Integration**
   - GPIO control
   - UART communication
   - ADC sampling
   - PWM output

3. **Advanced Scheduling**
   - Deadline scheduling
   - Rate monotonic scheduling
   - Load balancing

## 📋 Individual Contribution Guidelines

### For Team Members

Each team member should:

1. **Understand the Interface**
   - Study the header file for your module
   - Understand data structures and function signatures
   - Identify dependencies on other modules

2. **Implement Core Functions**
   - Focus on the essential functionality first
   - Add error checking and validation
   - Include comprehensive comments

3. **Test Your Module**
   - Create simple test functions
   - Verify integration with other modules
   - Test error conditions

4. **Document Your Work**
   - Comment all functions and structures
   - Create usage examples
   - Document any limitations or assumptions

### Workflow Description

To describe individual contributions:

1. **Member 1 (Task Management)**: "Implemented task creation, deletion, and state management. Created Task Control Block structure and stack management functions."

2. **Member 2 (Scheduler)**: "Developed priority-based preemptive scheduler with round-robin support. Implemented context switching and ready queue management."

3. **Member 3 (Queue/Semaphore)**: "Created message queue system for inter-task communication and semaphore implementation for synchronization."

4. **Member 4 (Memory Management)**: "Implemented dynamic memory allocator with first-fit algorithm and coalescing of free blocks."

5. **Member 5 (Timer/Interrupt)**: "Developed system timer, software timers, and interrupt handling infrastructure."

6. **Member 6 (Integration)**: "Integrated all modules, created example applications, performed system testing, and wrote documentation."

## 🎓 Educational Value

This project provides hands-on experience with:

- **Real-time system concepts**
- **Embedded software design**
- **ARM Cortex-M programming**
- **Team-based development**
- **Professional documentation**

## 📞 Support and Resources

### Additional Resources

- ARM Cortex-M Programming Manual
- Keil uVision User Guide
- Real-Time Systems Design Principles
- FreeRTOS Documentation (for reference)

### Common Issues and Solutions

1. **Compilation Errors**
   - Check include paths in project settings
   - Verify all source files are added to project
   - Ensure proper header file inclusion

2. **Simulation Issues**
   - Verify target device selection (ARMCM3)
   - Check memory map configuration
   - Enable debug information in project settings

3. **Runtime Problems**
   - Use debugger to step through code
   - Check task stack sizes
   - Verify memory allocation/deallocation
   - Monitor queue and semaphore usage

---

**Project Status**: Educational RTOS Implementation  
**Compatibility**: Keil uVision (Free Version)  
**Target**: ARM Cortex-M3 Simulator  
**License**: Educational Use