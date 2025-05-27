//
// Created by andesson on 18/05/25.
//

#include "../include/service.h"

void set_client_nickname(int sockfd, const char *nickname_raw) {
    char nickname[32];
    strncpy(nickname, nickname_raw, sizeof(nickname) - 1);
    nickname[sizeof(nickname) - 1] = '\0';

    nickname[strcspn(nickname, "\r\n")] = '\0';

    pthread_mutex_lock(&clients_lock);

    client_node_t *curr = clients;
    while (curr) {
        if (curr->sockfd == sockfd) {
            snprintf(curr->nickname, sizeof(curr->nickname), "%s", nickname);
            break;
        }
        curr = curr->next;
    }

    pthread_mutex_unlock(&clients_lock);

    char msg[64];
    snprintf(msg, sizeof(msg), "\n[INFO] @%s entrou no chat\n", nickname);

    // imprimir no servidor
    log_with_timestamp(msg);

    broadcast_message(-1, msg);
}
