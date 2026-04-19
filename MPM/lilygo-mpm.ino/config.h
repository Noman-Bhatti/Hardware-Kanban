#pragma once

// WiFi
#define WIFI_SSID         "your_ssid"
#define WIFI_PASSWORD     "your_password"

// API
#define API_BASE_URL      "https://api.modularpm.co.uk"
#define DEVICE_ID         "eink-tag-001"
#define DEVICE_TOKEN      "your_device_token_here"

// Timing
#define POLL_INTERVAL_MS  300000   // 5 min task refresh
#define BLE_SCAN_EVERY_MS 30000    // 30 sec column check

// BLE
#define BEACON_UUID       "550e8400-e29b-41d4-a716-446655440000"
#define RSSI_THRESHOLD    -85

// Column names — must exactly match ModularPM board column names
#define COL_TODO          "Todo"
#define COL_IN_PROGRESS   "In Progress"
#define COL_REVIEW        "Review"
#define COL_DONE          "Done"