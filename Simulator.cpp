// ��������ԭ��������Cache�����ֲ�ͬӳ�䷽�� �����ʵļ���

/////////////////////////////////////////////////////
/////                                          //////
/////               ����ԭ�� @ 911306137         //////
/////                                          //////
/////////////////////////////////////////////////////


// ˵����
// int ��ʾһ����
// Block�� ��ʾһ���飬 һ�� Block �� BLOCK_SIZE ����
// MainStorage�� ��ʾһ�����棬һ�������� MAINSTORAGE_BLOCK ����
// CacheWithoutGroup�� ��ʾû�з���� cache��һ�������� cache �� CACHE_BLOCK ����
// Group�� ��ʾһ���飬һ������������ GROUP_BLOCK ����
// CacheWithGroup�� ��ʾ�����cache��һ�������� cache �� CACHE_GROUP ����
// BlockAddressInGroup�� ��ʾ���� cache �п�ĵ�ַ��רΪ�����������
// ���� SimulatorDirectCon SimulatorAllCon SimulatorGroupCon �������ʾ����ģ���������������
// VISIT_TIMES ��ʾ��������Ĵ�����ÿ�η������� random1_5() ����
// CPU���ʾ CPU �� CPU ������ݽ������㣨�޸ģ�

#include <iostream>
#include <iomanip>
#include <time.h>
#include <math.h>
// ������ 4
const int BLOCK_SIZE = 4;
// cache����  64
const int CACHE_SIZE = 64;
// ��������  1024
// ���治��̫�󣬲�Ȼ����� �ҵĵ������1024*54 Ҫע���漴��ַ�Ĳ���ҲҪ��
const int MAINSTORAGE_SIZE = 1024;
// ������Ŀ���
const int GROUP_BLOCK = 2;
// cache�Ŀ���
const int CACHE_BLOCK = CACHE_SIZE/BLOCK_SIZE;
// cache������
const int CACHE_GROUP = CACHE_BLOCK/GROUP_BLOCK;
// ����Ŀ���
const int MAINSTORAGE_BLOCK = MAINSTORAGE_SIZE/BLOCK_SIZE;
// ��������Ĵ��� һ�η��� random1_5() ����      һ�� int �͵� word ��ʾһ����
const int VISIT_TIMES = 100;


using namespace std;

// ����0-1023֮��������
int random0_1023()
{
	return rand()%1024;
}
// ����1-5֮��������
int random1_5()
{
	return rand()%5+1;
}
// ������ݵĺ�����
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


