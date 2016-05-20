#include "ot_def.h"




//判断内存是否全部填充某个字符
//返回长度 和 样本长度相同 表示匹配
//小于样本长度 表示发现不匹配的首位置
u32_t memchcmp(const void* s, u8_t c, size_t n)
{
	u32_t u = 0;
	u8_t* p = (u8_t*)s;

	for(u = 0; u < n; u++)
		if(c != p[u])break;

	return u;
}







