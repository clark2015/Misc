#ifndef ICALLSTACKTRACK_H__
#define ICALLSTACKTRACK_H__
#include "IUnknownEx.h"
/*
	堆栈追踪，用于在Release版或其它不方便debug的情况下随时Trace堆栈的调用过程
	由于要跨模块运行，所以要继承IUnknownEx，
	由于要照顾运行效率，所以尽量不用虚函数且要inline

	2008-02-02/03/04	alan 建立文件，完成功能
	2009-12-15			alan v3.0,作成堆栈历史的形式
*/

static const CLASSUID	IF_UID_ICALLSTACKTRACK ("ICallStackTrack");
static const uint32		IF_VER_ICALLSTACKTRACK = INTERFACE_VER(3,0);

class ICallStackTrack : public IUnknownEx
{
	struct StStackItem
	{
		enum {	MAX_PARAM_COUNT	= 10	};	
		const char*	pFuncName;
		int			nParamCount;
		long		lParam[MAX_PARAM_COUNT];
		int 		nDeep;		//当前深度
		StStackItem*pNext;
	};
	typedef StStackItem*	StStackItemPtr;

protected:	
	int				m_nDeep;
	int				m_nStorage;

	StStackItem**	m_pStack;
	StStackItem*	m_pHistoryHead;
	StStackItem*	m_pHistoryTail;
	int				m_nHistoryCount;
	enum	{	HISTORY_COUNT	= 30	};
protected:
	virtual void AllowStackItem()
	{
		return new StStackItem;
	}
	virtual void ReAllowStack()
	{
		if( NULL == m_pStack )
		{
			assert(m_nStorage == 0  && NULL == m_pHistory);
			m_nStorage = 2;
			m_pStack   = new StStackItemPtr[m_nStorage];
		}
		else
		{
			m_nStorage *= 2;
			StStackItem**ps = new StStackItemPtr[m_nStorage];
			memcpy(ps,m_pStack,	 sizeof(StStackItem*)*m_nStorage/2);
			delete []m_pStack;
			m_pStack	= ps;
		}
		DbgTrace("ICallStackTrack::m_nStorage = %d",m_nStorage);
	}

	inline StStackItem* _EnterBlock(const char*pFunc)
	{
		assert(pFunc);
		if ( m_nDeep >= m_nStorage )
		{
			ReAllow();
		}
		StStackItem*pItem;
		if ( m_nHistoryCount > HISTORY_COUNT )
		{
			assert(m_pHistoryHead);
			pItem = m_pHistoryHead;
			m_pHistoryHead = m_pHistoryHead->pNext;
			m_nHistoryCount--;
		}
		else
		{
			pItem = AllowStackItem();
		}

		pItem->pFuncName = pFunc;
		pItem->nDeep	 = m_nDeep;
//		pItem->pNext	 = NULL;	//反正这里不赋值，_LeaveBlock()时也要赋	//不用，用m_pHistoryTail来判断结束就行了
		m_pStack[m_nDeep]= pItem;
		m_nDeep++;
		return pItem;
	}

	inline void _LeaveBlock()
	{
		assert(m_nDeep > 0 );
		m_nDeep--;
		StStackItem*pItem = m_pStack[m_nDeep];
		if ( m_pHistoryTail )
		{
			m_pHistoryTail->pNext = pItem;
			m_pHistoryTail = pItem;
		}
		else
		{
			assert(m_pHistoryHead == NULL);
			m_pHistoryHead =
			m_pHistoryTail = pItem;
		}
	}
	
