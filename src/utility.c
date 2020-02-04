//
// Created by cpasjuste on 03/02/2020.
//

#include <string.h>
#include <stdlib.h>
#include "cross.h"
#include "utility.h"

FileList s_file_list;

void add_file(FileList *list, File *entry, int sort) {

    entry->next = NULL;
    entry->previous = NULL;

    if (list->head == NULL) {
        list->head = entry;
        list->tail = entry;
    } else {
        if (sort != SORT_NONE) {
            File *p = list->head;
            File *previous = NULL;

            while (p != NULL) {
                // Sort by type
                if (entry->type < p->type)
                    break;

                // '..' is always at first
                if (strcmp(entry->name, "..") == 0)
                    break;

                if (strcmp(p->name, "..") != 0) {
                    // Get the minimum length without /
                    size_t elen = strlen(entry->name);
                    size_t plen = strlen(p->name);
                    int len = MIN(elen, plen);
                    if (entry->name[len - 1] == '/' || p->name[len - 1] == '/')
                        len--;

                    // Sort by name
                    if (entry->type == p->type) {
                        int diff = strncasecmp(entry->name, p->name, (size_t) len);
                        if (diff < 0 || (diff == 0 && elen < plen)) {
                            break;
                        }
                    }
                }

                previous = p;
                p = (File *) p->next;
            }

            if (previous == NULL) { // Order: entry (new head) -> p (old head)
                entry->next = (struct File *) p;
                p->previous = (struct File *) entry;
                list->head = entry;
            } else if (previous->next == NULL) { // Order: p (old tail) -> entry (new tail)
                File *tail = list->tail;
                tail->next = (struct File *) entry;
                entry->previous = (struct File *) tail;
                list->tail = entry;
            } else { // Order: previous -> entry -> p
                previous->next = (struct File *) entry;
                entry->previous = (struct File *) previous;
                entry->next = (struct File *) p;
                p->previous = (struct File *) entry;
            }
        } else {
            File *tail = list->tail;
            tail->next = (struct File *) entry;
            entry->previous = (struct File *) tail;
            list->tail = entry;
        }
    }

    list->count++;
}

void free_dir(FileList *list) {

    File *entry = list->head;

    while (entry) {
        File *next = (File *) entry->next;
        free(entry);
        entry = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

File *get_file(int index) {

    File *file = s_file_list.head;

    for (int i = 0; i < s_file_list.count; i++) {
        if (index == i) {
            return (File *) file->next;
        }
        file = (File *) file->next;
    }

    return NULL;
}
