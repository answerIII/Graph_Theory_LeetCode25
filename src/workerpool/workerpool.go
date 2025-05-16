package workerpool

import (
	"sync"
)

type Task func() error

type WorkerPool struct {
	tasks chan Task
	wg    sync.WaitGroup
}

// NewWorkerPool creates a new WorkerPool with the specified number of workers and task queue size.
func NewWorkerPool(workers, queueTaskSize int) *WorkerPool {
	wp := &WorkerPool{
		tasks: make(chan Task, queueTaskSize),
	}
	for i := 0; i < workers; i++ {
		go wp.worker()
	}
	return wp
}

// worker is a goroutine that processes tasks from the task channel.
func (wp *WorkerPool) worker() {
	for task := range wp.tasks {
		_ = task()
		wp.wg.Done()
	}
}

// Submit adds a task to the worker pool. It increments the WaitGroup counter.
func (wp *WorkerPool) Submit(task Task) {
	wp.wg.Add(1)
	wp.tasks <- task
}

// Wait blocks until all tasks have been completed.
func (wp *WorkerPool) Wait() {
	wp.wg.Wait()
}

// Shutdown closes the task channel.
func (wp *WorkerPool) Shutdown() {
	close(wp.tasks)
}
