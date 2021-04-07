#include <tea.h>

static void tea_encrypt(uint32_t  *v, uint32_t  *k)
{
	uint32_t  y = v[0], z = v[1], sum = 0, i;
	uint32_t  delta = 0x9e3779b9;
	uint32_t  a = k[0], b = k[1], c = k[2], d = k[3];

	for (i = 0; i < 32; i++)
	{
		sum += delta;
		y += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
		z += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
	}
	v[0] = y;
	v[1] = z;
}

/*********************************************************************
*             tea����
*����:v:Ҫ���ܵ�����,����Ϊ8�ֽ�
*     k:�����õ�key,����Ϊ16�ֽ�
**********************************************************************/

static void tea_decrypt(uint32_t  *v, uint32_t  *k)
{
	uint32_t  y = v[0], z = v[1], sum = 0xC6EF3720, i;
	uint32_t  delta = 0x9e3779b9;
	uint32_t  a = k[0], b = k[1], c = k[2], d = k[3];

	for (i = 0; i < 32; i++)
	{
		z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
		y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
		sum -= delta;
	}
	v[0] = y;
	v[1] = z;
}
	/*********************************************************************
*             �����㷨
*����:src:Դ����,��ռ�ռ����Ϊ8�ֽڵı���.������ɺ�����Ҳ�������
*     size_src:Դ���ݴ�С,��λ�ֽ�
*     key:��Կ,16�ֽ�
*����:���ĵ��ֽ���
**********************************************************************/
uint16_t  encrypt(uint8_t  *src, uint16_t  size_src, uint8_t  *key)
{
	uint8_t  a = 0;
	uint16_t  i = 0;
	uint16_t  num = 0;

	//�����Ĳ���Ϊ8�ֽڵı���
	a = size_src % 8;
	if (a != 0)
	{
		for (i = 0; i < 8 - a; i++)
		{
			src[size_src++] = 0;
		}
	}

	//����
	num = size_src / 8;
	for (i = 0; i < num; i++)
	{
		tea_encrypt((uint32_t  *)(src + i * 8), (uint32_t  *)key);
	}

	return size_src;
}

/*********************************************************************
*             �����㷨
*����:src:Դ����,��ռ�ռ����Ϊ8�ֽڵı���.������ɺ�����Ҳ�������
*     size_src:Դ���ݴ�С,��λ�ֽ�
*     key:��Կ,16�ֽ�
*����:���ĵ��ֽ���,���ʧ��,����0
**********************************************************************/

uint16_t  decrypt(uint8_t  *src, uint16_t  size_src, uint8_t  *key)
{
	uint16_t  i = 0;
	uint16_t  num = 0;

	//�жϳ����Ƿ�Ϊ8�ı���
	if (size_src % 8 != 0)
	{
		return 0;
	}

	//����
	num = size_src / 8;
	for (i = 0; i < num; i++)
	{
		tea_decrypt((uint32_t  *)(src + i * 8), (uint32_t  *)key);
	}

	return size_src;
}
 
/*int testsamples()
{
	int i;
    uint8_t  k[]="156787987987979879879879879879879879";
    uint8_t  v[]="sdfjhakjsdhfkjhakdhf";
    // vΪҪ���ܵ�����������32λ�޷�������
    // kΪ���ܽ�����Կ��Ϊ4��32λ�޷�������������Կ����Ϊ128λ
    printf("����ǰԭʼ����:");
    for(i = 0 ; i<sizeof(v) ; i++)
    {
    	printf("%c",v[i]);
	}
	printf("\n");
    int n =encrypt(v,sizeof(v), k);
    printf("���ܺ������:");
     for(i = 0 ; i<sizeof(v) ; i++)
    {
    	printf("%c",v[i]);
	}
	printf("\n");
    decrypt(v,n,k);
    printf("���ܺ������:");
     for(i = 0 ; i<sizeof(v) ; i++)
    {
    	printf("%c",v[i]);
	}
	printf("\n");
    system("pause");
    return 0;
}*/
