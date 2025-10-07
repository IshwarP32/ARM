# Keil uVision Troubleshooting Guide

## How to Run This Project

### Step-by-Step Instructions:

1. **Open the Project**
   - Launch Keil uVision
   - File → Open Project
   - Select `ARM_RTOS_Scheduler.uvprojx`

2. **Build the Project**
   - Press `F7` or click the Build icon
   - Wait for "0 Error(s), X Warning(s)" message
   - Some warnings are expected (ARM-specific functions)

3. **Start Debug Session**
   - Press `Ctrl+F5` or click Debug icon
   - Wait for simulator to load

4. **Open Debug Output Window**
   - Go to `View → Serial Windows → Debug (printf) Viewer`
   - This window will show all program output

5. **Run the Program**
   - Press `F5` or click Run icon
   - Watch the Debug (printf) Viewer window
   - You should see tasks executing

6. **Expected Behavior**
   - Program will run for ~30 seconds
   - You'll see multiple tasks running
   - Program will print "Program completed successfully!"
   - Execution will stop automatically

## Common Issues and Solutions

### Issue 1: "Program Gets Stuck"

**Symptom:** Simulator running but no new output appears

**Old Code Problem:** This was the main issue - infinite loops in tasks

**Fixed In New Code:** ✅ No longer happens
- Tasks no longer have infinite loops
- Program completes after 1000 iterations
- All tasks get to run

**What to Check:**
- Make sure you're using the LATEST code from this PR
- Rebuild the project (F7) after pulling changes
- Check that scheduler_run_next_task() exists in scheduler.c

### Issue 2: "Build Errors"

**Symptom:** Compilation fails with errors

**Solution:**
1. Check that all source files are included:
   - src/main.c
   - src/scheduler.c
   - src/task_manager.c
   - src/memory_manager.c
   - src/queue_manager.c
   - src/timer_manager.c
   - src/arm_cortex_m.c

2. Verify include path:
   - Right-click project → Options
   - C/C++ tab
   - Include Paths should have: `./include`

3. Check device selection:
   - Should be ARMCM3 or similar Cortex-M3 device

### Issue 3: "No Debug Output"

**Symptom:** Can't see printf messages

**Solution:**
1. Open the correct window:
   - `View → Serial Windows → Debug (printf) Viewer`
   - NOT the normal output window

2. Check DEBUG is defined:
   - In rtos_config.h, line 113-114:
   ```c
   #ifndef DEBUG
   #define DEBUG  1
   #endif
   ```

3. Verify simulator is selected:
   - Project → Options → Debug tab
   - Use Simulator should be checked

### Issue 4: "Warning: Implicit Declaration"

**Symptom:** Warnings about __disable_irq, __enable_irq

**Solution:** These warnings are EXPECTED
- They're ARM Cortex-M specific functions
- Not available in standard GCC
- They work fine in Keil ARM compiler
- You can ignore these warnings

### Issue 5: "Tasks Don't Run"

**Symptom:** Only see initialization messages, no task output

**Old Code Problem:** Tasks not added to ready queue

**Fixed In New Code:** ✅ Tasks now properly registered
- task_create() calls scheduler_add_ready_task()
- All tasks are added to scheduler

**What to Check:**
1. Verify task_manager.c includes scheduler.h
2. Check that tasks are created in main():
   ```c
   task_create(task1_function, "Task1", PRIORITY_HIGH, 256);
   task_create(task2_function, "Task2", PRIORITY_MEDIUM, 256);
   task_create(task3_function, "Task3", PRIORITY_LOW, 256);
   ```

### Issue 6: "Program Runs Forever"

**Symptom:** Program keeps running, doesn't stop

**Expected Behavior:** Program SHOULD stop after 1000 iterations

**What to Check:**
1. Look for this message:
   ```
   [MAIN] Completed 1000 iterations - stopping for demonstration
   [MAIN] Program completed successfully!
   ```

2. If you DON'T see this, check main.c:
   ```c
   if(iteration >= 1000) {
       DEBUG_PRINT("\n[MAIN] Completed 1000 iterations...\n");
       break;
   }
   ```

3. If you WANT it to run forever:
   - Remove or comment out the `if(iteration >= 1000)` block
   - Or change 1000 to a higher number

## Understanding the Output

### Normal Output Should Look Like:

