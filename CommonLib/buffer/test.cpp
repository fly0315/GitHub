#include "IoDebounce.h"
// 参考： https://www.cnblogs.com/Five100Miles/p/8458380.html
void main()
{
	/**
	 * 创建一个64通道（64位数据，每一位代表一个通道）IO采样滤波器对象。
	 * 初始化：0表示，初始值输出值全部为0，
	 * 10表示，采样深度为10个采样点。值域与平台有关，对于16位MCU为[0,2^16)
	 * 8表示，如果10个采样点中有大于等于8个相同状态，则输出该状态，如果没有则输出之前状态。值域[0,采样深度]
	 * 0x00表示，全部通道都需要滤波。如果是0xFF则是最后16个通道的IO采样值不进行滤波，也就是，输出即输入。
	 */
	IoDebounce m_IoBuffer(0, 10, 8, 0x00);
	/**
	 * 以上初始化将64个通道的采样深度，跳变阈值设定为同一值	 
	 * 如果需要对每一个通道进行单独设定则在以上初始化完成后，再进行如下设置
	 */
	unsigned int depth[64] = { 0 };
	unsigned int threshold[64] = { 0 };
	/**
	 * 将64个通道采样深度设置为5，阈值为4
	 * 并将第9个通道深度设置为10，阈值为8
	 */
	for (int i = 0; i < 64; i++)
	{
		depth[i] = 5;
		threshold[i] = 4;
	}
	depth[8] = 10;
	threshold[8] = 8;
	m_IoBuffer.SetFilter(depth, threshold);

	uint64_t IOs_64b;	// 保存采样值
	uint64_t q;			// 保存输出值
	for (int i = 0; i < 10; i++)
	{
		IOs_64b = 0X8000000000000001; 
		// 模拟进行一次采样，q值为输出值
		// 在第1~3次循环中q值为0.
		// 在循环到第4次及以上时，q值为0X8000000000000001
		q = m_IoBuffer.JitterControl(IOs_64b);
	}
	q = m_IoBuffer.GetOutput();	// 获取输出值，q=0X8000000000000001
	for (int i = 0; i < 10; i++)
	{
		IOs_64b = 0x7ffffffffffffffe; 
		// 模拟进行一次采样，q值为输出值
		// 在第1~3次循环中q值为0X8000000000000001.
		// 在循环到第4~7次时，q值为0X7ffffffffffffefe
		// 在循环到第8次及以上时，q值为0x7ffffffffffffffe
		q = m_IoBuffer.JitterControl(IOs_64b);
	}
	q = m_IoBuffer.GetOutput();	// 获取输出值，q=0x7ffffffffffffffe
	
}