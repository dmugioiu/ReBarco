#include "mqtt.h"

char mac_str[13];       // String to hold the MAC address
char short_mac_str[13]; // String to hold the MAC address

char mqtt_topic_cmd[50];
char mqtt_topic_state[50];
char mqtt_topic_discovery[100];

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t brightness;
    bool state;
} rgb_light_t;

rgb_light_t barco_light = {0, 0, 0, 255, true};

// Helper function to convert MAC address to a string
static void mac_to_string(char *mac_str, uint8_t *mac_addr)
{
    sprintf(mac_str, "%02X%02X%02X%02X%02X%02X",
            mac_addr[0], mac_addr[1], mac_addr[2],
            mac_addr[3], mac_addr[4], mac_addr[5]);
}

// Helper function to convert MAC address to a string
static void short_mac_to_string(char *mac_str, uint8_t *mac_addr)
{
    sprintf(mac_str, "%02X%02X", mac_addr[4], mac_addr[5]);
}

// Function to resolve mDNS name to an IP address
static esp_err_t resolve_mdns_hostname(const char *hostname, char *resolved_ip)
{
    struct esp_ip4_addr addr;
    esp_err_t err = mdns_query_a(hostname, 2000, &addr); // 2 second timeout for mDNS resolution
    if (err)
    {
        ESP_LOGE(MQTT_TAG, "Failed to resolve mDNS hostname %s", hostname);
        return err;
    }

    // Convert the IP address to a string
    strcpy(resolved_ip, inet_ntoa(addr));
    ESP_LOGI(MQTT_TAG, "Resolved %s to %s", hostname, resolved_ip);

    return ESP_OK;
}

// Send MQTT discovery message periodically (every 60 seconds)
static void mqtt_discovery_task(void *pvParameters) {
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t) pvParameters;

    while (1) {
        // Wait for 60 seconds (60000 milliseconds)
        vTaskDelay(60000 / portTICK_PERIOD_MS);

        // Send the MQTT discovery message
        mqtt_send_discovery(client);

        
    }
}

// Flash LED on MQTT Activity
static void mqtt_activity_task(void *arg) {
    
    rgb_set_fade_and_start(STANDBY_GREEN_R, STANDBY_GREEN_G, STANDBY_GREEN_B, GREEN_R, GREEN_G, GREEN_B, 250);
    rgb_set_fade_and_start(GREEN_R, GREEN_G, GREEN_B, STANDBY_GREEN_R, STANDBY_GREEN_G, STANDBY_GREEN_B, 250);
    vTaskDelete(NULL);
}

// Send the MQTT discovery configuration
static void mqtt_send_discovery(esp_mqtt_client_handle_t client)
{

    // Build the discovery message with dynamic topics
    char discovery_msg[512];
    snprintf(discovery_msg, sizeof(discovery_msg),
             "{\"name\": \"ReBarco RGB Light %s\","
             "\"unique_id\": \"rebarco_rgb_light_%s\","
             "\"command_topic\": \"%s\","
             "\"state_topic\": \"%s\","
             "\"schema\": \"json\","
             "\"rgb\": true,"
             "\"brightness\": true}",
             short_mac_str, short_mac_str, mqtt_topic_cmd, mqtt_topic_state);

    esp_mqtt_client_publish(client, mqtt_topic_discovery, discovery_msg, 0, 1, 0);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch (event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(event->client, mqtt_topic_cmd, 0);
        mqtt_send_discovery(event->client); // Send discovery message when connected
        // Create the discovery task to send MQTT discovery messages every 60 seconds
        xTaskCreate(mqtt_discovery_task, "mqtt_discovery_task", 2048, (void *) event->client, configMAX_PRIORITIES - 2, NULL);

        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");
        

        if (strncmp(event->topic, mqtt_topic_cmd, event->topic_len) == 0)
        {
            // Parse the RGB values from the message
            char *json_data = strndup(event->data, event->data_len);

            cJSON *root = cJSON_Parse(json_data);
            if (root == NULL)
            {
                ESP_LOGE(MQTT_TAG, "Invalid JSON received");
                return;
            }
            else
            {
                xTaskCreate(mqtt_activity_task, "mqtt_activity_task", 1024, NULL, configMAX_PRIORITIES - 2 , NULL);
                ESP_LOGD(MQTT_TAG, "JSON received: %s", cJSON_Print(root));
            }

            cJSON *brightness = cJSON_GetObjectItem(root, "brightness");
            cJSON *state = cJSON_GetObjectItem(root, "state");
            cJSON *color = cJSON_GetObjectItem(root, "color");
            cJSON *r = cJSON_GetObjectItem(color, "r");
            cJSON *g = cJSON_GetObjectItem(color, "g");
            cJSON *b = cJSON_GetObjectItem(color, "b");

            char state_msg[128];
            if (cJSON_IsNumber(r) && cJSON_IsNumber(g) && cJSON_IsNumber(b))
            {

                barco_light.red = r->valueint;
                barco_light.green = g->valueint;
                barco_light.blue = b->valueint;
                ESP_LOGI(MQTT_TAG, "Received Color Values R:%d G:%d B:%d", barco_light.red, barco_light.green, barco_light.blue);
            }

            if (cJSON_IsString(state))
            {
                if (strcmp(state->valuestring, "ON") == 0)
                {
                    barco_light.state = true;
                }
                else if (strcmp(state->valuestring, "OFF") == 0)
                {
                    barco_light.state = false;
                }
                else
                {
                    ESP_LOGE(MQTT_TAG, "Unknown state value: %s", state->valuestring);
                }
                ESP_LOGI(MQTT_TAG, "Received State: %d", barco_light.state);
            }

            if (cJSON_IsNumber(brightness))
            {
                barco_light.brightness = brightness->valueint;
                ESP_LOGI(MQTT_TAG, "Received Brightness Value: %d", barco_light.brightness);
            }

            // Publish the new state
            snprintf(state_msg, sizeof(state_msg), "{\"state\":\"%s\", \"brightness\":%d, \"color\":{\"r\":%d,\"g\":%d,\"b\":%d}}", barco_light.state ? "ON" : "OFF", barco_light.brightness, barco_light.red, barco_light.green, barco_light.blue);
            esp_mqtt_client_publish(event->client, mqtt_topic_state, state_msg, 0, 1, 0);

            cJSON_Delete(root);

            if (barco_light.state)
            {
                // Scale each color component by the brightness factor
                uint8_t scaled_red = (barco_light.red * barco_light.brightness) / 255;
                uint8_t scaled_green = (barco_light.green * barco_light.brightness) / 255;
                uint8_t scaled_blue = (barco_light.blue * barco_light.brightness) / 255;
                barco_sendcolor(scaled_red, scaled_green, scaled_blue);
            }
            else
            {
                barco_sendcolor(0, 0, 0);
            }
        }

        break;
    default:
        break;
    }
}

