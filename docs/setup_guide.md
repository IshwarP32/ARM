/**
 * @file setup_guide.md
 * @brief Step-by-step setup guide for the ARM RTOS Scheduler project
 * @author Team Member 6 - System Integration
 * @date 2024
 */

# ARM RTOS Scheduler - Setup Guide

## 🚀 Quick Start Guide

### Step 1: Install Keil uVision
1. Download Keil uVision from: https://www.keil.com/download/product/
2. Install the free version (MDK-Arm)
3. The free version supports up to 32KB code size (sufficient for this project)
4. Register for a free license when prompted

### Step 2: Open the Project
1. Launch Keil uVision IDE
2. Open project file: `ARM_RTOS_Scheduler.uvprojx`
3. The project should load with all source files

### Step 3: Build the Project
1. Press `F7` or go to `Project → Build Target`
2. Check the build output for any errors
3. The project should compile successfully

### Step 4: Run in Simulator
1. Press `Ctrl+F5` or go to `Debug → Start/Stop Debug Session`
2. The simulator will start
3. Press `F5` to run the program
4. View debug output in the "Debug (printf) Viewer" window

## 📁 Project Structure Overview

```
ARM_RTOS_Scheduler/
├── src/                           # Source files (.c)
│   ├── main.c                    # Main application
│   ├── task_manager.c            # Task management
│   ├── scheduler.c               # Scheduler implementation
│   ├── queue_manager.c           # Queues and semaphores
│   ├── memory_manager.c          # Memory management
│   ├── timer_manager.c           # Timer services
│   └── arm_cortex_m.c           # ARM-specific code
├── include/                       # Header files (.h)
│   ├── rtos_config.h            # Configuration
│   ├── task_manager.h           # Task management API
│   ├── scheduler.h              # Scheduler API
│   ├── queue_manager.h          # Queue/semaphore API
│   ├── memory_manager.h         # Memory management API
│   ├── timer_manager.h          # Timer API
│   └── arm_cortex_m.h          # ARM-specific definitions
├── examples/                      # Example applications
│   ├── led_blink_example.c      # LED blinking demo
│   └── producer_consumer_example.c # Inter-task communication demo
├── docs/                          # Documentation
│   ├── project_guide.md         # Team project guide
│   └── setup_guide.md           # This file
├── ARM_RTOS_Scheduler.uvprojx    # Keil project file
└── README.md                     # Main project documentation
```

## 🛠️ Keil uVision Configuration

### Project Settings Verification

1. **Target Device**: ARM Cortex-M3 (Generic)
2. **Compiler**: ARM Compiler v5 or v6
3. **Include Paths**: `./include` should be added
4. **Preprocessor Defines**: `DEBUG` should be defined
5. **Code Generation**: Thumb mode enabled

### Debug Configuration

1. **Debug Adapter**: Use Simulator
2. **Target**: Cortex-M3
3. **Memory Map**: 
   - ROM: 0x00000000, Size: 0x40000 (256KB)
   - RAM: 0x20000000, Size: 0x20000 (128KB)

### Viewing Debug Output

1. Go to `View → Serial Windows → Debug (printf) Viewer`
2. This window will show all DEBUG_PRINT() output
3. You can also use `View → Watch Windows` to monitor variables

## 🧪 Testing the Setup

### Basic Functionality Test

1. Build and run the project
2. You should see debug output showing:
   - System initialization messages
   - Task creation messages
   - RTOS scheduler starting
   - Task execution messages

### Expected Output

```
Memory Manager initialized with 4096 bytes heap
Task Manager initialized
Queue Manager initialized
Timer Manager initialized
Scheduler initialized
ARM Cortex-M initialized
System initialized for ARM Cortex-M3 simulation
Task 'Task1' created with ID 0, Priority 3
Task 'Task2' created with ID 1, Priority 2
Task 'Task3' created with ID 2, Priority 1
Starting scheduler...
First task: Task1 (ID: 0)
```

## 🔧 Troubleshooting

### Common Issues and Solutions

#### 1. Build Error: "Cannot find argument 'Reset_Handler'"

