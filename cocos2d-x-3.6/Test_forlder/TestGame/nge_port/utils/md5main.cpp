#include <utils/md5.h>
#include <math.h>
#include <stdio.h>
#include <string.h>


int toMD5(char* dest, const char* src)
{
	md5_state_t state;
	md5_byte_t digest[16];
	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)src, strlen((char *)src));
	md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di) {
	    sprintf(dest + di * 2, "%02x", digest[di]);
	}
	return 0;
}

int toMD5WithLen(char* dest, const char* src, int len)
{
	md5_state_t state;
	md5_byte_t digest[16];
	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)src, len);
	md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di) {
	    sprintf(dest + di * 2, "%02x", digest[di]);
	}
	return 0;
}

#define FILE_BUFFER_SIZE 4096
int fileMD5(char* dest, const char* path) {
	FILE * pFile = fopen(path, "rb");
    
    if (!pFile) {
		return -1;
    }

	md5_state_t state;
	md5_byte_t digest[16];
	md5_init(&state);
    
    int len = 0;
    md5_byte_t buffer[FILE_BUFFER_SIZE];
    while (!feof(pFile)) {
        len = fread(buffer, FILE_BUFFER_SIZE, 1, pFile);
        if (len == 1) {
            md5_append(&state, buffer, FILE_BUFFER_SIZE);
        } else {
            //reach end of file.
            int total_size = ftell(pFile);
            md5_append(&state, buffer, total_size % FILE_BUFFER_SIZE);
        }
    }
	
    md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di) {
	    sprintf(dest + di * 2, "%02x", digest[di]);
	}
    
    fclose(pFile);

	return 0;

}