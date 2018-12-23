// 计算机组成原理主存与Cache的三种不同映射方法 命中率的计算

/////////////////////////////////////////////////////
/////                                          //////
/////               焦焦原创 @ 911306137         //////
/////                                          //////
/////////////////////////////////////////////////////


// 说明：
// int 表示一个字
// Block类 表示一个块， 一个 Block 有 BLOCK_SIZE 个字
// MainStorage类 表示一个主存，一个主存有 MAINSTORAGE_BLOCK 个块
// CacheWithoutGroup类 表示没有分组的 cache，一个这样的 cache 有 CACHE_BLOCK 个块
// Group类 表示一个组，一个这样的组有 GROUP_BLOCK 个块
// CacheWithGroup类 表示分组的cache，一个这样的 cache 有 CACHE_GROUP 个组
// BlockAddressInGroup类 表示分组 cache 中块的地址，专为这种情况而建
// 另有 SimulatorDirectCon SimulatorAllCon SimulatorGroupCon 三个类表示三种模拟器，详见类声明
// VISIT_TIMES 表示访问主存的次数，每次访问主存 random1_5() 个字
// CPU类表示 CPU ， CPU 会对数据进行运算（修改）

#include <iostream>
#include <iomanip>
#include <time.h>
#include <math.h>
// 块字数 4
const int BLOCK_SIZE = 4;
// cache字数  64
const int CACHE_SIZE = 64;
// 主存字数  1024
// 主存不能太大，不然会出错 我的电脑最大1024*54 要注意随即地址的产生也要改
const int MAINSTORAGE_SIZE = 1024;
// 组包含的块数
const int GROUP_BLOCK = 2;
// cache的块数
const int CACHE_BLOCK = CACHE_SIZE/BLOCK_SIZE;
// cache的组数
const int CACHE_GROUP = CACHE_BLOCK/GROUP_BLOCK;
// 主存的块数
const int MAINSTORAGE_BLOCK = MAINSTORAGE_SIZE/BLOCK_SIZE;
// 访问主存的次数 一次访问 random1_5() 个字      一个 int 型的 word 表示一个字
const int VISIT_TIMES = 100;


using namespace std;

// 产生0-1023之间的随机数
int random0_1023()
{
	return rand()%1024;
}
// 产生1-5之间的随机数
int random1_5()
{
	return rand()%5+1;
}
// 检测数据的合理性
bool dataCheck()
{
	if(BLOCK_SIZE<=0 || CACHE_SIZE<=0 || MAINSTORAGE_SIZE<=0 || GROUP_BLOCK<=0 || CACHE_GROUP<=0)
		return false;
	if(BLOCK_SIZE>=2 && BLOCK_SIZE % 2 != 0)
		return false;
	if(CACHE_SIZE % 2 != 0)
		return false;
	if(MAINSTORAGE_SIZE % 2 != 0)
		return false;
	if(GROUP_BLOCK>=2 && GROUP_BLOCK%2!=0)
		return false;
	if(GROUP_BLOCK <= 0 )
		return false;
	if(BLOCK_SIZE > CACHE_SIZE)
		return false;
	if(BLOCK_SIZE > MAINSTORAGE_SIZE)
		return false;
	if(CACHE_SIZE > MAINSTORAGE_SIZE)
		return false;
	if(MAINSTORAGE_BLOCK % CACHE_BLOCK != 0)
		return false;
	if(MAINSTORAGE_SIZE>=1024*64)
		return false;
	if(VISIT_TIMES <= 0)
		return false;

	return true;
}


// 一个int表示一个字
class Block
{
public:
	// 一个块有4个字
	int word[BLOCK_SIZE];
	// 记录此块在主存的块地址
	int mainStorageBlockAddress;
	int useFrequency;
public:
	Block():mainStorageBlockAddress(-1),useFrequency(0)
	{
		for(int i=0;i<BLOCK_SIZE;i++)
			word[i]=0;
	}

