#include <sys/socket.h>
#include <netinet/in.h>

#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

#define CONFIG_CAFILE "/etc/ssl/certs/ca-certificates.crt"

struct http_client {
	struct {
		int auth:1;
	} flags;
	gnutls_certificate_credentials_t xcred;
	gnutls_session_t session;
};

typedef struct http_client * httpclnt;

int tcp_connect(void)
{
	int sockfd;
	struct sockaddr_in serv_addr;

	/* create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		return sockfd;

	/* connect to ip:port */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(1337); /* remember to swap bytes */
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		return -1;
	return sockfd;
}

void tcp_close(int sd)
{
	shutdown(sockfd, 2);
}

httpclnt httpclnt_init(void)
{
	httpclnt clnt;

	/* NOTE: this should be thread safe. This function seems obsolete though. */
	if (gnutls_global_init() < 0)
		return NULL;

	if ((clnt = malloc(sizeof(*clnt))) == NULL)
		return NULL;
	if (gnutls_certificate_allocate_credentials(&xcred) < 0)
		goto out1;
	if (gnutls_certificate_set_x509_trust_file(xcred, CAFILE,
		GNUTLS_X509_FMT_PEM) < 0)
		goto out;
	if (gnutls_init(&session, GNUTLS_CLIENT) < 0)
		goto out;
	if (gnutls_server_name_set(session, GNUTLS_NAME_DNS, dns, strlen(dns)) < 0)
		goto out;
	if (gnutl_set_default_priority(session) < 0)
		goto out;
	if (gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, xcred) < 0)
		goto out;
}

void httpclnt_perform(handle)
{
	tcp_connect();

	gnutls_transport_set_int(session, sd);
	gnutls_handshake_set_timeout(session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);

	/* Perform the TLS handshake */
	do {
		ret = gnutls_handshake(session);
	} while (ret < 0 && gnutls_error_is_fatal(ret) == 0);
	if (ret < 0)
		goto err;

	if (gnutls_record_send(session, , ) < 0)
		goto err;
	ret = gnutls_record_recv(session, buffer, MAX_BUF);
	if (ret <= 0)
		goto err;

	/* Terminate TLS connection. */
	if (gnutls_bye(session, GNUTLS_SHUT_RDWR) < 0)
		goto out;
	tcp_close(sd);
	gnutls_deinit(session);
}

void httpclnt_cleanup(handle)
{
	gnutls_certificate_free_credentials(xcred);
	gnutls_global_deinit()
}
