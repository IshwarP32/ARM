/**
 * @file project_guide.md
 * @brief Comprehensive Project Guide for Team Members
 * @author Team Leader
 * @date 2024
 */

# ARM RTOS Scheduler - Team Project Guide

## 🎯 Project Goals and Learning Outcomes

### Primary Objectives
1. **Understand RTOS Fundamentals**: Learn core concepts of real-time operating systems
2. **Practice Embedded Programming**: Work with ARM Cortex-M architecture and Keil uVision
3. **Team Collaboration**: Experience modular software development in a team environment
4. **Professional Documentation**: Create industry-standard documentation and code comments

### Learning Outcomes
By the end of this project, each team member will be able to:
- Explain RTOS concepts (tasks, scheduling, synchronization)
- Implement modular embedded software components
- Use professional development tools (Keil uVision, debugger)
- Write well-documented, maintainable code
- Collaborate effectively in a software development team

## 👥 Detailed Team Responsibilities

### Team Member 1: Task Management
**Primary Module**: Task Manager (`task_manager.h/c`)

**Core Responsibilities**:
1. **Task Control Block (TCB) Design**
   - Define task data structure
   - Include task ID, name, priority, state
   - Manage stack pointers and stack size
   - Track execution statistics

2. **Task Lifecycle Management**
   - Implement task creation function
   - Handle task deletion and cleanup
   - Manage task state transitions
   - Implement task delay functionality

3. **Stack Management**
   - Initialize task stacks
   - Handle stack overflow detection
   - Manage stack allocation/deallocation

**Key Deliverables**:
- Complete `task_manager.h` interface
- Implement all functions in `task_manager.c`
- Create unit tests for task operations
- Document TCB structure and usage

**Interface Functions to Implement**:
```c
rtos_result_t task_manager_init(void);
uint8_t task_create(void (*task_function)(void), const char* task_name, uint8_t priority, uint32_t stack_size);
rtos_result_t task_delete(uint8_t task_id);
void task_delay(uint32_t delay_ticks);
tcb_t* task_get_tcb(uint8_t task_id);
```

---

### Team Member 2: Scheduler Implementation
**Primary Module**: Scheduler (`scheduler.h/c`)

**Core Responsibilities**:
1. **Priority-Based Scheduling**
   - Implement priority queue management
   - Handle preemptive scheduling decisions
   - Manage ready task queues by priority level

2. **Round-Robin Scheduling**
   - Implement time-slice management
   - Handle round-robin within same priority
   - Manage task time quantum

3. **Context Switching**
   - Implement task switching logic
   - Handle scheduler invocation points
   - Manage scheduler statistics

**Key Deliverables**:
- Complete `scheduler.h` interface
- Implement scheduling algorithms in `scheduler.c`
- Create scheduler performance tests
- Document scheduling policies

**Interface Functions to Implement**:
```c
rtos_result_t scheduler_init(void);
void scheduler_start(void);
tcb_t* scheduler_get_next_task(void);
void scheduler_switch_context(void);
void scheduler_tick(void);
```

---

### Team Member 3: Queue/Semaphore Management
**Primary Module**: Queue Manager (`queue_manager.h/c`)

**Core Responsibilities**:
1. **Message Queue Implementation**
   - Implement circular buffer queues
   - Handle queue full/empty conditions
   - Manage blocking/non-blocking operations
   - Implement timeout handling

2. **Semaphore Implementation**
   - Binary and counting semaphores
   - Handle semaphore take/give operations
   - Manage waiting task lists
   - Implement priority inheritance (optional)

3. **Inter-Task Communication**
   - Provide reliable message passing
   - Handle synchronization primitives
   - Manage resource sharing

**Key Deliverables**:
- Complete `queue_manager.h` interface
- Implement queues and semaphores in `queue_manager.c`
- Create communication examples
- Document synchronization patterns

**Interface Functions to Implement**:
```c
rtos_result_t queue_manager_init(void);
queue_result_t queue_create(uint8_t queue_id, uint32_t size);
queue_result_t queue_send(uint8_t queue_id, const void* data, uint32_t timeout_ms);
queue_result_t queue_receive(uint8_t queue_id, void* data, uint32_t timeout_ms);
rtos_result_t semaphore_create(uint8_t semaphore_id, uint8_t initial_count, uint8_t max_count);
rtos_result_t semaphore_take(uint8_t semaphore_id, uint32_t timeout_ms);
rtos_result_t semaphore_give(uint8_t semaphore_id);
```