**Error Message:**
```
.\Objects\ARM_RTOS_Scheduler.axf: Error: L6320W: Ignoring --entry command. Cannot find argument 'Reset_Handler'.
.\Objects\ARM_RTOS_Scheduler.axf: Warning: L6320W: Ignoring --first command. Cannot find argument '__Vectors'.
```

**Solution:**
This error occurs when the ARM Cortex-M startup file is missing or not properly configured.

✅ **Fixed in current version**: The project now includes:
- `startup_ARMCM3.s` - ARM Cortex-M3 startup assembly file
- `system_ARMCM3.c` - System initialization functions
- Updated project configuration to include these files

If you encounter this error:
1. Ensure `startup_ARMCM3.s` and `system_ARMCM3.c` are in the project root
2. Verify they are included in the project build (should be in "Startup" and "Source Files" groups)
3. Check that the project target is set to ARM Cortex-M3

#### 2. "Device not found" Error
**Problem**: Keil cannot find the target device
**Solution**: 
- Check that ARMCM3 device is selected
- Install ARM CMSIS pack if missing
- Use `Pack Installer` to add required packs

#### 2. Compilation Errors
**Problem**: Missing include files or syntax errors
**Solution**:
- Verify include path is set to `./include`
- Check that all source files are added to project
- Ensure C99 standard is enabled in compiler options

#### 3. No Debug Output
**Problem**: printf output not visible
**Solution**:
- Open `View → Serial Windows → Debug (printf) Viewer`
- Ensure DEBUG is defined in preprocessor settings
- Check that ITM or semihosting is enabled

#### 4. Simulation Crashes
**Problem**: Program crashes or stops unexpectedly
**Solution**:
- Check stack sizes are sufficient
- Verify memory allocation/deallocation
- Use debugger to identify crash location

### Advanced Debugging

#### Using the Debugger

1. **Set Breakpoints**: Click in the left margin of source code
2. **Step Through Code**: Use F10 (step over) and F11 (step into)
3. **Watch Variables**: Add variables to watch window
4. **Memory Viewer**: Check memory contents and stack usage

#### Performance Analysis

1. **Execution Profiling**: Use `Debug → Performance Analysis`
2. **Code Coverage**: Enable code coverage analysis
3. **Trace**: Use instruction trace for detailed analysis

## 📖 Understanding the Code

### Module Dependencies

```
main.c
├── task_manager (creates and manages tasks)
├── scheduler (decides which task runs)
├── queue_manager (inter-task communication)
├── memory_manager (dynamic memory allocation)
├── timer_manager (timing services)
└── arm_cortex_m (hardware abstraction)
```

### Key Concepts

1. **Task**: Independent unit of execution
2. **Scheduler**: Decides which task runs when
3. **Queue**: Message passing between tasks
4. **Semaphore**: Synchronization primitive
5. **Timer**: Periodic or one-shot callbacks

### Code Flow

1. `main()` initializes all subsystems
2. Creates example tasks
3. Starts the scheduler
4. Scheduler manages task execution
5. Tasks communicate via queues and semaphores

## 🎯 Next Steps

### For Learning

1. **Study the Code**: Read through each module to understand RTOS concepts
2. **Modify Examples**: Change task priorities, timing, or behavior
3. **Add Features**: Implement additional RTOS features
4. **Create Applications**: Build your own tasks and examples

### For Development

1. **Code Review**: Review each team member's implementation
2. **Testing**: Create comprehensive test cases
3. **Documentation**: Add detailed comments and documentation
4. **Optimization**: Improve performance and memory usage

## 📚 Additional Resources

### Keil uVision Help

- Built-in help: `Help → uVision Help`
- User guides available in installation directory
- Online documentation: https://www.keil.com/support/docs/

### ARM Cortex-M Resources

- ARM Cortex-M3 Technical Reference Manual
- ARM Architecture Reference Manual
- Joseph Yiu's "The Definitive Guide to ARM Cortex-M3 and Cortex-M4 Processors"

### RTOS Learning Resources

- FreeRTOS documentation (for concepts)
- "MicroC/OS-III" book by Jean Labrosse
- Real-time systems design principles

---

**Support**: If you encounter issues not covered in this guide, please:
1. Check the Keil documentation
2. Search online forums and communities
3. Ask your instructor or team lead
4. Review the project documentation

**Happy coding!** 🚀