#ifndef ICALLSTACKTRACK_H__
#define ICALLSTACKTRACK_H__
#include "IUnknownEx.h"
/*
	堆栈追踪，用于在Release版或其它不方便debug的情况下随时Trace堆栈的调用过程
	由于要跨模块运行，所以要继承IUnknownEx，
	由于要照顾运行效率，所以尽量不用虚函数且要inline

  2008-02-02/03/04	alan 建立文件，完成功能
*/

static const CLASSUID	IF_UID_ICALLSTACKTRACK ("ICallStackTrack");
static const uint32		IF_VER_ICALLSTACKTRACK = INTERFACE_VER(2,0);

class ICallStackTrack : public IUnknownEx
{
	struct StStackItem
	{
		enum {	MAX_PARAM_COUNT	= 10	};		
		const char*	pFuncName;
		int			nParamCount;
		long		lParam[MAX_PARAM_COUNT];

		void PushParam(long l)
		{
			if ( nParamCount >= 0 && nParamCount < MAX_PARAM_COUNT)
			{
				lParam[nParamCount] = l;
				nParamCount++;
			}
			else
			{
				assert(0);
			}
		}
		void PopParam()
		{
			if ( nParamCount > 0 && nParamCount <= MAX_PARAM_COUNT)
				nParamCount--;
			else
				assert(0);
		}
		long GetParam(int n = -1) const //n == -1表示最后一个
		{
			if ( nParamCount <= 0 || nParamCount > MAX_PARAM_COUNT)
			{
				assert(0);
				return -1;
			}
			
			if ( n == -1 )
				return lParam[nParamCount-1];
			else if ( n >= 0 && n < nParamCount )
				return lParam[n];
			else
			{
				assert(0);
				return -1;
			}
		}
		
		const char*	GetFunctionName() const
		{
			return pFuncName;
		}
		
		int	GetParamCount() const
		{
			return	nParamCount;
		}
	};

protected:	
	StStackItem*m_pStack;
	int		m_nStorage;
	int		m_nCount;
	StStackItem	m_NullItem;
protected:
	virtual void ReAllowStack()
	{
		m_nStorage *= 2;
		StStackItem*p = new StStackItem[m_nStorage];
		if ( m_nCount  > 0 )
			memcpy(p,m_pStack,sizeof(StStackItem)*m_nCount);
		delete []m_pStack;
		m_pStack = p;
	}

	inline void _EnterBlock(const char*pFunc)
	{
		assert(pFunc && m_nCount <= m_nStorage && m_nCount >=0);
		if ( m_nCount >= m_nStorage  ) 
			ReAllowStack();
		m_pStack[m_nCount].pFuncName = pFunc;
	}

	inline void _LeaveBlock()
	{
		m_nCount--;
		assert(m_nCount >= 0);
	}
public:
	ICallStackTrack()
	{
		m_nCount	= 0;		
		m_nStorage	= 50;
		m_pStack	= new StStackItem[m_nStorage];
		memset(&m_NullItem,0,sizeof(&m_NullItem));
		m_NullItem.pFuncName = "傻了吧？知道我是谁吗？";
	}
	~ICallStackTrack()
	{
		if ( m_pStack )
			delete []m_pStack;
	}
	
	virtual void* QueryInterface(const CLASSUID & cuid,uint32 ulVer)
	{
		if ( cuid == IF_UID_ICALLSTACKTRACK && IsVersionFit(IF_VER_ICALLSTACKTRACK,ulVer) )
		{
			ICallStackTrack*pThis = this;
			return pThis;
		}

		return 	IUnknownEx::QueryInterface(cuid,ulVer);
	}
	