```
HELLO WORLD - BASIC TEST
Test 0 - Program is running
Test 1 - Program is running
Test 2 - Program is running
Test 3 - Program is running
Test 4 - Program is running
=== ARM RTOS Scheduler Starting ===
[INIT] Initializing system...
[SYS] Starting system initialization...
[SYS] ARM Cortex-M initialized
[SYS] SysTick configured
[SYS] Interrupt priorities set
[SYS] System initialized for ARM Cortex-M3 simulation
[INIT] Initializing memory manager...
Memory Manager initialized
[INIT] Initializing task manager...
Task Manager initialized
[INIT] Initializing queue manager...
Queue Manager initialized
[INIT] Initializing timer...
Timer initialized
[INIT] Initializing scheduler...
Task 'IDLE' created with ID 4, Priority 0
Scheduler initialized
[TASK] Creating tasks...
Task 'Task1' created with ID 0, Priority 3
Task 'Task2' created with ID 1, Priority 2
Task 'Task3' created with ID 2, Priority 1
[SCHED] Starting RTOS scheduler...
Scheduler started with first task: Task1 (ID: 0)
[MAIN] Entering scheduler loop...
[MAIN] This is a SIMPLIFIED scheduler - tasks run one iteration at a time
[TASK1] High priority task started
[TASK1] Running - Counter: 1
[TASK2] Medium priority task started
[TASK2] No data in queue
[TASK3] Low priority background task started
[TASK3] Background task running (iteration 1)...
[TASK1] Running - Counter: 2
...
[MAIN] Scheduler iteration 50 completed
=== Scheduler Information ===
Running: Yes
Locked: No
Context Switches: 150
...
[MAIN] Completed 1000 iterations - stopping for demonstration
[MAIN] In a real system, this would run indefinitely
[MAIN] Program completed successfully!
```

### What Each Section Means:

1. **Basic Test** - Verifies program starts
2. **Initialization** - All RTOS components initialize
3. **Task Creation** - Three tasks created
4. **Scheduler Start** - Scheduler begins
5. **Task Execution** - Tasks run in round-robin
6. **Status Updates** - Periodic scheduler info
7. **Completion** - Program finishes successfully

## Debugging Tips

### Use Breakpoints:
1. Click in left margin to set breakpoint
2. Run with F5
3. Program stops at breakpoint
4. Press F11 to step through code

### Watch Variables:
1. View → Watch Windows → Watch 1
2. Add variables to watch:
   - `iteration`
   - `test_counter`
   - Task static variables

### Useful Breakpoints:
- Line in main(): `scheduler_run_next_task()`
- Line in scheduler.c: `next_task->task_function()`
- Line in task functions to see when they execute

## Performance Notes

### Simulation Speed:
- Simulation is slower than real hardware
- Output may appear gradually
- Be patient, let it run for 30-60 seconds

### Output Rate:
- Reduced output frequency to avoid overwhelming simulator
- Status prints every 50 iterations
- Adjust in main.c if needed:
  ```c
  if(iteration % 50 == 0) {  // Change 50 to adjust frequency
  ```

## Getting Help

If you're still having issues:

1. **Check the documentation:**
   - QUICK_REFERENCE.md - Visual examples
   - SIMPLIFIED_APPROACH.md - Detailed explanation
   - CHANGES_SUMMARY.md - What was changed

2. **Verify your code:**
   - Make sure you have the latest changes
   - Rebuild from scratch (clean + build)
   - Check git status to see if you have uncommitted changes

3. **Common mistakes:**
   - Using old code with infinite loops in tasks
   - Not opening Debug (printf) Viewer window
   - Wrong device selected (should be ARMCM3)
   - Include paths not set correctly

## Success Indicators

✅ You'll know it's working when:
1. Build completes with 0 errors
2. Debug session starts without issues
3. Debug (printf) Viewer shows output
4. You see multiple tasks running
5. Program prints completion message
6. Execution stops automatically

## Compare with Old Code

### Old Behavior (BROKEN):
- Runs for a bit then stops outputting
- Appears to hang
- Only first task runs
- Must manually stop simulation

### New Behavior (FIXED):
- Runs smoothly
- All tasks execute
- Regular output appears
- Completes with success message
- Stops automatically

---

**If you see "Program completed successfully!" - IT WORKS! 🎉**
