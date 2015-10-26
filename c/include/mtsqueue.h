#include <list>
#include <queue>
#include <assert.h>

//���̰߳�ȫ���Ƚ��ȳ�����
template  <class _element_type>
class mtsqueue  
	{
	protected:
		std::list<_element_type>	m_List;
		CLock m_Lock;
		CSem m_sem;

	public:
		//���б�ǰȡ,�б����ȴ�
		_element_type PopFront()
			{
			_element_type retVal;
			m_sem.Down();
			m_Lock.Lock();
			assert( ! m_List.empty() );
			retVal = m_List.front();
			m_List.pop_front();
		//	retVal = front();
		//	pop();
			m_Lock.Unlock();
			return retVal;
			}
			
		//���б��������
		void Append(const _element_type& p)
			{
			m_Lock.Lock();
			m_List.push_back(p);
		//	push(p);
			m_Lock.Unlock();
			m_sem.Up();
			}

		//���б�ͷ����
		void Add(const _element_type& p)
			{
			m_Lock.Lock();
			m_List.push_front(p);
			m_Lock.Unlock();
			m_sem.Up();
			}
		
		int Size()
			{
			m_Lock.Lock();	//��Ϊlist::size()���ܻ�һ����������
			int r = m_List.size();
			m_Lock.Unlock();
			return r;
			}
	};
/*
template  <class _element_type>
class mtsqueue  : private std::list<_element_type>
	{
	protected:
		CLock m_Lock;
		CSem m_sem;

	public:
		//���б�ǰȡ,�б����ȴ�
		_element_type PopFront()
			{
			_element_type retVal;
			m_sem.Down();
			m_Lock.Lock();
			assert( ! empty() );
			retVal = front();
			pop_front();
		//	retVal = front();
		//	pop();
			m_Lock.Unlock();
			return retVal;
			}
			
		//���б��������
		void Append(_element_type p)
			{
			m_Lock.Lock();
			push_back(p);
		//	push(p);
			m_Lock.Unlock();
			m_sem.Up();
			}

		//���б�ͷ����
		void Add(_element_type p)
			{
			m_Lock.Lock();
			push_front(p);
			m_Lock.Unlock();
			m_sem.Up();
			}
		
		int Size()
			{
			m_Lock.Lock();	//��Ϊlist::size()���ܻ�һ����������
			int r = size();
			m_Lock.Unlock();
			return r;
			}
	};
*/
typedef  mtsqueue<void *> CMTSQueue;

