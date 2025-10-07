# Quick Reference - What Changed and Why

## Problem: Program Gets Stuck in Keil uVision ❌

**Original Code:**
- Tasks had `while(1)` infinite loops
- Scheduler called first task function directly
- First task never returned
- Other tasks never got to run
- Program appeared to hang

## Solution: Simplified Cooperative Scheduler ✅

### 3 Main Changes:

## 1. Task Functions - No More Infinite Loops

```c
// OLD (BROKEN) ❌
void task1_function(void) {
    uint32_t counter = 0;
    while(1) {                    // INFINITE LOOP - NEVER RETURNS!
        counter++;
        printf("Count: %d\n", counter);
        task_delay(100);
    }
}

// NEW (WORKING) ✅
void task1_function(void) {
    static uint32_t counter = 0;  // 'static' keeps value between calls
    
    counter++;                    // Do work
    printf("Count: %d\n", counter);
    
    if(counter % 5 == 0) {
        task_delay(100);
    }
    
    // FUNCTION RETURNS - other tasks can run!
}
```

**Why `static`?**
- Normal variables reset each time function is called
- `static` variables keep their value between calls
- Allows tasks to remember state without `while(1)`

---

## 2. Main Loop - Cooperative Scheduling

```c
// OLD (BROKEN) ❌
int main(void) {
    // ... initialization ...
    scheduler_start();  // Calls first task's while(1) - NEVER RETURNS!
    
    while(1) {
        // NEVER REACHED!
    }
}

// NEW (WORKING) ✅
int main(void) {
    // ... initialization ...
    scheduler_start();  // Just sets up scheduler, then returns
    
    uint32_t iteration = 0;
    while(iteration < 1000) {     // Runs 1000 times then stops
        iteration++;
        
        if(iteration % 10 == 0) {
            task_update_delays();  // Handle task delays
        }
        
        scheduler_run_next_task(); // Run ONE task for ONE iteration
        
        if(iteration % 50 == 0) {
            scheduler_print_info(); // Show status
        }
    }
    
    printf("Program completed successfully!\n");
    return 0;
}
```

**How it works:**
1. Loop runs 1000 times
2. Each iteration runs ONE task for ONE iteration
3. Tasks take turns (round-robin)
4. Program completes and shows success message

---

## 3. Scheduler - Run One Task at a Time

**NEW function added:** `scheduler_run_next_task()`

```c
void scheduler_run_next_task(void) {
    // 1. Find next ready task (highest priority)
    tcb_t* next_task = scheduler_get_next_task();
    
    if(next_task == NULL) {
        scheduler_idle_task();  // No tasks? Run idle
        return;
    }
    
    // 2. Set task as running
    task_set_state(next_task->task_id, TASK_STATE_RUNNING);
    
    // 3. Call task function ONCE
    next_task->task_function();
    
    // 4. Set back to ready
    if(next_task->state == TASK_STATE_RUNNING) {
        next_task->state = TASK_STATE_READY;
    }
    
    // 5. Move to next task in round-robin
    scheduler_round_robin_next(next_task->priority);
}
```

---

## How to Run in Keil uVision

1. **Build** (F7) - Compile the code
2. **Start Debug** (Ctrl+F5) - Enter debug mode
3. **View → Serial Windows → Debug (printf) Viewer** - Open output window
4. **Run** (F5) - Execute the program
5. **Watch Output** - See all tasks running!

### Expected Output:

```
HELLO WORLD - BASIC TEST
Test 0 - Program is running
Test 1 - Program is running
...
=== ARM RTOS Scheduler Starting ===
[INIT] Initializing system...
[INIT] Initializing memory manager...
[INIT] Initializing task manager...
...
[SCHED] Starting RTOS scheduler...
[MAIN] Entering scheduler loop...
[TASK1] High priority task started
[TASK1] Running - Counter: 1
[TASK2] Medium priority task started
[TASK2] No data in queue
[TASK3] Low priority background task started
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

---

## Understanding Task Execution

### Round-Robin Example:
```
Iteration 1:  Task 1 runs (counter = 1)
Iteration 2:  Task 2 runs (checks queue)
Iteration 3:  Task 3 runs (iteration 1)
Iteration 4:  Task 1 runs (counter = 2)
Iteration 5:  Task 2 runs (receives data)
Iteration 6:  Task 3 runs (iteration 2)
...and so on...
```

### Task Delays:
When a task calls `task_delay(100)`:
- Task is marked as BLOCKED
- Scheduler skips this task for ~100 iterations
- After delay expires, task becomes READY again
- Task resumes on next turn

---

## Key Points for Beginners

### ✅ Do This:
- Use `static` variables in task functions
- Let task functions return (no `while(1)`)
- One unit of work per task call
- Trust the scheduler to call you again

### ❌ Don't Do This:
- Don't use `while(1)` in task functions
- Don't use local variables for state (use `static`)
- Don't try to run task forever
- Don't block the scheduler

---

## Comparison with Real RTOS

| Feature | This Code | Real RTOS (e.g., FreeRTOS) |
|---------|-----------|---------------------------|
| Task Loops | ❌ No `while(1)` | ✅ Yes, infinite loops OK |
| Scheduling | Cooperative | Preemptive |
| Context Switch | Function call | Save/restore CPU registers |
| Timing | Approximate | Precise real-time |
| Interrupts | No | Yes |
| Use Case | **Learning** | **Production** |

---

## Next Steps

1. **Run the code** - See it work without getting stuck!
2. **Read SIMPLIFIED_APPROACH.md** - Detailed explanation
3. **Read CHANGES_SUMMARY.md** - Complete list of changes
4. **Experiment** - Try modifying task behaviors
5. **Learn more** - Study real RTOS (FreeRTOS documentation)

---

## Questions?

**Q: Why does it stop after 1000 iterations?**
A: For demonstration. Change `iteration < 1000` to `iteration < 10000` for more, or remove the limit for infinite.

**Q: Can I add more tasks?**
A: Yes! Follow the pattern in existing tasks. Use `static` variables, do one unit of work, return.

**Q: Is this how real RTOS works?**
A: No. Real RTOS use timer interrupts and preemptive scheduling. This is simplified for learning.

**Q: Can I use this in a real product?**
A: No! Use established RTOS like FreeRTOS, Zephyr, or CMSIS-RTOS for production.

---

## Summary

✅ **Fixed:** Program no longer gets stuck
✅ **Simplified:** Easy to understand for beginners
✅ **Working:** All tasks execute and program completes
✅ **Educational:** Good introduction to RTOS concepts

**The code now runs successfully in Keil uVision!** 🎉
