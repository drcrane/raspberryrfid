#ifndef __READCARD_H__
#define __READCARD_H__

#ifdef __cplusplus
extern "C" {
#endif

int ReadCard(unsigned char* cardid, size_t* cardidsz);

#ifdef __cplusplus
}
#endif

#endif // __READCARD_H__