// һ��int��ʾһ����
class Block
{
public:
	// һ������4����
	int word[BLOCK_SIZE];
	// ��¼�˿�������Ŀ��ַ
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
// δ�����cache ����ֱ����block
class CacheWithoutGroup
{
public:
	// cache ӵ�еĿ���  64/4
	Block block[CACHE_BLOCK];
};
// ���ڷ����cache block��cache�ڵĵ�ַ
class BlockAddressInGroup
{
public:
	int groupAddress;
	int blockAddressInGroup;
public:
	BlockAddressInGroup():groupAddress(-1),blockAddressInGroup(-1)
	{}
};
// �飨cache���棩
class Group
{
public:
	Block block[GROUP_BLOCK];
};
// �����cache
class CacheWithGroup
{
public:
	Group group[CACHE_GROUP];
};
// ���棬����ÿ��ģ������˵�����涼��һ����
class MainStorage
{
public:
	// ���� ӵ�еĿ���  1024/4
	Block block[MAINSTORAGE_BLOCK];
	MainStorage()
	{
		for(int i=0;i<MAINSTORAGE_BLOCK;i++)
		{
			// �����ÿ���ֵ�ֵ����Ϊ  ��ַ+1
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
	// CPU �Ի�ȡ����������
	int operate()
	{
		return int(word*50/43);
	}
	// �������ʵ�ַ
	int address2visit()
	{
		return random0_1023();
	}
	// �������ʷ�ʽ
	
	int RW()
	{
		return random0_1023()%2;
	}
	// ����һ�η��ʵ� ����
	int wordNum2Visit()
	{
		return random1_5(); 
	}
};


// ֱ��ӳ��ģ����
class SimulatorDirectCon
{
public:
	// ӵ��һ�������һ��Cache һ��CPU
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
	// ������ĵ�mainBlockAddress�����Cache
	bool loadMainStorage2Cache(int mainBlockAddress)
	{
		// ������˿��Ӧ��cache�Ŀ��ַΪ ������ַ % cache�Ŀ���
		cache.block[mainBlockAddress % CACHE_BLOCK] = mainStorage.block[mainBlockAddress];
		return true;
	}
	// ������ģ�����Ĳ�ͬ ��Ҫ�����ڴ˺��� ------------------------------------------------
	// ��cache�е�����ˢ��������
	bool refreshMainstorageByCache(int cacheBlockAddress)
	{
		// �Ȱ�ʹ��Ƶ������
		cache.block[cacheBlockAddress].useFrequency = 0;
		mainStorage.block[ cache.block[cacheBlockAddress].mainStorageBlockAddress ] = 
			cache.block[cacheBlockAddress];
		return true;
	}

	bool visitMainStorage()
	{
		// ����������� VISIT_TIMES��
		for(int t=0;t<VISIT_TIMES;t++)
		{
			// ����ÿһ�η��� �������һ����ַ
			int address = cpu.address2visit();
			// ������������   random1_5()
			for(int i = 1;i <= cpu.wordNum2Visit();i++)
			{
				// �� cpu ����������д����
				RW = cpu.RW();
				int mainStorageBlockAddress = int( address/BLOCK_SIZE );
				int wordInBlockAddress = address % BLOCK_SIZE;
				int cacheBlockAddress = mainStorageBlockAddress % CACHE_BLOCK;
				// cache�е� mainStorageBlockAddress == -1 ˵�� �ÿ黹δ�����ص��ڴ���
				// �ȷ��� cache 
				if(cache.block[cacheBlockAddress].mainStorageBlockAddress == mainStorageBlockAddress)
				{
					visitCacheTimes += 1.0;
					// ������� ʹ��Ƶ������ �Ѹ��ָ�CPU
					cache.block[cacheBlockAddress].useFrequency ++;
					// ��
					if(RW == 0)
						cpu.word = cache.block[cacheBlockAddress].word[wordInBlockAddress];
					// д
					else
						cache.block[cacheBlockAddress].word[wordInBlockAddress] = cpu.operate();
				}
				// û������
				else
				{
					visitMainStorageTimes += 1.0;
					// ʹ��Ƶ������ ���
					mainStorage.block[mainStorageBlockAddress].useFrequency ++;
					// ��
					if(RW == 0)
						cpu.word = mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress];
					// д
					else
						mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress] = cpu.operate();

					// �ж� ������ÿ��Ӧ�� cache �Ŀ��Ƿ���װ����
					// װ��
					if(cache.block[cacheBlockAddress].mainStorageBlockAddress != -1)
					{
						// �Ȱ�cache�е�����ˢ��������
						refreshMainstorageByCache(cacheBlockAddress);
						// �ٰ������е����ݵ����Ӧ��cache
						loadMainStorage2Cache(mainStorageBlockAddress);
					}
					// δװ
					else
					{

						loadMainStorage2Cache(mainStorageBlockAddress);
					}
				}
				// ����ÿһ�η��� ��ַ��������
				address ++;
				if(address >= MAINSTORAGE_SIZE)
					address = 0;
			}
		}
		hitRate = visitCacheTimes/(visitCacheTimes + visitMainStorageTimes);
		return true;
	}
	
};

// ȫ����ӳ��ģ����
class SimulatorAllCon
{
public:
	// ӵ��һ�������һ��Cache һ�� CPU
	MainStorage mainStorage;
	CacheWithoutGroup cache;
	CPU cpu;

	double hitRate;
	double visitCacheTimes;
	double visitMainStorageTimes;
	// ��ʾ ����д
	int RW;
	public:
	SimulatorAllCon()
		:hitRate(0.0),
		visitCacheTimes(0.0),
		visitMainStorageTimes(0.0),
		RW(0)
	{
		
	}
	// ������ĵ�mainBlockAddress�����Cache
	bool loadMainStorage2Cache(int cacheBlockAddr,int mainBlockAddress)
	{
		// 
		cache.block[cacheBlockAddr] = mainStorage.block[mainBlockAddress];
		return true;
	}
	// ������ģ�����Ĳ�ͬ ��Ҫ�����ڴ˺��� ------------------------------------------------
	// ��cache�е�����ˢ��������
	int refreshMainstorageByCache()
	{
		int min = VISIT_TIMES;
		int cacheBlockAddress = -1;
		//	��cache ��ʹ��Ƶ����С�Ŀ�
		for(int i=0;i<CACHE_BLOCK;i++)
		{
			if(cache.block[i].useFrequency <= min)
			{
				min = cache.block[i].useFrequency;
				cacheBlockAddress = i;
			}
		}
		// �Ȱ�ʹ��Ƶ������
		cache.block[cacheBlockAddress].useFrequency = 0;
		mainStorage.block[ cache.block[cacheBlockAddress].mainStorageBlockAddress ] = 
			cache.block[cacheBlockAddress];
		return cacheBlockAddress;
	}

