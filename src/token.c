#include <stdio.h>
#include <stdlib.h>

#include "token.h"

void l_append(Token **head, char *x) {
    Token *temp, *p;
    temp = (Token*)malloc(sizeof(Token));
    temp->data = x;
    temp->next = NULL;

    if (*head == NULL) {
        *head = temp;
    } else {
        p = *head;

        while(p->next != NULL) {
            p = p->next;
        }
        p->next = temp;
    }
}

void l_free(Token *temp) {
    if(temp == NULL) {
        return;
    }
    free(temp->data);
    l_free(temp->next);
    free(temp);
}
