#ifndef TYPDELIST20091210_H_INLUCDE__
#define TYPDELIST20091210_H_INLUCDE__

namespace mmo_server
{

struct TypeList0 {
    typedef TypeList0 _Parent;
};

template< class _A1 >
struct TypeList1
{
    typedef TypeList1<_A1> _Parent;
    typedef _A1 _Result;
};

template< class _A1, class _A2 >
struct TypeList2 : public TypeList1<_A2>
{
    typedef TypeList1<_A2> _Parent;
    typedef _A1 _Result;
};

template< class _A1, class _A2, class _A3 >
struct TypeList3 : public TypeList2<_A2, _A3>
{
    typedef TypeList2<_A2, _A3> _Parent;
    typedef _A1 _Result;
};

template< class _A1, class _A2, class _A3, class _A4 >
struct TypeList4 : public TypeList3<_A2, _A3, _A4>
{
    typedef TypeList3<_A2, _A3, _A4> _Parent;
    typedef _A1 _Result;
};

template <typename _TList, unsigned int i>
struct _TypeAt
{
    typedef typename _TypeAt<typename _TList::_Parent, i-1>::_Result _Result;
};

template <>
struct _TypeAt<TypeList0, 0>
{
    typedef TypeList0 _Result;
};

template <class _TList>
struct _TypeAt<_TList, 0>
{
    typedef typename _TList::_Result _Result;
};

} //namespace mmo_server
#endif //TYPDELIST20091210_H_INLUCDE__
