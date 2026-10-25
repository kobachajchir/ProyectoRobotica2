/* AFR2-20261024-07. Shared semantic IDs; labels live in AVR flash/Web catalog.
 * No user-controlled string is copied into SRAM sequence records. */
#ifndef AFR2_SEQUENCE_LABELS_H
#define AFR2_SEQUENCE_LABELS_H
enum { AFR2_SEQ_DISPLACEMENT, AFR2_SEQ_RAMP, AFR2_SEQ_PIVOT,
       AFR2_SEQ_CURVE, AFR2_SEQ_WAIT, AFR2_SEQ_FINISH, AFR2_SEQ_WHEELS,
       AFR2_SEQ_LABEL_COUNT };
#endif
