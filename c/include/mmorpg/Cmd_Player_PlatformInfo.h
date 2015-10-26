#ifndef StPlayerPlatformInfo_h
#define StPlayerPlatformInfo_h

#pragma pack(push,1)

struct StPlayerPlatformInfo_V2 
{
	uint16		usSize;
	uint32		uiServerID;
	uint32		uiCountryID;
//	char		szExtendInfo[256];
	//这里应该有个union，有各种平台相关的东西

	void Init()
	{
		memset(this,0,sizeof(*this));
		usSize = sizeof(*this);
	}
};

#pragma pack(pop)

#endif