---

### Team Member 4: Memory Management
**Primary Module**: Memory Manager (`memory_manager.h/c`)

**Core Responsibilities**:
1. **Dynamic Memory Allocation**
   - Implement heap management
   - First-fit allocation algorithm
   - Handle memory fragmentation
   - Implement coalescing of free blocks

2. **Memory Protection**
   - Validate memory operations
   - Detect memory corruption
   - Handle out-of-memory conditions
   - Implement memory statistics

3. **Performance Optimization**
   - Minimize allocation/deallocation time
   - Reduce memory fragmentation
   - Provide memory usage statistics

**Key Deliverables**:
- Complete `memory_manager.h` interface
- Implement allocator in `memory_manager.c`
- Create memory stress tests
- Document memory management policies

**Interface Functions to Implement**:
```c
rtos_result_t memory_init(void);
void* memory_alloc(uint32_t size);
rtos_result_t memory_free(void* ptr);
void* memory_realloc(void* ptr, uint32_t new_size);
uint32_t memory_get_free_size(void);
rtos_result_t memory_check_integrity(void);
```

---

### Team Member 5: Timer/Interrupt Management
**Primary Module**: Timer Manager (`timer_manager.h/c`)

**Core Responsibilities**:
1. **System Timer Implementation**
   - Configure ARM SysTick timer
   - Handle timer interrupts
   - Maintain system tick counter
   - Provide timing services

2. **Software Timers**
   - Implement one-shot and periodic timers
   - Handle timer expiration callbacks
   - Manage timer lists efficiently
   - Provide high-resolution timing

3. **Interrupt Management**
   - Handle interrupt enable/disable
   - Manage interrupt priorities
   - Provide critical section support

**Key Deliverables**:
- Complete `timer_manager.h` interface
- Implement timers in `timer_manager.c`
- Create timing accuracy tests
- Document timer usage patterns

**Interface Functions to Implement**:
```c
rtos_result_t timer_init(void);
rtos_result_t timer_start(void);
uint32_t timer_get_ticks(void);
uint8_t timer_create(timer_type_t type, uint32_t period_ms, timer_callback_t callback, void* user_data);
rtos_result_t timer_start_timer(uint8_t timer_id);
void timer_interrupt_handler(void);
```

---

### Team Member 6: System Integration
**Primary Module**: System Integration (`main.c`, examples, docs)

**Core Responsibilities**:
1. **System Initialization**
   - Coordinate module initialization
   - Handle system startup sequence
   - Implement main application logic
   - Manage system configuration

2. **Example Applications**
   - Create LED blinking example
   - Implement producer-consumer example
   - Develop comprehensive test cases
   - Demonstrate RTOS features

3. **Integration Testing**
   - Test inter-module communication
   - Verify system performance
   - Handle error conditions
   - Create integration test suite

4. **Documentation**
   - Write comprehensive README
   - Create user guides and tutorials
   - Document API usage examples
   - Maintain project documentation

**Key Deliverables**:
- Complete `main.c` implementation
- Create example applications
- Comprehensive project documentation
- Integration test results

## 📋 Weekly Milestones

### Week 1: Design and Planning
**All Members**:
- [ ] Study assigned module interface
- [ ] Understand dependencies between modules
- [ ] Create detailed implementation plan
- [ ] Set up development environment

### Week 2: Core Implementation
**Individual Tasks**:
- [ ] Implement basic module functionality
- [ ] Create unit tests for core functions
- [ ] Document implementation decisions
- [ ] Review interface with other team members

### Week 3: Integration and Testing
**Team Activities**:
- [ ] Integrate all modules
- [ ] Resolve integration issues
- [ ] Test complete system functionality
- [ ] Performance optimization

### Week 4: Examples and Documentation
**Final Deliverables**:
- [ ] Complete example applications
- [ ] Finalize documentation
- [ ] Prepare demonstration
- [ ] Code review and cleanup

## 🧪 Testing Strategy

### Unit Testing (Individual)
Each team member should:
1. Test their module independently
2. Verify all function parameters and return values
3. Test error conditions and edge cases
4. Validate performance requirements