	void initialize(int w)
	{
		mainStorageBlockAddress = w;
		for(int i=0;i<BLOCK_SIZE;i++)
		{
			word[i] = w*BLOCK_SIZE + i+1;
		}
	}
};
// 未分组的cache 里面直接是block
class CacheWithoutGroup
{
public:
	// cache 拥有的块数  64/4
	Block block[CACHE_BLOCK];
};
// 对于分组的cache block在cache内的地址
class BlockAddressInGroup
{
public:
	int groupAddress;
	int blockAddressInGroup;
public:
	BlockAddressInGroup():groupAddress(-1),blockAddressInGroup(-1)
	{}
};
// 组（cache里面）
class Group
{
public:
	Block block[GROUP_BLOCK];
};
// 分组的cache
class CacheWithGroup
{
public:
	Group group[CACHE_GROUP];
};
// 主存，对于每种模拟器来说，主存都是一样的
class MainStorage
{
public:
	// 主存 拥有的块数  1024/4
	Block block[MAINSTORAGE_BLOCK];
	MainStorage()
	{
		for(int i=0;i<MAINSTORAGE_BLOCK;i++)
		{
			// 主存的每个字的值设置为  地址+1
			block[i].initialize(i);
		}
	}
};

class CPU
{
public:
	int word;
public:
	CPU():word(0){}
	// CPU 对获取的数的运算
	int operate()
	{
		return int(word*50/43);
	}
	// 产生访问地址
	int address2visit()
	{
		return random0_1023();
	}
	// 产生访问方式
	
	int RW()
	{
		return random0_1023()%2;
	}
	// 产生一次访问的 字数
	int wordNum2Visit()
	{
		return random1_5(); 
	}
};


// 直接映射模拟器
class SimulatorDirectCon
{
public:
	// 拥有一个主存和一个Cache 一个CPU
	MainStorage mainStorage;
	CacheWithoutGroup cache;
	CPU cpu;
	double hitRate;
	double visitCacheTimes;
	double visitMainStorageTimes;
	int RW;
public:
	SimulatorDirectCon()
		:hitRate(0.0),
		visitCacheTimes(0.0),
		visitMainStorageTimes(0.0),
		RW(0)
	{
		
	}
	// 把主存的第mainBlockAddress块调入Cache
	bool loadMainStorage2Cache(int mainBlockAddress)
	{
		// 与主存此块对应的cache的块地址为 主存块地址 % cache的块数
		cache.block[mainBlockAddress % CACHE_BLOCK] = mainStorage.block[mainBlockAddress];
		return true;
	}
	// 与其他模拟器的不同 主要体现在此函数 ------------------------------------------------
	// 把cache中的数据刷新至主存
	bool refreshMainstorageByCache(int cacheBlockAddress)
	{
		// 先把使用频率置零
		cache.block[cacheBlockAddress].useFrequency = 0;
		mainStorage.block[ cache.block[cacheBlockAddress].mainStorageBlockAddress ] = 
			cache.block[cacheBlockAddress];
		return true;
	}

	bool visitMainStorage()
	{
		// 访问主存次数 VISIT_TIMES次
		for(int t=0;t<VISIT_TIMES;t++)
		{
			// 对于每一次访问 随机产生一个地址
			int address = cpu.address2visit();
			// 访问主存条数   random1_5()
			for(int i = 1;i <= cpu.wordNum2Visit();i++)
			{
				// 由 cpu 给出读还是写操作
				RW = cpu.RW();
				int mainStorageBlockAddress = int( address/BLOCK_SIZE );
				int wordInBlockAddress = address % BLOCK_SIZE;
				int cacheBlockAddress = mainStorageBlockAddress % CACHE_BLOCK;
				// cache中的 mainStorageBlockAddress == -1 说明 该块还未被加载到内存中
				// 先访问 cache 
				if(cache.block[cacheBlockAddress].mainStorageBlockAddress == mainStorageBlockAddress)
				{
					visitCacheTimes += 1.0;
					// 如果命中 使用频率增加 把该字给CPU
					cache.block[cacheBlockAddress].useFrequency ++;
					// 读
					if(RW == 0)
						cpu.word = cache.block[cacheBlockAddress].word[wordInBlockAddress];
					// 写
					else
						cache.block[cacheBlockAddress].word[wordInBlockAddress] = cpu.operate();
				}
				// 没有命中
				else
				{
					visitMainStorageTimes += 1.0;
					// 使用频率增加 输出
					mainStorage.block[mainStorageBlockAddress].useFrequency ++;
					// 读
					if(RW == 0)
						cpu.word = mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress];
					// 写
					else
						mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress] = cpu.operate();

					// 判断 与主存该块对应的 cache 的块是否已装数据
					// 装了
					if(cache.block[cacheBlockAddress].mainStorageBlockAddress != -1)
					{
						// 先把cache中的数据刷新至主存
						refreshMainstorageByCache(cacheBlockAddress);
						// 再把主存中的数据调入对应的cache
						loadMainStorage2Cache(mainStorageBlockAddress);
					}
					// 未装
					else
					{

						loadMainStorage2Cache(mainStorageBlockAddress);
					}
				}
				// 对于每一次访问 地址是连续的
				address ++;
				if(address >= MAINSTORAGE_SIZE)
					address = 0;
			}
		}
		hitRate = visitCacheTimes/(visitCacheTimes + visitMainStorageTimes);
		return true;
	}
	
};

