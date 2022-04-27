its own id and memory space
kernal ressign memory for the process
isolated, independent -> does not know the info of other process
similarly we need common rules, process A and B need to know the status of each other.. to enable it we need common memory space
this memory space cannot be accessed by others so we need some common memory space so that processes can access..

IPC -> SIGCHLD, PIPE(called ), FIFOs, Semaphores..

everything is file descriptor in unix, linux..

PIPE is the same.. temporary data..

unidirectional or bidirectional....

PIPE is usually unidirectional..

relantionship between two processes must exit..

PIPE only be used by relatives..

InterProcessCommunication

unnamed pipe(used by relative) vs.. 
named pipe(can be used by any processes)

Stream pipes, FIFOs(other variations)..?

process A and B there exits a PIPE..

pipe() -> two file descriptors
can exit as long as the processes use them

for unidirectional pipe..
parameter is array inclnuding two elements.
return type is integer.. -1 for failure 0 on success

pipefd[1] is write end of the pipe.
pipefd[0] is read end of the pipe.
pipe system call will return two integer file descriptors, pipefd[0] and pipefd[0]

one PIPE is shared 

PIPE who sent have no idea..
.........

input | output -> separated by two PIPEs
------------
I/O multiplexing

Input and output such as read and write if there are multiple clinents and wanna write to server

how to use child processes to handle multiple processes

two clients......
ten child processes one for each client
another way is use the function select..

more efficient to use select instead of multi process..
one reason is that each process we need memoery,so we can save the resource........

int select(10 clients..)

timeout or if there is event, execute..

ten file descriptor.. need to be handled by kernel

functionalto be triggered only by event timeout shoudl be set to NULL.....

timeout.. timeeval struct

fd_set -> binary array(0 or 1 only)
	readset
	fd set






