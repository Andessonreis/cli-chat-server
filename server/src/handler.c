//
// Created by andesson on 18/05/25.

#include "../include/service.h"

void *server_input_handler(void *arg) {
    char buffer[MAXMSG];
    while (fgets(buffer, MAXMSG, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        log_with_timestamp(buffer);
        broadcast_message(-1, buffer);
    }
    return NULL;
}

void *client_handler(void *arg) {
    int client_sockfd = *(int *)arg;
    free(arg);

    char buffer[MAXMSG];
    ssize_t bytes_read;

    add_client(client_sockfd);

    while ((bytes_read = recv(client_sockfd, buffer, MAXMSG - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';

        if (strncmp(buffer, "/nick ", 6) == 0) {
            set_client_nickname(client_sockfd, buffer + 6);

            // Logar e notificar a entrada do cliente com nickname
            char nick_log[64];
            snprintf(nick_log, sizeof(nick_log), "[INFO] @%s entrou no chat\n", buffer + 6);
            log_with_timestamp(nick_log);
            broadcast_message(client_sockfd, nick_log);
            continue;
        }

        // Remover quebra de linha
        buffer[strcspn(buffer, "\r\n")] = '\0';

        // Buscar nickname do remetente
        char sender_nick[32] = "desconhecido";
        pthread_mutex_lock(&clients_lock);
        client_node_t *curr = clients;
        while (curr) {
            if (curr->sockfd == client_sockfd) {
                strncpy(sender_nick, curr->nickname, sizeof(sender_nick));
                break;
            }
            curr = curr->next;
        }
        pthread_mutex_unlock(&clients_lock);

        // Montar mensagem formatada
        char final_msg[MAXMSG + 64];
        snprintf(final_msg, sizeof(final_msg), "@%s: %s", sender_nick, buffer);

        // Logar no servidor
        log_with_timestamp(final_msg);

        // Enviar mensagem para os outros clientes
        broadcast_message(client_sockfd, final_msg);
    }

    close(client_sockfd);
    remove_client(client_sockfd);
    pthread_exit(NULL);
}
