/**
 * 工具函数
 */
#include "global.h"
#include <ctype.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


/**
 * mmap a file
 */
int map_file(void **addr,const char *file)
{
    struct stat st;
    void *content;
    int fd = open(file, O_RDONLY);

    if (fd < 0)
    {
        return -1;
    }

    if ( fstat(fd, &st) < 0 || st.st_size == 0 )
    {
        close(fd);
        return -2;
    }

    content = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    if (content == MAP_FAILED)
    {
        return -3;
    }

    *addr = content;
    return st.st_size;
}


/**
 * read a file
 */
int read_file(uint8_t **addr,const char *file)
{
	int len;
    struct stat st;
    uint8_t *content;
    int fd = open(file, O_RDONLY);

    if (fd < 0)
    {
        return -1;
    }

    if ( fstat(fd, &st) < 0 || st.st_size == 0 )
    {
        close(fd);
        return -2;
    }

    content = (uint8_t *)malloc(st.st_size + 1);
	if (content == NULL)
	{
		close(fd);
		return -3;
	}

	len = read(fd, content, st.st_size);
    close(fd);

	if (len != st.st_size)
	{
		free(content);
		return -4;
	}

	content[len] = 0;

    *addr = content;
    return len;
}


char *strip(char *s)
{
	char *p;

	for (p = s; *p; p++)
	{
		if ( ! isspace(*p) )
			break;
	}

	s = p;

	for (; *p; p++)
	{
		if ( isspace(*p) )
		{
			*p = '\0';
			break;
		}
	}

	return s;
}


int32_t checksum(uint16_t *buf, int32_t len)
{
	int32_t nleft = len;
	int32_t sum = 0;
	uint16_t *w = buf;
	uint16_t answer = 0;

	while(nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}

	if(nleft == 1)
	{
		*(uint16_t *)(&answer) = *(uint8_t *)w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;

	return answer;
}


static char *format_line(unsigned char *data,size_t len)
{
    unsigned char c;
    register size_t j;
    static char line[128];
    const char *hexbase = "0123456789ABCDEF";
    char *p = line;

    for ( j = 0; j < len; j++ )
    {
        c = data[ j ];
        *p++ = hexbase[c >> 4];
        *p++ = hexbase[c & 0xF];
        *p++ = (j==7?'-':' ');
    }

    if (len == 8)
    {
        *(p-1) = ' ';
    }

    j = 16 - len + 1;
    for ( ; j; j-- )
    {
        *p++ = ' ';
        *p++ = ' ';
        *p++ = ' ';
    }

    for ( j = 0; j < len; j++ )
    {
        c = data[ j ];
        if ( c  >= 0x20  && c  <= 0x7E )
        {
            *p++ = c;
        }
        else
        {
            *p++ = '.';
        }
    }
     *p = '\0';

    return line;
}


void hexdump (unsigned char *data, int index, int len)
{
    int datalen;
    register int   k, i;

    if (data == NULL || index < 0 || len <= 0)
    {
        return;
    }

    i = 0;
    datalen = len;
    data += index;

    for ( k = datalen / 16; k > 0; k--,i+=16 )
    {
        printf("%04X  %s\n",i,format_line(data+i,16));
    }

    k = datalen - i;
    if ( k >= 0 )
    {
        printf("%04X  %s\n",i,format_line(data+i,k));
    }
}