// 全相连映射模拟器
class SimulatorAllCon
{
public:
	// 拥有一个主存和一个Cache 一个 CPU
	MainStorage mainStorage;
	CacheWithoutGroup cache;
	CPU cpu;

	double hitRate;
	double visitCacheTimes;
	double visitMainStorageTimes;
	// 表示 读或写
	int RW;
	public:
	SimulatorAllCon()
		:hitRate(0.0),
		visitCacheTimes(0.0),
		visitMainStorageTimes(0.0),
		RW(0)
	{
		
	}
	// 把主存的第mainBlockAddress块调入Cache
	bool loadMainStorage2Cache(int cacheBlockAddr,int mainBlockAddress)
	{
		// 
		cache.block[cacheBlockAddr] = mainStorage.block[mainBlockAddress];
		return true;
	}
	// 与其他模拟器的不同 主要体现在此函数 ------------------------------------------------
	// 把cache中的数据刷新至主存
	int refreshMainstorageByCache()
	{
		int min = VISIT_TIMES;
		int cacheBlockAddress = -1;
		//	求cache 中使用频率最小的块
		for(int i=0;i<CACHE_BLOCK;i++)
		{
			if(cache.block[i].useFrequency <= min)
			{
				min = cache.block[i].useFrequency;
				cacheBlockAddress = i;
			}
		}
		// 先把使用频率置零
		cache.block[cacheBlockAddress].useFrequency = 0;
		mainStorage.block[ cache.block[cacheBlockAddress].mainStorageBlockAddress ] = 
			cache.block[cacheBlockAddress];
		return cacheBlockAddress;
	}

