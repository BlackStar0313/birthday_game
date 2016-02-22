#include <nsl/ngHashMapIterator.h>
#include <nsl/ngHashMap.h>
#include <nsl/ngHashSet.h>

ngHashMapIterator::ngHashMapIterator(ngHashMap* pMap)
:ngHashSetIterator(pMap->GetEntrySet())
{
}

ngHashMapIterator::~ngHashMapIterator()
{
}

void ngHashMapIterator::Destroy()
{
	DELETE_OBJECT(this, ngHashMapIterator);
}