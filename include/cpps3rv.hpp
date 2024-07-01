#pragma once

#include <atomic>
#include <concurrentqueue/concurrentqueue>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

class cpps3rv {

public:
  template <typename receive_callback, typename send_callback,
            typename binding_error_callback>
  cpps3rv() {
    this->runningp = true;
  }

  void run() {

    while (runningp) {
      int client_fd = accept(server_fd, NULL, NULL);
      if (client_fd == -1) {
        perror("accept");
        continue;
      }

      SSL *ssl = SSL_new(ctx);
      SSL_set_fd(ssl, client_fd);

      if (SSL_accept(ssl) <= 0) {
        // ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(client_fd);
      } else {
        make_socket_non_blocking(client_fd);
        epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        Connection *conn = new Connection{client_fd, ssl};
        event.data.ptr = conn;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
          // perror("epoll_ctl: client_fd");
          SSL_shutdown(ssl);
          SSL_free(ssl);
          close(client_fd);
          delete conn;
        } else {
          connection_queue.enqueue(*conn);
        }
      }
    }
  }

protected:
  int create_server_socket(int port) {
    int server_fd;
    struct sockaddr_in addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
      // perror
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
      // perror("setsockopt");
      close(server_fd);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      // perror("bind failed");
      close(server_fd);
    }

    if (listen(server_fd, 10) < 0) {
      // perror("listen");
      close(server_fd);
    }

    return server_fd;
  }

  int make_socket_non_blocking(int sfd) {
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1) {
      // perror("fcntl");
      return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1) {
      // perror("fcntl");
      return -1;
    }

    return 0;
  }

protected:
  std::atomic<bool> runningp;
};