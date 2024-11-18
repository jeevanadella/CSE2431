/* Copyright (C) 2024 Sreenivas Jeevan Nadella */

typedef int buffer_item;

/* Auto-generated */

void buffer_destroy();
void buffer_init();
void *consumer();
int insert_item(buffer_item item);
void *producer();
int remove_item(buffer_item *item);