	const StStackItem& GetStackItem(int n = -1) const
	{
		assert(m_nCount <= m_nStorage && m_nCount >=0);
		if ( m_nCount <= 0 )
		{
			assert(0);
			return m_NullItem;
		}
		if ( n == -1 )
			return m_pStack[m_nCount-1];
		else if ( n >= 0 )
			return m_pStack[n];
		else
		{
			assert(0);
			return m_NullItem;
		}
	}
	int GetStorageCount() const	
	{
		return m_nStorage;
	}
	int GetDeep() const
	{
		return m_nCount;
	}

	//为最顶的栈元素加/减参数
	void PushParam(long l)
	{
		if ( m_nCount > 0 )
			m_pStack[m_nCount-1].PushParam(l);
		else
			assert(0);
	}
	void PopParam()
	{
		if ( m_nCount > 0 )
			m_pStack[m_nCount-1].PopParam();
		else
			assert(0);
	}

	void TraceStack()
	{
		assert(m_nCount <= m_nStorage);
		if ( m_nCount <= 0 )
		{
			assert(m_nCount == 0);
			RelTrace("堆栈深度=%d",m_nCount);	//=0才是正确的
			return;
		}
		for (int i=0; i<m_nCount; i++)
		{
			if ( m_pStack[i].nParamCount == 0 )
				RelTrace("%d) %s",														i,m_pStack[i].pFuncName);
			else if ( m_pStack[i].nParamCount == 1 )
				RelTrace("%d) %s 参数(0x%X)",											i,m_pStack[i].pFuncName,m_pStack[i].lParam[0]);
			else if ( m_pStack[i].nParamCount == 2 )
				RelTrace("%d) %s 参数(0x%X,0x%X)",										i,m_pStack[i].pFuncName,m_pStack[i].lParam[0],m_pStack[i].lParam[1]);
			else if ( m_pStack[i].nParamCount == 3 )
				RelTrace("%d) %s 参数(0x%X,0x%X,0x%X)",									i,m_pStack[i].pFuncName,m_pStack[i].lParam[0],m_pStack[i].lParam[1],m_pStack[i].lParam[2]);
			else if ( m_pStack[i].nParamCount == 4 )
				RelTrace("%d) %s 参数(0x%X,0x%X,0x%X,0x%X)",							i,m_pStack[i].pFuncName,m_pStack[i].lParam[0],m_pStack[i].lParam[1],m_pStack[i].lParam[2],m_pStack[i].lParam[3]);
			else if ( m_pStack[i].nParamCount == 5 )
				RelTrace("%d) %s 参数(0x%X,0x%X,0x%X,0x%X,0x%X)",						i,m_pStack[i].pFuncName,m_pStack[i].lParam[0],m_pStack[i].lParam[1],m_pStack[i].lParam[2],m_pStack[i].lParam[3],m_pStack[i].lParam[4]);
			else if ( m_pStack[i].nParamCount == 6 )
				RelTrace("%d) %s 参数(0x%X,0x%X,0x%X,0x%X,0x%X,0x%X)",					i,m_pStack[i].pFuncName,m_pStack[i].lParam[0],m_pStack[i].lParam[1],m_pStack[i].lParam[2],m_pStack[i].lParam[3],m_pStack[i].lParam[4],m_pStack[i].lParam[5]);
			else if ( m_pStack[i].nParamCount == 7 )
				RelTrace("%d) %s 参数(0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X)",				i,m_pStack[i].pFuncName,m_pStack[i].lParam[0],m_pStack[i].lParam[1],m_pStack[i].lParam[2],m_pStack[i].lParam[3],m_pStack[i].lParam[4],m_pStack[i].lParam[5],m_pStack[i].lParam[6]);
			else if ( m_pStack[i].nParamCount == 8 )
				RelTrace("%d) %s 参数(0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X)",		i,m_pStack[i].pFuncName,m_pStack[i].lParam[0],m_pStack[i].lParam[1],m_pStack[i].lParam[2],m_pStack[i].lParam[3],m_pStack[i].lParam[4],m_pStack[i].lParam[5],m_pStack[i].lParam[6],m_pStack[i].lParam[7]);
			else if ( m_pStack[i].nParamCount == 9 )
				RelTrace("%d) %s 参数(0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X)",	i,m_pStack[i].pFuncName,m_pStack[i].lParam[0],m_pStack[i].lParam[1],m_pStack[i].lParam[2],m_pStack[i].lParam[3],m_pStack[i].lParam[4],m_pStack[i].lParam[5],m_pStack[i].lParam[6],m_pStack[i].lParam[7],m_pStack[i].lParam[8]);
			else
			{
				assert(m_pStack[i].nParamCount == 10);
				RelTrace("%d) %s 参数(0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X)",	i,m_pStack[i].pFuncName,m_pStack[i].lParam[0],m_pStack[i].lParam[1],m_pStack[i].lParam[2],m_pStack[i].lParam[3],m_pStack[i].lParam[4],m_pStack[i].lParam[5],m_pStack[i].lParam[6],m_pStack[i].lParam[7],m_pStack[i].lParam[8],m_pStack[i].lParam[9]);
			}
		}
	}

