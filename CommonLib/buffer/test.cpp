#include "IoDebounce.h"
// �ο��� https://www.cnblogs.com/Five100Miles/p/8458380.html
void main()
{
	/**
	 * ����һ��64ͨ����64λ���ݣ�ÿһλ����һ��ͨ����IO�����˲�������
	 * ��ʼ����0��ʾ����ʼֵ���ֵȫ��Ϊ0��
	 * 10��ʾ���������Ϊ10�������㡣ֵ����ƽ̨�йأ�����16λMCUΪ[0,2^16)
	 * 8��ʾ�����10�����������д��ڵ���8����ͬ״̬���������״̬�����û�������֮ǰ״̬��ֵ��[0,�������]
	 * 0x00��ʾ��ȫ��ͨ������Ҫ�˲��������0xFF�������16��ͨ����IO����ֵ�������˲���Ҳ���ǣ���������롣
	 */
	IoDebounce m_IoBuffer(0, 10, 8, 0x00);
	/**
	 * ���ϳ�ʼ����64��ͨ���Ĳ�����ȣ�������ֵ�趨Ϊͬһֵ	 
	 * �����Ҫ��ÿһ��ͨ�����е����趨�������ϳ�ʼ����ɺ��ٽ�����������
	 */
	unsigned int depth[64] = { 0 };
	unsigned int threshold[64] = { 0 };
	/**
	 * ��64��ͨ�������������Ϊ5����ֵΪ4
	 * ������9��ͨ���������Ϊ10����ֵΪ8
	 */
	for (int i = 0; i < 64; i++)
	{
		depth[i] = 5;
		threshold[i] = 4;
	}
	depth[8] = 10;
	threshold[8] = 8;
	m_IoBuffer.SetFilter(depth, threshold);

	uint64_t IOs_64b;	// �������ֵ
	uint64_t q;			// �������ֵ
	for (int i = 0; i < 10; i++)
	{
		IOs_64b = 0X8000000000000001; 
		// ģ�����һ�β�����qֵΪ���ֵ
		// �ڵ�1~3��ѭ����qֵΪ0.
		// ��ѭ������4�μ�����ʱ��qֵΪ0X8000000000000001
		q = m_IoBuffer.JitterControl(IOs_64b);
	}
	q = m_IoBuffer.GetOutput();	// ��ȡ���ֵ��q=0X8000000000000001
	for (int i = 0; i < 10; i++)
	{
		IOs_64b = 0x7ffffffffffffffe; 
		// ģ�����һ�β�����qֵΪ���ֵ
		// �ڵ�1~3��ѭ����qֵΪ0X8000000000000001.
		// ��ѭ������4~7��ʱ��qֵΪ0X7ffffffffffffefe
		// ��ѭ������8�μ�����ʱ��qֵΪ0x7ffffffffffffffe
		q = m_IoBuffer.JitterControl(IOs_64b);
	}
	q = m_IoBuffer.GetOutput();	// ��ȡ���ֵ��q=0x7ffffffffffffffe
	
}