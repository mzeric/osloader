
int memcpy(void *dst, void *src, int size){
	char *s = dst;
	char *e = src;
	while(size--)
		*s++ = *e++;
}
