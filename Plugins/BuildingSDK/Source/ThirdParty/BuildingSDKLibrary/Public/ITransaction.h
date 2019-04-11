
#ifndef __AI_ITRANSACTION_H__
#define __AI_ITRANSACTION_H__

class IAtomTask
{
public:
	virtual ~IAtomTask() {}
	virtual bool  TestDo() = 0;
	virtual bool  TestUnDo() = 0;
	virtual void  Do() = 0;
	virtual void  UnDo() = 0;
	virtual class ISubTransactionSystem *GetSystem() = 0;
};

class ITransactionListener
{
public:
	virtual ~ITransactionListener() {}
	virtual void AfterExecDo(IAtomTask *Task) = 0;
	virtual void AfterExecUnDo(IAtomTask *Task) = 0;
};

class ISubTransactionSystem
{
public:
	virtual ~ISubTransactionSystem() {}
	virtual void OnBeginTransaction(class ITransact *Transaction) = 0;
	virtual void OnEndTransaction(class ITransact *Transaction) = 0;
	virtual void Serialize(IAtomTask *&pTask, class ISerialize &Ar) = 0;
};

class ITransact
{
public:
	virtual ~ITransact() {}
	virtual void BeginTransaction() = 0;
	virtual void EndTransaction() = 0;
	virtual void Clear() = 0;
	virtual void Redo() = 0;
	virtual void Undo() = 0;
	virtual int GetRedoCount() = 0;
	virtual int GetUndoCount() = 0;
	virtual void PushTask(IAtomTask *Task) = 0;
	virtual void AddListener(ITransactionListener *Listener) = 0;
	virtual void RemoveListener(ITransactionListener *Listener) = 0;
	virtual void AddSubSystem(ISubTransactionSystem *SubSystem) = 0;
	virtual void RemoveSubSystem(ISubTransactionSystem *SubSystem) = 0 ;
};

#endif //__AI_ITRANSACTION_H__


