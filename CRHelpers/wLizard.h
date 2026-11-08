//////////////////////////////////////////////////////////////////////////
// JMuf's "LIst wiZARD"
//////////////////////////////////////////////////////////////////////////

#ifndef __LIZARD__  
#define __LIZARD__  

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DECLARE_LIST(MyClass) class Myclass; \
class MyClass##RefList:public CObList										\
{																										\
 private:																								\
 BOOL m_bDestroyable;																				\
 protected:																								\
 MyClass##RefList(BOOL bDestroyable):m_bDestroyable(bDestroyable){}								\
 public:                                                                                                \
 MyClass *& GetHead() 		{return (MyClass*&)CObList::GetHead();}                                     \
 MyClass*  GetHead() const	{return (MyClass*) CObList::GetHead();}                                     \
 MyClass*& GetTail()		{return (MyClass*&)CObList::GetTail();}                                     \
 MyClass*  GetTail() const	{return (MyClass*) CObList::GetTail();}                                     \
                                                                                                        \
 MyClass* RemoveHead()		{return (MyClass*) CObList::RemoveHead();}                                  \
 MyClass* RemoveTail()		{return (MyClass*) CObList::RemoveTail();}                                  \
                                                                                                        \
 MyClass*& GetNext(POSITION& rPosition)			{return (MyClass*&) CObList::GetNext(rPosition);}       \
 MyClass* GetNext(POSITION& rPosition) const	{return (MyClass*)  CObList::GetNext(rPosition);}       \
 MyClass*& GetPrev(POSITION& rPosition)			{return (MyClass*&) CObList::GetPrev(rPosition);}       \
 MyClass* GetPrev(POSITION& rPosition) const	{return (MyClass*)  CObList::GetPrev(rPosition);}       \
                                                                                                        \
 MyClass*& GetAt(POSITION rPosition)				{return (MyClass*&) CObList::GetAt(rPosition);}     \
 MyClass* GetAt(POSITION rPosition) const		{return (MyClass*)  CObList::GetAt(rPosition);}			\
																										\
 BOOL Has(MyClass*p) const { return Find(p)!=NULL;}														\
																										\
 void DestroyAll()																						\
	{																									\
	 while (!IsEmpty()) 																				\
	 {																									\
		CObject *p = RemoveHead();																		\
		POSITION pos;																					\
		do																								\
		{																								\
		  pos = Find(p);																				\
		  if (pos) RemoveAt(pos);																		\
		} while (pos);																					\
		if (m_bDestroyable) delete p;																	\
	 }																									\
	}																									\
																										\
	BOOL Remove(MyClass *p){																			\
		POSITION pos=Find(p);																			\
		if (pos) {																						\
			RemoveAt(pos);																				\
			if (m_bDestroyable) delete p;																\
			return TRUE;																				\
		}																								\
		return FALSE;																					\
	}																									\
};																										\
																										\
class MyClass##List: public MyClass##RefList															\
{																										\
 public:																								\
 MyClass##List(BOOL bDestroyable=TRUE):MyClass##RefList(bDestroyable){}									\
 ~MyClass##List() {DestroyAll();}																		\
};

#endif //__LIZARD__  
