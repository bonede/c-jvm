#define U4(num) (((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000))

#define U2(num) ((num << 8) | (num >> 8 ))