	bool visitMainStorage()
	{
		// ����������� VISIT_TIMES��
		for(int t=0;t<VISIT_TIMES;t++)
		{
			// ����ÿһ�η��� �������һ����ַ
			int address = cpu.address2visit();
			// ������������   random1_5()
			for(int i = 1;i <= cpu.wordNum2Visit();i++)
			{
				// �� cpu ����������д����
				RW = cpu.RW();
				int mainStorageBlockAddress = int( address/BLOCK_SIZE );
				int wordInBlockAddress = address % BLOCK_SIZE;
				// cache�е� mainStorageBlockAddress == -1 ˵�� �ÿ黹δ�����ص��ڴ���
				// �ȷ��� cache
				// �ж�cache �Ƿ����� ���� δ��
				int cacheEmptyBlockAddr = -1;
				bool isHitTarget = false;
				for(int j=0;j<CACHE_BLOCK;j++)
				{
					// �Ѹ��ֿ� �� cache ���еĿ��ǽ��бȽ�
					if(cache.block[j].mainStorageBlockAddress == mainStorageBlockAddress)
					{
						isHitTarget = true;
						visitCacheTimes += 1.0;
						// ������� ʹ��Ƶ������ ֱ�������ֵ
						cache.block[j].useFrequency ++;
						// ������
						if(RW == 0)
							cpu.word = cache.block[j].word[wordInBlockAddress];
						// д����
						else
							cache.block[j].word[wordInBlockAddress] = cpu.operate();
						break;
					}
					else
					{
						isHitTarget = false;
					}
					// ˵��cache �˿�Ϊ��
					if(cache.block[j].mainStorageBlockAddress==-1 && cacheEmptyBlockAddr==-1)
					{
						cacheEmptyBlockAddr = j;
						
					}
				
				}
				// û������
				if(!isHitTarget)
				{
					visitMainStorageTimes += 1.0;
					// ʹ��Ƶ������ ���
					mainStorage.block[mainStorageBlockAddress].useFrequency ++;
					// ������
					if(RW == 0)
						cpu.word = mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress];
					// д����
					else
						mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress] = cpu.operate();
		
					if(cacheEmptyBlockAddr != -1)
					{
					
						// �������е����ݵ��� ���ŵ� cache
						loadMainStorage2Cache(cacheEmptyBlockAddr,mainStorageBlockAddress);
					}
					// 
					else
					{
						// �Ȱ�cache�е�����ˢ��������
						int cacheBlockAddress = refreshMainstorageByCache();
						loadMainStorage2Cache(cacheBlockAddress,mainStorageBlockAddress);
					}
				}
				// ����ÿһ�η��� ��ַ��������
				address ++;
				if(address >= MAINSTORAGE_SIZE)
					address = 0;
			}
		}
		hitRate = visitCacheTimes/(visitCacheTimes + visitMainStorageTimes);
		
		return true;
	}
};
// ������ӳ��ģ����
class SimulatorGroupCon
{
public:
	// ӵ��һ�������һ��Cache һ��CPU
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
	// ������ĵ�mainBlockAddress�����Cache �Ŀ�Block
	bool loadMainStorage2Cache(BlockAddressInGroup cacheBlockAddr,int mainBlockAddress)
	{
		// 
		cache.group[cacheBlockAddr.groupAddress].block[cacheBlockAddr.blockAddressInGroup] 
		= mainStorage.block[mainBlockAddress];
		return true;
	}
	// ������ģ�����Ĳ�ͬ ��Ҫ�����ڴ˺��� ------------------------------------------------
	// ��cache�е�����ˢ��������  
	BlockAddressInGroup refreshMainstorageByCache()
	{
		int min = VISIT_TIMES;
		BlockAddressInGroup cacheBlockAddress;
		//	��cache ��ʹ��Ƶ����С�Ŀ�
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
		// �Ȱ�ʹ��Ƶ������
		cache.group[cacheBlockAddress.groupAddress].block[cacheBlockAddress.blockAddressInGroup].useFrequency = 0;
		mainStorage.block[ cache.group[cacheBlockAddress.groupAddress].block[cacheBlockAddress.blockAddressInGroup].mainStorageBlockAddress ] = 
			cache.group[cacheBlockAddress.groupAddress].block[cacheBlockAddress.blockAddressInGroup];
		return cacheBlockAddress;
	}

