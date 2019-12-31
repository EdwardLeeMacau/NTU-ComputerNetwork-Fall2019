# Homework 3

1. True or false
    1. A user requests a Web page that consists of some text and three images. For this page, the client will send one request message and receive four response messages.
        > False, the client should send 4 requests.
    
    2. Two distinct Web pages (for example, www.mit.edu/research.html and www.mit.edu/students.html) can be sent over the same persistent connection.
        > True, because they are at the same host.
    
    3. With non-persistent connections between browser and origin server, it is possible for a single TCP segment to carry two distinct HTTP request messages.
        > False. 1 non-persistent TCP connection will one carry 1 HTTP request.
    
    4. The `Date:` header in the HTTP response message indicates when the object in the response was last modified.
        > False. `Date:` header is used to record the message sending time. The header indicates when the object in the response was last modified is `Last Modified:`
    
    5. HTTP response messages never have an empty message body.
        > False. The body can be empty. For HTTP State-Code 204 / 304 MUST NOT include a message body 

2. Suppose within your Web browser you click on a link to obtain a Web page. The IP address for the associated URL is not cached in your local host, so a DNS lookup is necessary to obtain the IP address. Suppose that $n$ DNS servers are visited before your host receives the IP address from DNS; the successive visits incur an $RTT$ of $RTT_1 ... RTT_n$. Further suppose that the Web page associated with the link contains exactly one object, consisting of a small amount of HTMLtext. Let $RTT_0$ denote the $RTT$ between the local host and the server containing the object. **Assuming zero transmission time of the object**, how much time elapses from when the client clicks on the link until the client receives the object?

    To retrivel the IP address of the server by DNS lookup:
    $$
    \sum_{i=1}^{N} RTT_i
    $$

    To establish TCP connection to the server and get the object:
    $$
    2 \times RTT_0
    $$

    Total time:
    $$
    2 \times RTT_0 + \sum_{i=1}^{N} RTT_i
    $$

3. Referring to Problem 2, suppose the HTML file references eight very small objects on the same server. Neglecting transmission times, how much time elapses with
    1. Non-persistent HTTP with no parallel TCP connections?

        For each object, client need to establish 1 TCP connection to get the file
        $$
        8 \times (2 \times RTT_0) = 16 \times RTT_0
        $$

        Total Time:
        $$
        18 \times RTT_0 + \sum_{i=1}^{N} RTT_i
        $$

    2. Non-persistent HTTP with the browser configured for 5 parallel connections?

        Client establish at most 5 parallel connections to get objects
        - At first $2 RTT_0$, request 5 objects
        - At second $2 RTT_0$, request remain 3 objects
        $$
        (2 \times RTT_0) + (2 \times RTT_0) = 4 \times RTT_0
        $$

        Total Time
        $$
        6 \times RTT_0 + \sum_{i=1}^{N} RTT_i
        $$

    3. Persistent HTTP? 
    
        Because TCP connection is established, only 1 times of $RTT_0$ to get object

        Total Time
        $$
        3 \times RTT_0 + \sum_{i=1}^{N} RTT_i
        $$

4. Consider distributing a file of $F$ bits to $N$ peers using a P2P architecture. Assume a fluid model. For simplicity assume that $d_{min}$ is very large, so that peer download bandwidth is never a bottleneck.

    > **Fluid Model**: The server can simultaneously transmit to multiple peers, transmitting to each peer at different rates, but the total rate is less or equal to $u_s$

    Define:
    $$
    u = \sum_{i=1}^{N} u_i    
    $$


    1. Suppose that $u_s \leq \frac{(u_s + u_1 + ... + u_N)}{N}$. Specify a distribution scheme that has a distribution time of $\frac{F}{u_s}$

        First, server divide the file as $N$ part, each part contains $F_i = \frac{u_i}{u} F$ bits, and server send them to each peer $i$ simultaneously with rate 
        
        $$
        u_{s, i} = \frac{u_i}{u} u_s
        $$

        When the peer receive parts of $F_i$ with speed $u_{s, i}$, they share to other $N-1$ peers with rate not over $u_i$
    
        $$
        (N - 1) u_{s,i} = (N - 1) \frac{u_i}{u} u_s \leq u_i
        $$

        Because $N u_s \leq (u_s + u)$. Therefore, The time need to recieve each block $F_i$ is

        $$
        \frac{F_i}{u_{s, i}} = \frac{F_i u}{u_i u_s} = \frac{F}{u_s}
        $$

        Notes:
        - This scheme maximize the utilize of $u_s$, and try to archive the best transmission rate $\frac{F}{u_s}$

    2. Suppose that $u_s \geq \frac{(u_s + u_1 + ... + u_N)}{N}$. Specify a distribution scheme that has a distribution time of $\frac{NF}{(u_s + u_1 + ... + u_n)}$.

        First, server divide the file into $N + 1$ parts, send the $i$-th part to $Peer_i$, with the speed of $r_i = \frac{u_i}{N - 1}$, then peer sharing $F_i$ to other peers with speed $r_i = \frac{u_i}{N - 1}$, the total transmission rate of $peer_i$ is equal to $u_i$ 
        
        Then, $F_{N+1}$ will be send from server to everyone with remain bandwidth
        
        $$
        u_{N+1} = u_s - \frac{u}{N - 1}
        $$

        $$
        r_{N+1} = \frac{u_{N+1}}{N}
        $$

        The total received rate is
        $$
        r_{total} = \sum_{i=1}^{N + 1} r_i = \frac{u}{N - 1} + \frac{u_s - \frac{u}{N - 1}}{N} = \frac{u_s + u}{N}
        $$

        And the distribution time is
        $$
        \frac{F}{r_{total}} = \frac{NF}{u_s + u}
        $$
        
        Notes:
        - This scheme maximize the utilize of $u_i$ and $u_s$. The size of each part of file $F_i$, it should be satisfy for $i \in [1, N + 1]$

            $$
            F = \sum_{i=1}^{N} F_i
            $$

            $$
            \frac{F_i}{r_i} = t
            $$

        - $t = t_1 = t_2 = ... = t_{N+1}$ and $F_i$ can be solved

    3. Conclude that the minimum distribution time is in general given by $\max(\frac{F}{u_s}, \frac{NF}{(u_s + u_1 + ... + u_N)})$

        The fluid Model is the ideal model, therefore the answer derived in a) and b) is a lower bound of $D_{P2P}$

        $$
        D_{P2P} \geq \max(\frac{F}{u_s}, \frac{NF}{u_s + \sum_{i=1}^{N} u_i})
        $$