	struct StRecordCallStackAuto;
	friend struct ICallStackTrack::StRecordCallStackAuto;

	struct StRecordCallStackAuto
	{
		ICallStackTrack*m_pTrack;
		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 0;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 1;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 2;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pStack[pTrack->m_nCount].lParam[1]	= l1;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 3;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pStack[pTrack->m_nCount].lParam[1]	= l1;
			pStack[pTrack->m_nCount].lParam[2]	= l2;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 4;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pStack[pTrack->m_nCount].lParam[1]	= l1;
			pStack[pTrack->m_nCount].lParam[2]	= l2;
			pStack[pTrack->m_nCount].lParam[3]	= l3;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 5;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pStack[pTrack->m_nCount].lParam[1]	= l1;
			pStack[pTrack->m_nCount].lParam[2]	= l2;
			pStack[pTrack->m_nCount].lParam[3]	= l3;
			pStack[pTrack->m_nCount].lParam[4]	= l4;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 6;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pStack[pTrack->m_nCount].lParam[1]	= l1;
			pStack[pTrack->m_nCount].lParam[2]	= l2;
			pStack[pTrack->m_nCount].lParam[3]	= l3;
			pStack[pTrack->m_nCount].lParam[4]	= l4;
			pStack[pTrack->m_nCount].lParam[5]	= l5;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5,long l6)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 7;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pStack[pTrack->m_nCount].lParam[1]	= l1;
			pStack[pTrack->m_nCount].lParam[2]	= l2;
			pStack[pTrack->m_nCount].lParam[3]	= l3;
			pStack[pTrack->m_nCount].lParam[4]	= l4;
			pStack[pTrack->m_nCount].lParam[5]	= l5;
			pStack[pTrack->m_nCount].lParam[6]	= l6;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5,long l6,long l7)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 8;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pStack[pTrack->m_nCount].lParam[1]	= l1;
			pStack[pTrack->m_nCount].lParam[2]	= l2;
			pStack[pTrack->m_nCount].lParam[3]	= l3;
			pStack[pTrack->m_nCount].lParam[4]	= l4;
			pStack[pTrack->m_nCount].lParam[5]	= l5;
			pStack[pTrack->m_nCount].lParam[6]	= l6;
			pStack[pTrack->m_nCount].lParam[7]	= l7;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5,long l6,long l7,long l8)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 9;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pStack[pTrack->m_nCount].lParam[1]	= l1;
			pStack[pTrack->m_nCount].lParam[2]	= l2;
			pStack[pTrack->m_nCount].lParam[3]	= l3;
			pStack[pTrack->m_nCount].lParam[4]	= l4;
			pStack[pTrack->m_nCount].lParam[5]	= l5;
			pStack[pTrack->m_nCount].lParam[6]	= l6;
			pStack[pTrack->m_nCount].lParam[7]	= l7;
			pStack[pTrack->m_nCount].lParam[8]	= l8;
			pTrack->m_nCount++;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5,long l6,long l7,long l8,long l9)
		{
			m_pTrack = pTrack;
			pTrack->_EnterBlock(pFunc);
			ICallStackTrack::StStackItem *pStack = pTrack->m_pStack;
			
			pStack[pTrack->m_nCount].nParamCount= 10;
			pStack[pTrack->m_nCount].lParam[0]	= l0;
			pStack[pTrack->m_nCount].lParam[1]	= l1;
			pStack[pTrack->m_nCount].lParam[2]	= l2;
			pStack[pTrack->m_nCount].lParam[3]	= l3;
			pStack[pTrack->m_nCount].lParam[4]	= l4;
			pStack[pTrack->m_nCount].lParam[5]	= l5;
			pStack[pTrack->m_nCount].lParam[6]	= l6;
			pStack[pTrack->m_nCount].lParam[7]	= l7;
			pStack[pTrack->m_nCount].lParam[8]	= l8;
			pStack[pTrack->m_nCount].lParam[9]	= l9;
			pTrack->m_nCount++;
		}

		~StRecordCallStackAuto()
		{
			m_pTrack->_LeaveBlock();
		}
	};
};