	void TraceItem(StStackItem*pItem,bool bLeadSpace = false)
	{
		if( NULL == pItem || NULL == pItem->pFuncName )
		{
			RelTrace("NULL == pItem or NULL == pFuncName ???");
			return;
		}

		char szSpace[256];
		if ( bLeadSpace )
		{		
			int nDeep = pItem->nDeep;
			int i;
			for (i=0; i<nDeep && i< sizeof(szSpace)-10; i+=2)
			{
				szSpace[i  ] = ' ';
				szSpace[i+1] = ' ';
			}
			szSpace[i] = 0;
		}
		else
		{
			szSpace[0] = 0;
		}
		
		if ( pItem->nParamCount == 0 )
			RelTrace("%d) %s%s",									pItem->nDeep,szSpace,pItem->pFuncName);
		else if ( pItem->nParamCount == 1 )
			RelTrace("%d) %s%s 参数(%X)",							pItem->nDeep,szSpace,pItem->pFuncName,pItem->lParam[0]);
		else if ( pItem->nParamCount == 2 )
			RelTrace("%d) %s%s 参数(%X,%X)",						pItem->nDeep,szSpace,pItem->pFuncName,pItem->lParam[0],pItem->lParam[1]);
		else if ( pItem->nParamCount == 3 )
			RelTrace("%d) %s%s 参数(%X,%X,%X)",						pItem->nDeep,szSpace,pItem->pFuncName,pItem->lParam[0],pItem->lParam[1],pItem->lParam[2]);
		else if ( pItem->nParamCount == 4 )
			RelTrace("%d) %s%s 参数(%X,%X,%X,%X)",					pItem->nDeep,szSpace,pItem->pFuncName,pItem->lParam[0],pItem->lParam[1],pItem->lParam[2],pItem->lParam[3]);
		else if ( pItem->nParamCount == 5 )
			RelTrace("%d) %s%s 参数(%X,%X,%X,%X,%X)",				pItem->nDeep,szSpace,pItem->pFuncName,pItem->lParam[0],pItem->lParam[1],pItem->lParam[2],pItem->lParam[3],pItem->lParam[4]);
		else if ( pItem->nParamCount == 6 )
			RelTrace("%d) %s%s 参数(%X,%X,%X,%X,%X,%X)",			pItem->nDeep,szSpace,pItem->pFuncName,pItem->lParam[0],pItem->lParam[1],pItem->lParam[2],pItem->lParam[3],pItem->lParam[4],pItem->lParam[5]);
		else if ( pItem->nParamCount == 7 )
			RelTrace("%d) %s%s 参数(%X,%X,%X,%X,%X,%X,%X)",			pItem->nDeep,szSpace,pItem->pFuncName,pItem->lParam[0],pItem->lParam[1],pItem->lParam[2],pItem->lParam[3],pItem->lParam[4],pItem->lParam[5],pItem->lParam[6]);
		else if ( pItem->nParamCount == 8 )
			RelTrace("%d) %s%s 参数(%X,%X,%X,%X,%X,%X,%X,%X)",		pItem->nDeep,szSpace,pItem->pFuncName,pItem->lParam[0],pItem->lParam[1],pItem->lParam[2],pItem->lParam[3],pItem->lParam[4],pItem->lParam[5],pItem->lParam[6],pItem->lParam[7]);
		else if ( pItem->nParamCount == 9 )
			RelTrace("%d) %s%s 参数(%X,%X,%X,%X,%X,%X,%X,%X,%X)",	pItem->nDeep,szSpace,pItem->pFuncName,pItem->lParam[0],pItem->lParam[1],pItem->lParam[2],pItem->lParam[3],pItem->lParam[4],pItem->lParam[5],pItem->lParam[6],pItem->lParam[7],pItem->lParam[8]);
		else
		{
			assert(pItem->nParamCount == 10);
			RelTrace("%d) %s%s 参数(%X,%X,%X,%X,%X,%X,%X,%X,%X)",	pItem->nDeep,pItem->pFuncName,pItem->lParam[0],pItem->lParam[1],pItem->lParam[2],pItem->lParam[3],pItem->lParam[4],pItem->lParam[5],pItem->lParam[6],pItem->lParam[7],pItem->lParam[8],pItem->lParam[9]);
		}			
	}
public:
	ICallStackTrack()
	{
		m_nDeep			= 0;
		m_nStorage		= 0;
		m_pStack		= NULL;
		m_pHistoryHead	= NULL;
		m_pHistoryTail	= NULL;
		m_nHistoryCount	= 0;
	}
	~ICallStackTrack()
	{
		int i;
		for (i=0 ; i<m_nDeep; i++)
			delete m_pStack[i];
		delete []m_pStack;

		if ( m_pHistoryHead )
		{
			StStackItem*pItem = m_pHistoryHead;
			for ( ; pItem != m_pHistoryTail; )
			{
				StStackItem*temp = pItem->pNext;
				delete pItem;
				pItem = temp;
			}
			delete m_pHistoryTail;
		}
	}
	
	virtual void* QueryInterface(const CLASSUID & cuid,ulong ulVer)
	{
		if ( cuid == IF_UID_ICALLSTACKTRACK && IsVersionFit(IF_VER_ICALLSTACKTRACK,ulVer) )
		{
			ICallStackTrack*pThis = this;
			return pThis;
		}

		return 	IUnknownEx::QueryInterface(cuid,ulVer);
	}
	
