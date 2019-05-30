#include "common.h"
#include <iostream>
#include <iomanip>
#include <iconv.h> //clw note��������Ҫ��VS���iconv.h�İ���Ŀ¼
/************************************************************************/
/* ���ߣ�clwclw
/* �ο���https ://blog.csdn.net/lf426/article/details/2243032                                                                    */
/************************************************************************/
std::vector<Uint16> getUnicode(const std::string& str)
{
	//string size
	const int STR_SIZE = 256;
	//string to be changed
	const unsigned char* src = (const unsigned char*)(str.c_str());
	size_t src_len = strlen((char*)src);

	//Unicode dst to get
	//wchar_t dst[STR_SIZE] = { 0 };  //clw modify 20190125
	unsigned char dst[STR_SIZE] = { 0 };
	size_t dst_len = sizeof(dst);

	//iconv's arg
	const unsigned char* in = src;
	//wchar_t* out = dst; //clw modify 20190125
	unsigned char* out = dst;

#ifdef _DEBUG
	std::cout << "src: " << src << std::endl;
#endif

	//GB2312 to UCS-2 (Unicode)
	iconv_t cd;
	cd = iconv_open("UCS-2", "GB2312");
	if ((iconv_t)-1 == cd) {
		exit(-1);
	}
	//conversion
	iconv(cd, (const char**)&in, &src_len, (char**)&out, &dst_len);
	iconv_close(cd);


#ifdef _DEBUG
	//Unicode dst
	std::cout << "dst: ";
#endif

	/************************************************************************/
	//�޸�ԭ��䣺
	//    int unicode_len = strlen((char*)dst);
	//�޸�֮��
	int unicode_len = wcslen((wchar_t*)dst); //clw modify 20190125��
	/************************************************************************
	����������
		iconv������ȫ��ȷ�Ļ��Unicode��Ҳ�������Ǹղ������Ĵ�����ת��û���⣬����Ӣ�ľͲ����ˣ�
		GB2312ת��unicodeʱ������Ӣ���ַ�w��gb2312�����ǵ��ֽڣ�unicode�����������ֽڣ���λΪ0x00,
		ת�����w����������ַ���û��....������ʹ��C�⺯��strlen������ʱ��ͳ��������⣬
		����ANSI�ַ��ı�����'a'=97����ʹ�ø�λ��0��Ҳ����dst[0]=0����˵'\0'
		dst[1]=97����˵'\a'����ô��ʹ��strlen������ʱ��ͳ��������⡣

	ԭ��
		��Ϊunicode�а������ַ���������'\0'�����Բ�����strlen�����ĺ���ȥȡ����

	�����
		����1��ʹ��wcslen��
		����2��Ҳ���Կ����Լ�ʵ��wcslen��
		�Բ���Ϊ2����ͳ�ƣ�����һ���ַ�Ϊ0x00ʱ�����ַ�������
		��������Լ�дûʲô�ѵİɣ�strlen�ǵ��ֽڱȽϣ����ÿ��˫�ֽڱȽϲ������ˣ�
		������wchar�Ļ���ÿ��ָ��+2���Ƚ������ֽڲ������ˣ�
		char* p = str;
		while(!(*p == 0 &&*(p+1) == 0)) p += 2;
		return (int)(p-str)/2;
	/************************************************************************/

	std::vector<Uint16> unicodeVectorArray;
	for (int i = 0; i < unicode_len; i++)
	{
		int a = 256 * dst[2 * i] + dst[2 * i + 1]; //clw note��iconv�Ĳ�����dst��char���ͣ�8λ������
		                                           //������Ҫֱ��ȡdst[2*i]��dst[2*i+1]���ɵ�16λ��
		unicodeVectorArray.push_back(a);
#ifdef _DEBUG
		std::cout << std::hex;  //clw note��������������ʮ�����������
		std::cout << "0x" << std::setw(4) << std::setfill('0') << a << " ";
		std::cout << std::dec;  //clw note���ָ�ʮ���������
#endif
	}
#ifdef _DEBUG
	std::cout << std::endl;
	std::cout << "len: " << unicode_len << std::endl;
#endif
	return unicodeVectorArray;
}