	bool visitMainStorage()
	{
		// 访问主存次数 VISIT_TIMES次
		for(int t=0;t<VISIT_TIMES;t++)
		{
			// 对于每一次访问 随机产生一个地址
			int address = cpu.address2visit();
			// 访问主存条数   random1_5()
			for(int i = 1;i <= cpu.wordNum2Visit();i++)
			{
				// 由 cpu 给出读还是写操作
				RW = cpu.RW();
				int mainStorageBlockAddress = int( address/BLOCK_SIZE );
				int wordInBlockAddress = address % BLOCK_SIZE;
				// cache中的 mainStorageBlockAddress == -1 说明 该块还未被加载到内存中
				// 先访问 cache
				// 判断cache 是否已满 假设 未满
				int cacheEmptyBlockAddr = -1;
				bool isHitTarget = false;
				for(int j=0;j<CACHE_BLOCK;j++)
				{
					// 把该字块 和 cache 所有的块标记进行比较
					if(cache.block[j].mainStorageBlockAddress == mainStorageBlockAddress)
					{
						isHitTarget = true;
						visitCacheTimes += 1.0;
						// 如果命中 使用频率增加 直接输出该值
						cache.block[j].useFrequency ++;
						// 读操作
						if(RW == 0)
							cpu.word = cache.block[j].word[wordInBlockAddress];
						// 写操作
						else
							cache.block[j].word[wordInBlockAddress] = cpu.operate();
						break;
					}
					else
					{
						isHitTarget = false;
					}
					// 说明cache 此块为空
					if(cache.block[j].mainStorageBlockAddress==-1 && cacheEmptyBlockAddr==-1)
					{
						cacheEmptyBlockAddr = j;
						
					}
				
				}
				// 没有命中
				if(!isHitTarget)
				{
					visitMainStorageTimes += 1.0;
					// 使用频率增加 输出
					mainStorage.block[mainStorageBlockAddress].useFrequency ++;
					// 读操作
					if(RW == 0)
						cpu.word = mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress];
					// 写操作
					else
						mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress] = cpu.operate();
		
					if(cacheEmptyBlockAddr != -1)
					{
					
						// 把主存中的数据调入 空着的 cache
						loadMainStorage2Cache(cacheEmptyBlockAddr,mainStorageBlockAddress);
					}
					// 
					else
					{
						// 先把cache中的数据刷新至主存
						int cacheBlockAddress = refreshMainstorageByCache();
						loadMainStorage2Cache(cacheBlockAddress,mainStorageBlockAddress);
					}
				}
				// 对于每一次访问 地址是连续的
				address ++;
				if(address >= MAINSTORAGE_SIZE)
					address = 0;
			}
		}
		hitRate = visitCacheTimes/(visitCacheTimes + visitMainStorageTimes);
		
		return true;
	}
};
// 组相连映射模拟器
class SimulatorGroupCon
{
public:
	// 拥有一个主存和一个Cache 一个CPU
	MainStorage mainStorage;
	CacheWithGroup cache;
	CPU cpu;
	double hitRate;
	double visitCacheTimes;
	double visitMainStorageTimes;
	int RW;

public:
	SimulatorGroupCon()
		:hitRate(0.0),
		visitCacheTimes(0.0),
		visitMainStorageTimes(0.0),
		RW(0)
	{
		
	}
	// 把主存的第mainBlockAddress块调入Cache 的空Block
	bool loadMainStorage2Cache(BlockAddressInGroup cacheBlockAddr,int mainBlockAddress)
	{
		// 
		cache.group[cacheBlockAddr.groupAddress].block[cacheBlockAddr.blockAddressInGroup] 
		= mainStorage.block[mainBlockAddress];
		return true;
	}
	// 与其他模拟器的不同 主要体现在此函数 ------------------------------------------------
	// 把cache中的数据刷新至主存  
	BlockAddressInGroup refreshMainstorageByCache()
	{
		int min = VISIT_TIMES;
		BlockAddressInGroup cacheBlockAddress;
		//	求cache 中使用频率最小的块
		for(int i=0;i<CACHE_GROUP;i++)
		{
			for(int j=0;j<GROUP_BLOCK;j++)
			{
				if(cache.group[i].block[j].useFrequency <= min)
				{
					min = cache.group[i].block[j].useFrequency;
					cacheBlockAddress.groupAddress = i;
					cacheBlockAddress.blockAddressInGroup = j;
				}
			}
		}
		// 先把使用频率置零
		cache.group[cacheBlockAddress.groupAddress].block[cacheBlockAddress.blockAddressInGroup].useFrequency = 0;
		mainStorage.block[ cache.group[cacheBlockAddress.groupAddress].block[cacheBlockAddress.blockAddressInGroup].mainStorageBlockAddress ] = 
			cache.group[cacheBlockAddress.groupAddress].block[cacheBlockAddress.blockAddressInGroup];
		return cacheBlockAddress;
	}