	void TraceStack()
	{
		if ( 0 == m_nStorage )
			return;
		int i;
		for (i=0; i<m_nDeep; i++)
		{
			TraceItem(m_pStack[i],false);
		}

		RelTrace("堆栈历史:");
		if ( m_pHistoryHead )
		{
			StStackItem*pItem = m_pHistoryHead;
			for ( ; pItem != m_pHistoryTail; )
			{
				TraceItem(pItem,true);
			}
			TraceItem(m_pHistoryTail,true);
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
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 0;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 1;
			pItem->lParam[0]	= l0;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 2;
			pItem->lParam[0]	= l0;
			pItem->lParam[1]	= l1;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 3;
			pItem->lParam[0]	= l0;
			pItem->lParam[1]	= l1;
			pItem->lParam[2]	= l2;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 4;
			pItem->lParam[0]	= l0;
			pItem->lParam[1]	= l1;
			pItem->lParam[2]	= l2;
			pItem->lParam[3]	= l3;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 5;
			pItem->lParam[0]	= l0;
			pItem->lParam[1]	= l1;
			pItem->lParam[2]	= l2;
			pItem->lParam[3]	= l3;
			pItem->lParam[4]	= l4;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 6;
			pItem->lParam[0]	= l0;
			pItem->lParam[1]	= l1;
			pItem->lParam[2]	= l2;
			pItem->lParam[3]	= l3;
			pItem->lParam[4]	= l4;
			pItem->lParam[5]	= l5;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5,long l6)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 7;
			pItem->lParam[0]	= l0;
			pItem->lParam[1]	= l1;
			pItem->lParam[2]	= l2;
			pItem->lParam[3]	= l3;
			pItem->lParam[4]	= l4;
			pItem->lParam[5]	= l5;
			pItem->lParam[6]	= l6;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5,long l6,long l7)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 8;
			pItem->lParam[0]	= l0;
			pItem->lParam[1]	= l1;
			pItem->lParam[2]	= l2;
			pItem->lParam[3]	= l3;
			pItem->lParam[4]	= l4;
			pItem->lParam[5]	= l5;
			pItem->lParam[6]	= l6;
			pItem->lParam[7]	= l7;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5,long l6,long l7,long l8)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
			
			pItem->nParamCount	= 9;
			pItem->lParam[0]	= l0;
			pItem->lParam[1]	= l1;
			pItem->lParam[2]	= l2;
			pItem->lParam[3]	= l3;
			pItem->lParam[4]	= l4;
			pItem->lParam[5]	= l5;
			pItem->lParam[6]	= l6;
			pItem->lParam[7]	= l7;
			pItem->lParam[8]	= l8;
		}

		StRecordCallStackAuto(ICallStackTrack*pTrack,const char*pFunc,long l0,long l1,long l2,long l3,long l4,long l5,long l6,long l7,long l8,long l9)
		{
			m_pTrack = pTrack;
			ICallStackTrack::StStackItem *pItem = pTrack->_EnterBlock(pFunc);
				
			pItem->nParamCount	= 10;
			pItem->lParam[0]	= l0;
			pItem->lParam[1]	= l1;
			pItem->lParam[2]	= l2;
			pItem->lParam[3]	= l3;
			pItem->lParam[4]	= l4;
			pItem->lParam[5]	= l5;
			pItem->lParam[6]	= l6;
			pItem->lParam[7]	= l7;
			pItem->lParam[8]	= l8;
			pItem->lParam[9]	= l9;
		}

		~StRecordCallStackAuto()
		{
			m_pTrack->_LeaveBlock();
		}
	};
};

//****************************************************************************************************

#ifdef CALL_STACK_TRACK_NEED_RECORD

#define RECORD_STACK_TRACK(funname)									ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname)
#define RECORD_STACK_TRACK0(funname)								ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname)
#define RECORD_STACK_TRACK1(funname,l0)								ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0))
#define RECORD_STACK_TRACK2(funname,l0,l1)							ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1))
#define RECORD_STACK_TRACK3(funname,l0,l1,l2)						ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2))
#define RECORD_STACK_TRACK4(funname,l0,l1,l2,l3)					ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3))
#define RECORD_STACK_TRACK5(funname,l0,l1,l2,l3,l4)					ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4))
#define RECORD_STACK_TRACK6(funname,l0,l1,l2,l3,l4,l5)				ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5))
#define RECORD_STACK_TRACK7(funname,l0,l1,l2,l3,l4,l5,l6)			ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5),long(l6))
#define RECORD_STACK_TRACK8(funname,l0,l1,l2,l3,l4,l5,l6,l7)		ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5),long(l6),long(l7))
#define RECORD_STACK_TRACK9(funname,l0,l1,l2,l3,l4,l5,l6,l7,l8)		ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5),long(l6),long(l7),long(l8))
#define RECORD_STACK_TRACK10(funname,l0,l1,l2,l3,l4,l5,l6,l7,l8,l9)	ICallStackTrack::StRecordCallStackAuto _recore_stack_trace(MY_CALL_STACK_TRACK_OBJ,funname,long(l0),long(l1),long(l2),long(l3),long(l4),long(l5),long(l6),long(l7),long(l8),long(l9))

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
