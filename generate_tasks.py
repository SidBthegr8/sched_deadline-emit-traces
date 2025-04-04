#!/usr/bin/env python3
import random
import sys

def generate_tasks(utilization, num_tasks):
    tasks = []
    
    for i in range(num_tasks):
        period = random.randint(100, 1000)
        wcet = period * (utilization / num_tasks)
        deadline = period
        tasks.append((i, period, deadline, wcet))
    
    return tasks

def main():
    if len(sys.argv) != 3:
        print("Usage: ./generate_tasks.py <utilization> <num_tasks>")
        sys.exit(1)
    
    # Parse command line arguments
    try:
        utilization = float(sys.argv[1])
        num_tasks = int(sys.argv[2])
    except ValueError:
        print("Error: Utilization must be a float and num_tasks must be an integer.")
        sys.exit(1)
    tasks = generate_tasks(utilization, num_tasks)
    for task in tasks:
        print(task)

if __name__ == "__main__":
    main()
