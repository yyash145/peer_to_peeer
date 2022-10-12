socket() creates  an endpoint for communication and returns a file descriptor that refers to that endpoint. The file descriptor returned by a  successful call will be the lowest-numbered file descriptor not currently open for the process.

On success, a file descriptor for the new socket is returned. On error, -1 is returned, and errno is set appropriately.

POSIX.1 does  not require the inclusion of <sys/types.h>, and this header file is not required on Linux. However, some historical (BSD) implementations required this header file, and portable applications are probably wise to include it.

A port number is a way to identify a specific process to which an internet or other network message is to be forwarded when it arrives at a server. All network-connected devices come equipped with standardized ports that have an assigned number.