	bool visitMainStorage()
	{
		
		// ����������� VISIT_TIMES��
		for(int t=0;t<VISIT_TIMES;t++)
		{
			// ����ÿһ�η��� �������һ����ַ
			int address = cpu.address2visit();
			// ������������   random1_5()
			for(int i = 1;i <= cpu.wordNum2Visit();i++)
			{
				// �� cpu ����������д����
				RW = cpu.RW();
				int mainStorageBlockAddress = int( address/BLOCK_SIZE );
				int cacheGroupAddrdss = mainStorageBlockAddress % CACHE_GROUP;
				int wordInBlockAddress = address % BLOCK_SIZE;
				// cache�е� mainStorageBlockAddress == -1 ˵�� �ÿ黹δ�����ص��ڴ���
				// �ȷ��� cache 
				// �ж�cache �Ƿ����� ���� δ��
				BlockAddressInGroup cacheEmptyBlockAddr ;
				bool isHitTarget = false;
				for(int j=0;j<CACHE_GROUP;j++)
				{
					// �Ѹ��ֿ� �� cache ���е�����ǽ��бȽ�
					for(int k=0;k<GROUP_BLOCK;k++)
					{
						if(cache.group[j].block[k].mainStorageBlockAddress == mainStorageBlockAddress)
						{
							isHitTarget = true;
							visitCacheTimes += 1.0;
							// ������� ʹ��Ƶ������ ֱ�������ֵ
							cache.group[j].block[k].useFrequency ++;
							// ������
							if(RW == 0)
								cpu.word = cache.group[j].block[k].word[wordInBlockAddress];
							// д����
							else
								cache.group[j].block[k].word[wordInBlockAddress] = cpu.operate();
							break;
						}
						else
						{
							isHitTarget = false;
						}
						// ˵��cache �˿�Ϊ��
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
				// û������
				if(!isHitTarget)
				{
					visitMainStorageTimes += 1.0;
					// ʹ��Ƶ������ ���
					mainStorage.block[mainStorageBlockAddress].useFrequency ++;					
					// ������
					if(RW == 0)
						cpu.word = mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress];
					// д����
					else
						mainStorage.block[mainStorageBlockAddress].word[wordInBlockAddress] = cpu.operate();
					// cache �пտ�
					if(cacheEmptyBlockAddr.groupAddress != -1
						&&cacheEmptyBlockAddr.blockAddressInGroup!=-1)
					{
						// �������е����ݵ��� ���ŵ� cache
						loadMainStorage2Cache(cacheEmptyBlockAddr,mainStorageBlockAddress);
					}
					// cache �޿տ�
					else
					{
						// �Ȱ�cache�е�����ˢ�������� �ó��տ�
						BlockAddressInGroup cacheBlockAddress = refreshMainstorageByCache();
						loadMainStorage2Cache(cacheBlockAddress,mainStorageBlockAddress);
					}
				}
				// ����ÿһ�η��� ��ַ��������
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
	
	// �ȼ�����ݺ�����
	if(dataCheck())
	{
		cout<<"��λ����"<<endl<<endl;
		cout<<"�����С:          "<<MAINSTORAGE_SIZE<<endl;
		cout<<"cache��С:         "<<CACHE_SIZE<<endl;
		cout<<"���С��           "<<BLOCK_SIZE<<endl;
		cout<<"�����:            "<<GROUP_BLOCK<<endl;
		cout<<"���������������:  "<<VISIT_TIMES<<endl;
		SimulatorDirectCon sD;
		SimulatorAllCon sA;
		SimulatorGroupCon sG;
		srand((unsigned)time(NULL)); 
		sD.visitMainStorage();
		cout<<endl<<"ֱ��ӳ��cache�����ʣ�  "<<sD.hitRate<<endl;
		sA.visitMainStorage();
		cout<<endl<<"ȫ����ӳ��cache�����ʣ�"<<sA.hitRate<<endl;
		sG.visitMainStorage();
		cout<<endl<<"������ӳ��cache�����ʣ�"<<sG.hitRate<<endl;
	}	
	else
		cout<<"Data set Error!";
	system("pause");
	
	return 0;
}