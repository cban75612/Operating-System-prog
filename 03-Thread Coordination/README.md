Operating System Prog3

A synchronized production simulation system based on the Producer-Consumer Model, managing concurrent resource allocation among multiple worker threads.

Thread Synchronization: Employed Pthread Mutex and Critical Sections to ensure thread-safe access to a shared resource buffer (Supply Front-desk).

Dynamic Scheduling: Implemented a Dispatcher logic that intelligently monitors buffer states to prevent redundant resource supply, coordinating with 3 concurrent production stations.

Concurrency Control: Designed a multi-producer architecture to simulate complex factory automation and resolve potential deadlocks during synchronized resource picking.