### Integration Testing (Team)
The team should collectively:
1. Test module interactions
2. Verify system-level functionality
3. Test complete user scenarios
4. Validate timing and performance

### Test Cases by Module

**Task Manager**:
- Create maximum number of tasks
- Test task state transitions
- Verify stack overflow detection
- Test task delay accuracy

**Scheduler**:
- Verify priority-based scheduling
- Test round-robin within priorities
- Validate context switch timing
- Test scheduler statistics

**Queue/Semaphore**:
- Test queue full/empty conditions
- Verify semaphore counting
- Test timeout mechanisms
- Validate blocking operations

**Memory Manager**:
- Test allocation/deallocation cycles
- Verify memory fragmentation handling
- Test out-of-memory conditions
- Validate memory corruption detection

**Timer Manager**:
- Test timer accuracy
- Verify callback execution
- Test timer overflow conditions
- Validate interrupt handling

## 📊 Individual Assessment Criteria

### Technical Implementation (40%)
- Correctness of implementation
- Code quality and style
- Error handling and robustness
- Performance optimization

### Documentation (20%)
- Code comments and clarity
- API documentation
- Usage examples
- Technical explanations

### Testing (20%)
- Unit test coverage
- Test case quality
- Bug finding and fixing
- Integration support

### Collaboration (20%)
- Interface compliance
- Team communication
- Meeting participation
- Code review contributions

## 🛠️ Development Guidelines

### Coding Standards
1. **Naming Conventions**
   - Functions: `module_verb_noun()` (e.g., `task_create()`)
   - Variables: `snake_case` (e.g., `task_count`)
   - Constants: `UPPER_CASE` (e.g., `MAX_TASKS`)
   - Types: `snake_case_t` (e.g., `task_state_t`)

2. **Code Structure**
   - Include file guards for all headers
   - Clear separation of public/private functions
   - Consistent indentation (4 spaces)
   - Maximum 80 characters per line

3. **Documentation**
   - Doxygen-style comments for all functions
   - Explain complex algorithms
   - Include usage examples
   - Document assumptions and limitations

### Git Workflow
1. **Branch Management**
   - Create feature branches for each module
   - Regular commits with descriptive messages
   - Merge only tested and reviewed code

2. **Commit Messages**
   - Format: `[MODULE] Brief description`
   - Examples:
     - `[TASK] Implement task creation function`
     - `[SCHED] Add priority-based scheduling`
     - `[QUEUE] Fix queue overflow handling`

## 📈 Success Metrics

### Project Success Indicators
- All modules integrate successfully
- System runs stably in simulation
- Example applications demonstrate RTOS features
- Code passes all test cases
- Documentation is comprehensive and clear

### Individual Success Indicators
- Module implements all required functions
- Code compiles without warnings
- Unit tests pass with good coverage
- Documentation explains implementation clearly
- Active participation in team activities

## 🎓 Learning Resources

### Recommended Reading
1. **RTOS Fundamentals**
   - "Real-Time Systems" by Jane Liu
   - "MicroC/OS-III" by Jean Labrosse
   - ARM Cortex-M Programming Guide

2. **Embedded Programming**
   - "Programming Embedded Systems" by Barr & Massa
   - ARM Cortex-M3 Technical Reference Manual
   - Keil uVision User Guide

3. **Software Engineering**
   - "Clean Code" by Robert Martin
   - "Code Complete" by Steve McConnell
   - Doxygen documentation guide

### Online Resources
- ARM Developer Documentation
- Keil Application Notes
- FreeRTOS Documentation (for reference)
- Embedded Systems Programming blogs

## 🚀 Getting Started Checklist

### For Each Team Member
- [ ] Read and understand this project guide
- [ ] Study your assigned module interface
- [ ] Set up Keil uVision development environment
- [ ] Clone project repository and explore structure
- [ ] Attend team kickoff meeting
- [ ] Create personal development plan
- [ ] Start implementation of core functions

### Team Activities
- [ ] Hold project kickoff meeting
- [ ] Assign specific responsibilities
- [ ] Establish communication channels
- [ ] Set up regular team meetings
- [ ] Create shared development timeline
- [ ] Establish code review process

---

**Remember**: This is a learning project. Don't hesitate to ask questions, help team members, and iterate on your implementation. The goal is to understand RTOS concepts deeply while building a working system together!

**Contact**: Reach out to team lead or instructor for guidance on any aspect of the project.