#ifndef ROBOT_CONTROL_BOARD_LPS_CONFIG_H
#define ROBOT_CONTROL_BOARD_LPS_CONFIG_H

// Network config
#define NETWORK_ID 10
#define TOTAL_NUM_ANCHORS 3
#define POLL_ACK_CHECK_THRESHOLD 8

// Message flags
#define SET_DEFAULT_VALUE 0
#define SEND_SONG_INFO 1
#define FINAL_RESULT 2
#define EXCHANGE_FINISH 3

// Anchors configuration
#define D12 5.813 // distance bewteen Anchor1 and Anchor2
#define A3X -0.60 // x coordinate of Anchor3
#define A3Y 22.3117 // y coordinate of Anchor3

// Maximum and minimum distance from anchor 1, 2, 3
const float maxDistance[3] = {25.1, 25.1, 25.1};
const float minDistance[3] = {0.1, 0.1, 0.1};

#endif