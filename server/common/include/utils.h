//
// Created by andesson on 07/05/25.
//

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

void save_message_to_log(const char *sender, const char *msg, const char *chat_type);

#endif //UTILS_H
