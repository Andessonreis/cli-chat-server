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

    char welcome_msg[] = "\n\tBem vindo ao servidor de chat!!\n"
                         " Use o comando /nick <seu_nome> para definir seu apelido.\n"
                         " Exemplo: /nick João\n"
                         " Use o comando /msg <apelido> <mensagem> para enviar mensagens privadas.\n\n";
    send(client_sockfd, welcome_msg, strlen(welcome_msg), 0);

    char buffer[MAXMSG];
    ssize_t bytes_read;

    add_client(client_sockfd);

    while ((bytes_read = recv(client_sockfd, buffer, MAXMSG - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';

        // Comando /nick
        if (strncmp(buffer, "/nick ", 6) == 0) {
            set_client_nickname(client_sockfd, buffer + 6);

            char nick_log[64];
            snprintf(nick_log, sizeof(nick_log), "[INFO] @%s entrou no chat\n", buffer + 6);
            log_with_timestamp(nick_log);
            broadcast_message(client_sockfd, nick_log);
            continue;
        }

        // Comando /msg (mensagem privada)
        if (strncmp(buffer, "/msg ", 5) == 0) {
            char *space = strchr(buffer + 5, ' ');
            if (space) {
                *space = '\0';
                const char *recipient_nick = buffer + 5;
                const char *private_msg = space + 1;

                // Remover quebra de linha da mensagem privada
                char clean_msg[MAXMSG];
                strncpy(clean_msg, private_msg, sizeof(clean_msg));
                clean_msg[strcspn(clean_msg, "\r\n")] = '\0';

                // Buscar socket do destinatário
                int recipient_sockfd = -1;
                pthread_mutex_lock(&clients_lock);
                client_node_t *curr = clients;
                while (curr) {
                    if (strcmp(curr->nickname, recipient_nick) == 0) {
                        recipient_sockfd = curr->sockfd;
                        break;
                    }
                    curr = curr->next;
                }
                pthread_mutex_unlock(&clients_lock);

                if (recipient_sockfd != -1) {
                    // Buscar apelido do remetente
                    char sender_nick[32] = "desconhecido";
                    pthread_mutex_lock(&clients_lock);
                    curr = clients;
                    while (curr) {
                        if (curr->sockfd == client_sockfd) {
                            strncpy(sender_nick, curr->nickname, sizeof(sender_nick));
                            break;
                        }
                        curr = curr->next;
                    }
                    pthread_mutex_unlock(&clients_lock);

                    char final_msg[MAXMSG + 64];
                    snprintf(final_msg, sizeof(final_msg), "[Privado] @%s: %s", sender_nick, clean_msg);
                    send(recipient_sockfd, final_msg, strlen(final_msg), 0);

                    // Logar a mensagem privada
                    save_message_to_log(sender_nick, clean_msg, "private");
                } else {
                    char error_msg[] = "[ERRO] Usuário não encontrado.\n";
                    send(client_sockfd, error_msg, strlen(error_msg), 0);
                }
                continue;
            }
        }

        // Remover quebra de linha da mensagem pública
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

        // Salvar no log público
        save_message_to_log(sender_nick, buffer, "public");

        // Enviar mensagem para os outros clientes
        broadcast_message(client_sockfd, buffer);  

    }

    close(client_sockfd);
    remove_client(client_sockfd);
    pthread_exit(NULL);
}
