/**
 * 工具函数
 */
#ifndef __CBB_UTILS_H__
#define __CBB_UTILS_H__

int map_file(void **addr,const char *file);
int read_file(uint8_t **addr,const char *file);

char *strip(char *s);
int32_t checksum(uint16_t *buf, int32_t len);
void hexdump (unsigned char *data, int index, int len);

#endif