//****************************************************************************************************

#if	defined(CALL_STACK_TRACK_NEED_RECORD) && ( defined(_WIN32) || defined(_DEBUG) )

#define RECORD_STACK_TRACK(funname)									ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname)
#define RECORD_STACK_TRACK0(funname)								ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname)
#define RECORD_STACK_TRACK1(funname,l0)								ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0))
#define RECORD_STACK_TRACK2(funname,l0,l1)							ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1))
#define RECORD_STACK_TRACK3(funname,l0,l1,l2)						ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2))
#define RECORD_STACK_TRACK4(funname,l0,l1,l2,l3)					ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3))
#define RECORD_STACK_TRACK5(funname,l0,l1,l2,l3,l4)					ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4))
#define RECORD_STACK_TRACK6(funname,l0,l1,l2,l3,l4,l5)				ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5))
#define RECORD_STACK_TRACK7(funname,l0,l1,l2,l3,l4,l5,l6)			ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5),long(l6))
#define RECORD_STACK_TRACK8(funname,l0,l1,l2,l3,l4,l5,l6,l7)		ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5),long(l6),long(l7))
#define RECORD_STACK_TRACK9(funname,l0,l1,l2,l3,l4,l5,l6,l7,l8)		ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5),long(l6),long(l7),long(l8))
#define RECORD_STACK_TRACK10(funname,l0,l1,l2,l3,l4,l5,l6,l7,l8,l9)	ICallStackTrack::StRecordCallStackAuto _record_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5),long(l6),long(l7),long(l8),long(l9))

#else

#define RECORD_STACK_TRACK(funname)								
#define RECORD_STACK_TRACK0(funname)							
#define RECORD_STACK_TRACK1(funname,l0)							
#define RECORD_STACK_TRACK2(funname,l0,l1)						
#define RECORD_STACK_TRACK3(funname,l0,l1,l2)					
#define RECORD_STACK_TRACK4(funname,l0,l1,l2,l3)				
#define RECORD_STACK_TRACK5(funname,l0,l1,l2,l3,l4)				
#define RECORD_STACK_TRACK6(funname,l0,l1,l2,l3,l4,l5)			
#define RECORD_STACK_TRACK7(funname,l0,l1,l2,l3,l4,l5,l6)		
#define RECORD_STACK_TRACK8(funname,l0,l1,l2,l3,l4,l5,l6,l7)	
#define RECORD_STACK_TRACK9(funname,l0,l1,l2,l3,l4,l5,l6,l7,l8)		
#define RECORD_STACK_TRACK10(funname,l0,l1,l2,l3,l4,l5,l6,l7,l8,l9)	

#endif

#endif
