#include "wrappers_network.h"

/* ##################################################
 * wrap_socket -- create an endpoint for communication
 * @domain -- communication domain
 * @type -- socket type
 * @protocol -- protocol are associated with socket
 *
 * tcp_socket_ip4() -- create TCP socket from AF_INET domain
 *
 * return:
 *		fd of socket of -1 if it is failed
 */

int tcp_socket_ip4(void)
{
	return wrap_socket(AF_INET, SOCK_STREAM, 0);
}

int wrap_socket(int domain, int type, int protocol)
{
	int fd = socket(domain, type, protocol);

	if (fd == -1)
	{
		log_err_with_perror("FAIL", NULL);
	}
	else
	{
		log_debug("SUCCESS(fd: %d)", fd);
	}

	return fd;
}

// ##################################################

/*
 * wrap_bind -- bind a name to a socket
 * @sockfd -- fd of socket
 * @addr -- pointer to sockaddr
 * @addrlen -- size of struct on which pointing addr
 *
 * return:
 *		0 -- success; -1 -- failed;
 */

int wrap_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int ret_f = bind(sockfd, addr, addrlen);

	if (ret_f == -1)
	{
		log_err_with_perror("FAIL", NULL);
	}
	else
	{
		log_debug("SUCCESS", NULL);
	}

	return ret_f;
}

// ##################################################

/*
 * wrap_listen -- listen for connections on a socket
 * @sockfd -- fd of socket for listening
 * @backlog -- queue of connection
 *
 * return:
 *		0 -- success; -1 -- failed;
 */

int wrap_listen(int sockfd, int backlog)
{
	int ret_f = listen(sockfd, backlog);

	if (ret_f == -1)
	{
		log_err_with_perror("FAIL", NULL);
	}
	else
	{
		log_debug("SUCCESS", NULL);
	}

	return ret_f;
}

// ##################################################

/*
 * wrap_accept -- accept a connection on a socket
 * @sockfd -- socket are listening
 * @addr -- pointer for storage address of peer
 * @addrlen -- size of addr
 *
 * accept_without_addr -- wrap_accept without addr and addrlen arguments
 *
 * return:
 *		>= 0 -- success(fd for communicating with peer);
 *		-1 -- failed;
 */

int accept_without_addr(int sockfd)
{
	return wrap_accept(sockfd, NULL, NULL);
}

int wrap_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int ret_f = accept(sockfd, addr, addrlen);

	if (ret_f == -1)
	{
		log_err_with_perror("FAIL", NULL);
	}
	else
	{
		log_debug("SUCCESS", NULL);
	}

	return ret_f;
}

// ##################################################


/*
 * wrap_connect -- initiate a connection on a socket
 * @sockfd -- host socket
 * @addr -- peer address
 * @addrlen -- size of addr
 *
 * return:
 *		0 -- success; -1 -- failed;
 */

int wrap_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int ret_f = connect(sockfd, addr, addrlen);

	if (ret_f == -1)
	{
		log_err_with_perror("FAIL", NULL);
	}
	else
	{
		log_debug("SUCCESS", NULL);
	}

	return ret_f;
}

// ##################################################

/*
 * wrap_recv -- receive a message from a socket
 * @sockfd -- socket from which taking data
 * @buf -- buffer for receiving data
 * @len -- size of buffer
 * @flags -- flags
 *
 * return:
 *		number of received bytes or -1 if are failed
 */

ssize_t wrap_recv(int sockfd, void *buf, size_t len, int flags)
{
	ssize_t ret_f = recv(sockfd, buf, len, flags);

	if (ret_f == -1)
	{
		log_err_with_perror("FAIL", NULL);
	}
	else
	{
		log_debug("SUCCESS", NULL);
	}

	return ret_f;
}

// ##################################################

/*
 * wrap_send -- send a message on a socket
 * @sockfd -- socket are connected with peer
 * @buf -- buffer for sending
 * @len -- size of buffer
 * @flags -- flags
 *
 * return:
 *		number of sent bytes or -1 if are failed
 */

ssize_t wrap_send(int sockfd, const void *buf, size_t len, int flags)
{
	ssize_t ret_f = send(sockfd, buf, len, flags | MSG_NOSIGNAL);

	if (ret_f == -1)
	{
		log_err_with_perror("FAIL", NULL);
	}
	else
	{
		log_debug("SUCCESS", NULL);
	}

	return ret_f;
}

// ##################################################

/*
 * wrap_ntop -- convert IPv4 and IPv6 addresses from binary to text form
 * @af -- address family
 * @src -- network address structure
 * @dst -- resulting string
 * @size -- size of dst
 *
 * easy_ntop_ip4 -- converting IPv4 from binary to text form
 *
 * return:
 *		non-NULL -- success;
 *		NULL -- failed;
 */

const char* easy_ntop_ip4(const void *src)
{
	char *buf = (char*)malloc(sizeof(char) * INET_ADDRSTRLEN);

	if (buf != NULL)
	{
		if (wrap_ntop(AF_INET, src, buf, sizeof(char) * INET_ADDRSTRLEN) == NULL)
		{
			free(buf);
			buf = NULL;
		}
	}
	else
	{
		log_err_with_perror("troubles with malloc", NULL);
	}

	return buf;
}

const char* wrap_ntop(int af, const void *src, char *dst, socklen_t size)
{
	const char *ret_f = inet_ntop(af, src, dst, size);

	if (ret_f == NULL)
	{
		log_err_with_perror("FAIL", NULL);
	}
	else
	{
		log_debug("converted address: %s", ret_f);
	}

	return ret_f;
}

// ##################################################

/*
 * wrap_pton -- convert IPv4 and IPv6 addresses from text to binary form
 * @af -- address family
 * @src -- source string
 * @dst -- buffer are value-result
 *
 * easy_pton_ip4 -- convert IPv4 from string to binary form
 *
 * return:
 *		1 -- success;
 *		0 -- incorrect address into @src
 *		-1 -- src are not corresponding to @af
 */

int easy_pton_ip4(const char *src, void *dst)
{
	return wrap_pton(AF_INET, src, dst);
}

int wrap_pton(int af, const char *src, void *dst)
{
	int ret_f = inet_pton(af, src, dst);

	if (ret_f == 1)
	{
		log_debug("SUCCESS", NULL);
	}
	else
	{
		log_err_with_perror("not valid character into address or "
								   "address family are not matching to specified address", NULL);
	}

	return ret_f;
}

// ##################################################

/*
 * get_sockaddr_in -- created and initializing sockadr_in with given arguments
 * @ip -- IP address string
 * @port -- port's number
 *
 * return:
 *		NULL -- failed;
 *		non-NULL -- success;
 */

struct sockaddr_in* get_sockaddr_in(const char *ip, uint16_t port)
{
	struct sockaddr_in *ret_f = (struct sockaddr_in*)calloc(1, sizeof(struct sockaddr_in));

	if (ret_f != NULL)
	{
		ret_f->sin_family = AF_INET;
		ret_f->sin_port = htons(port);

		if (easy_pton_ip4(ip, &(ret_f->sin_addr)) == 1)
		{
			log_debug("SUCCESS", NULL);
		}
		else
		{
			log_err_with_perror("FAIL", NULL);
			free(ret_f);
			ret_f = NULL;
		}
	}
	else
	{
		log_err_with_perror("calloc are failed", NULL);
	}

	return ret_f;
}

// ##################################################
