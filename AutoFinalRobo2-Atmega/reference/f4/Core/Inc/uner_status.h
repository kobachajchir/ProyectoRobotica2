#ifndef UNER_STATUS_H
#define UNER_STATUS_H
typedef enum {
    UNER_OK = 0,
    UNER_ERR_BUSY,
    UNER_ERR_LEN,
    UNER_ERR_CHK,
    UNER_ERR_HEADER,
    UNER_ERR_TOKEN,
    UNER_ERR_VER,
    UNER_ERR_QUEUE_FULL,
} UNER_Status;
#endif
