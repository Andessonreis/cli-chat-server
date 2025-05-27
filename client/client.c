//
// Created by andesson on 18/05/25.
//

#include "../server/include/service.h"

void add_client(int sockfd) {
    pthread_mutex_lock(&clients_lock);

    client_node_t *new_client = malloc(sizeof(client_node_t));
    new_client->sockfd = sockfd;
    new_client->next = clients;
    clients = new_client;

    pthread_mutex_unlock(&clients_lock);
}

void remove_client(int sockfd) {
    pthread_mutex_lock(&clients_lock);

    client_node_t **curr = &clients;
    while (*curr) {
        if ((*curr)->sockfd == sockfd) {
            client_node_t *to_delete = *curr;
            *curr = (*curr)->next;
            free(to_delete);
            break;
        }
        curr = &(*curr)->next;
    }

    pthread_mutex_unlock(&clients_lock);
}