void mqtt_app_start(void)
{

    uint8_t mac_addr[6];
    char hostname[32];
    char resolved_ip[16]; // Buffer to hold the resolved IP address as a string
    char mqtt_uri[32];    // Buffer to hold the MQTT URI as a string

    // Get the MAC address of the ESP32
    esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);     // Use the station MAC address
    mac_to_string(mac_str, mac_addr);             // Convert MAC address to a string
    short_mac_to_string(short_mac_str, mac_addr); // Convert MAC address to a string

    snprintf(hostname, sizeof(hostname), "rebarco%s", short_mac_str);
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(hostname)); // Set your own hostname for the ESP32

    // Create unique MQTT topics based on the MAC address
    snprintf(mqtt_topic_cmd, sizeof(mqtt_topic_cmd), MQTT_TOPIC_CMD, mac_str);
    snprintf(mqtt_topic_state, sizeof(mqtt_topic_state), MQTT_TOPIC_STATE, mac_str);
    snprintf(mqtt_topic_discovery, sizeof(mqtt_topic_discovery), MQTT_TOPIC_DISCOVERY_PREFIX "%s/config", mac_str);

    // Resolve the mDNS name
    if (resolve_mdns_hostname(MQTT_MDNS_NAME, resolved_ip) == ESP_OK)
    {
        rgb_set_fade_and_start(ORANGE_R, ORANGE_G, ORANGE_B, BLUE_R, BLUE_G, BLUE_B, 500);
        snprintf(mqtt_uri, sizeof(mqtt_uri), "mqtt://%s:1883", resolved_ip);
        esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = mqtt_uri,
            .credentials.username = "barco",
            .credentials.authentication.password = "barco",
        };
        esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
        esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
        if (esp_mqtt_client_start(client) == ESP_OK) {
            rgb_set_fade_and_start(BLUE_R, BLUE_G, BLUE_B, STANDBY_GREEN_R, STANDBY_GREEN_G, STANDBY_GREEN_B, 500);
        }
        else {
            ESP_LOGE(MQTT_TAG, "Failed to start MQTT Client");
            rgb_set_fade_and_start(BLUE_R, BLUE_G, BLUE_B, RED_R, RED_G, RED_B, 500);
            while (1)
            {
                rgb_set_fade_and_start(OFF_R, OFF_G, OFF_B, RED_R, RED_G, RED_B, 500);
                vTaskDelay(pdMS_TO_TICKS(5));
                rgb_set_fade_and_start(RED_R, RED_G, RED_B, OFF_R, OFF_G, OFF_B, 500);
                vTaskDelay(pdMS_TO_TICKS(5));
            }

        }
    }
    else{
        ESP_LOGE(MQTT_TAG, "Failed to resolve mDNS name");
        rgb_set_fade_and_start(ORANGE_R, ORANGE_G, ORANGE_B, RED_R, RED_G, RED_B, 500);
        while (1)
        {
            rgb_set_fade_and_start(OFF_R, OFF_G, OFF_B, RED_R, RED_G, RED_B, 500);
            vTaskDelay(pdMS_TO_TICKS(5));
            rgb_set_fade_and_start(RED_R, RED_G, RED_B, OFF_R, OFF_G, OFF_B, 500);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}