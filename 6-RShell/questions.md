1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client can determine when a command's output is fully received from the server by using techniques such like:
- **End-of-message markers**: The server can append a special marker at the end of the command output to signal completion
- **Fixed-length messages**: The server can send a fixed number of bytes for each message, and the client reads exactly that many bytes
- **Length-prefix framing**: The server can send the length of the message before the actual message, allowing the client to know how many bytes to expect
- **Timeouts**: The client can use timeouts to determine if no more data is being sent after a certain period

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol can define and detect the beginning and end of a command sent over a TCP connection by using:
- **Message delimiters**: Special characters or sequences that indicate the start and end of a message
- **Length-prefix framing**: Including the length of the message at the beginning so the receiver knows how many bytes to read
- **Application-level protocols**: Defining a protocol that specifies how messages are structured and parsed

Challenges that arise if this is not handled correctly include:
- **Message fragmentation**: Partial messages being received and misinterpreted
- **Message concatenation**: Multiple messages being received as a single message
- **Data corruption**: Incorrectly parsing messages leading to corrupted data or commands

3. Describe the general differences between stateful and stateless protocols.

- **Stateful protocols**: Maintain state information across multiple requests. Each request is dependent on the previous ones, and the server keeps track of the client's state. Examples include FTP and HTTP with sessions
- **Stateless protocols**: Do not maintain state information between requests. Each request is independent and contains all the information needed for the server to process it. Examples include HTTP without sessions and UDP

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

Despite being "unreliable", UDP is used because it has lower latency compared to TCP because it does not require connection establishment or acknowledgment of packets