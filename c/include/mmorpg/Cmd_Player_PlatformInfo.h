#ifndef StPlayerPlatformInfo_h
#define StPlayerPlatformInfo_h

#pragma pack(push,1)

struct StPlayerPlatformInfo_V2 
{
	uint16		usSize;
	uint32		uiServerID;
	uint32		uiCountryID;
//	char		szExtendInfo[256];
	//����Ӧ���и�union���и���ƽ̨��صĶ���

	void Init()
	{
		memset(this,0,sizeof(*this));
		usSize = sizeof(*this);
	}
};

#pragma pack(pop)

#endif