	bool visitMainStorage()
	{
		
		// 访问主存次数 VISIT_TIMES次
		for(int t=0;t<VISIT_TIMES;t++)
		{
			// 对于每一次访问 随机产生一个地址
			int address = cpu.address2visit();
			// 访问主存条数   random1_5()
			for(int i = 1;i <= cpu.wordNum2Visit();i++)
			{
				// 由 cpu 给出读还是写操作
				RW = cpu.RW();
				int mainStorageBlockAddress = int( address/BLOCK_SIZE );
				int cacheGroupAddrdss = mainStorageBlockAddress % CACHE_GROUP;
				int wordInBlockAddress = address % BLOCK_SIZE;
				// cache中的 mainStorageBlockAddress == -1 说明 该块还未被加载到内存中
				// 先访问 cache 
				// 判断cache 是否已满 假设 未满
				BlockAddressInGroup cacheEmptyBlockAddr ;
				bool isHitTarget = false;
				for(int j=0;j<CACHE_GROUP;j++)
				{
					// 把该字块 和 cache 所有的组块标记进行比较
					for(int k=0;k<GROUP_BLOCK;k++)
					{
						if(cache.group[j].block[k].mainStorageBlockAddress == mainStorageBlockAddress)
						{
							isHitTarget = true;
							visitCacheTimes += 1.0;
							// 如果命中 使用频率增加 直接输出该值
							cache.group[j].block[k].useFrequency ++;
							// 读操作
							if(RW == 0)
								cpu.word = cache.group[j].block[k].word[wordInBlockAddress];
							// 写操作
							else
								cache.group[j].block[k].word[wordInBlockAddress] = cpu.operate();
							break;
						}
						else
						{
							isHitTarget = false;
						}
						// 说明cache 此块为空
						if(cache.group[j].block[k].mainStorageBlockAddress==-1 && 
							cacheEmptyBlockAddr.groupAddress==-1 && cacheEmptyBlockAddr.blockAddressInGroup==-1)
						{
							cacheEmptyBlockAddr.groupAddress = j;
							cacheEmptyBlockAddr.blockAddressInGroup = k;
						
						}
					}
					if(isHitTarget)
						break;

				}
				// 没有命中
				if(!isHitTarget)
				{
					visitMainStorageTimes += 1.0;
					// 使用频率增加 输出
					mainStorage.block[mainStorageBlockAddress].useFrequency ++;					
					// 读操作
					if(RW == 0)
						cpu.word = mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress];
					// 写操作
					else
						mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress] = cpu.operate();
					// cache 有空块
					if(cacheEmptyBlockAddr.groupAddress != -1
						&&cacheEmptyBlockAddr.blockAddressInGroup!=-1)
					{
						// 把主存中的数据调入 空着的 cache
						loadMainStorage2Cache(cacheEmptyBlockAddr,mainStorageBlockAddress);
					}
					// cache 无空块
					else
					{
						// 先把cache中的数据刷新至主存 让出空块
						BlockAddressInGroup cacheBlockAddress = refreshMainstorageByCache();
						loadMainStorage2Cache(cacheBlockAddress,mainStorageBlockAddress);
					}
				}
				// 对于每一次访问 地址是连续的
				address ++;
				if(address >= MAINSTORAGE_SIZE)
					address = 0;
			}
		}
		hitRate = visitCacheTimes/(visitCacheTimes + visitMainStorageTimes);
		return true;
	}
};

int main()
{
	
	// 先检查数据合理性
	if(dataCheck())
	{
		cout<<"单位：字"<<endl<<endl;
		cout<<"主存大小:          "<<MAINSTORAGE_SIZE<<endl;
		cout<<"cache大小:         "<<CACHE_SIZE<<endl;
		cout<<"块大小：           "<<BLOCK_SIZE<<endl;
		cout<<"组块数:            "<<GROUP_BLOCK<<endl;
		cout<<"连续访问主存次数:  "<<VISIT_TIMES<<endl;
		SimulatorDirectCon sD;
		SimulatorAllCon sA;
		SimulatorGroupCon sG;
		srand((unsigned)time(NULL)); 
		sD.visitMainStorage();
		cout<<endl<<"直接映射cache命中率：  "<<sD.hitRate<<endl;
		sA.visitMainStorage();
		cout<<endl<<"全相联映射cache命中率："<<sA.hitRate<<endl;
		sG.visitMainStorage();
		cout<<endl<<"组相联映射cache命中率："<<sG.hitRate<<endl;
	}	
	else
		cout<<"Data set Error!";
	system("pause");
	
	return 0;
}