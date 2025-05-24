/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "tcp.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>  // struct addrinfo
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "esp_log.h"
#include "esp_netif.h"
#include "sdkconfig.h"

#define PORT 3333
#define RX_BUFFER_SIZE 128

static const char *TAG = "example";
static const char *payload = "Message from ESP32 ";
static const char host_ip[] = "192.168.178.26";
static char rx_buffer[RX_BUFFER_SIZE];

int get_new_socket(int *old_socket) {
    if (old_socket != NULL) {
        ESP_LOGI(TAG, "Closing old socket");
        close(*old_socket);
    }
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Socket creation failed with error: %d, %s:%d", errno, host_ip, PORT);
        return -1;
    }
    return sock;
}

// Creates a connection to host_ip:port, will return the port of the connection. or -1 on failure
int get_new_connection() {
    // Create Socket
    int socket = get_new_socket(NULL);
    if (socket == -1) {
        return -1;
    }

    struct sockaddr_in dest_addr;
    inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);

    // Try to connect, if an error occurs, wait
    if (connect(socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) != 0) {
        ESP_LOGE(TAG, "Connect failed with error code: %d", errno);
        if (errno == ETIMEDOUT || errno == ECONNREFUSED || errno == ECONNRESET) {
            while (true) {
                // Get new socket, as the old socket will be blocked regardless
                socket = get_new_socket(&socket);

                // Wait a few seconds
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                ESP_LOGI(TAG, "retrying...");

                // Try to connect again
                if (connect(socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == 0) {
                    return socket;
                }
            }
            return -1;
        } else {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            return -1;
        }
    }
    return socket;
}

// Sends shit
void tcp_client() {
    while (1) {
        // Connect to PC
        ESP_LOGI(TAG, "Connecting...");
        int sock = get_new_connection();
        if (sock == -1) {
            ESP_LOGE(TAG, "Could not connect");
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");

        while (1) {
            int err = send(sock, payload, strlen(payload), 0);
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }

            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                rx_buffer[len] = 0;  // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI(TAG, "%s", rx_buffer);
            }
        }
        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}