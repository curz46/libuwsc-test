#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <uwsc.h>

struct uwsc_client* client;
char* test_string;

void ws_onopen(struct uwsc_client* client) {
    printf("Connection opened.\n");
}

void ws_onmessage(struct uwsc_client* client,
                  void*  data,
                  size_t length,
                  bool   binary) {
    if (binary) {
        printf("Received binary data, ignoring.\n");
        return;
    }

    if (! test_string) {
        printf("Received a string when no string was expected.\n");
        return;
    }

    char* content = (char*) data;
    if (strcmp(content, test_string) != 0) {
        printf("BUG: Received content which does *not* match expected string:\n");
        printf("%s\n\n", content);
    } else {
        printf("Received expected string.\n");
    }

    test_string = NULL;
}

void send_data_periodically() {
    const long period = 1000000 * 3; // every 3 seconds
    while (1) {
        usleep(period);

        char* content = (char*) malloc(sizeof(char) * strlen(test_string));
        strcpy(content, test_string);

        // Send packet
        client->send(client, content, strlen(content), UWSC_OP_TEXT);
        printf("Sent: %s\n", content);
    }
}

int main() {
    printf("Run test with small test string or large test string?\n");
    printf("1. Small (<100 chars)\n");
    printf("2. Large (>500 chars)\n");
    int choice;
    scanf("%i", &choice);

    if (choice == 1) {
        test_string = "This is a sample string.";
    } else if (choice == 2) {
        FILE* file = fopen("test.txt", "r");
        fseek(file, 0, SEEK_END);
        int length = ftell(file);
        fseek(file, 0, SEEK_SET);
        test_string = (char*) malloc(length);
        if (!test_string) {
            printf("Failed to allocate buffer for test string.\n");
            exit(1);
        }
        fread(test_string, 1, length, file);
        fclose(file);
    } else {
        printf("Unrecognised choice.\n");
        exit(1);
    }

    printf("Using test string:\n\n%s\n\n", test_string);

    const char* url = "wss://echo.websocket.org";
    // const char* url = "wss://demos.kaazing.com/echo";
    
    client = uwsc_new(EV_DEFAULT, url, 10, NULL);
    client->onopen    = ws_onopen;
    client->onmessage = ws_onmessage;

    struct ev_signal signal_watcher;
    ev_signal_init(&signal_watcher, NULL, SIGINT);
    ev_signal_start(EV_DEFAULT, &signal_watcher);

    // Start sending
    pthread_t* thread;
    pthread_create(&thread, NULL, send_data_periodically, NULL);

    // Start event loop
    ev_run(EV_DEFAULT, 0);

    return 0;
}
