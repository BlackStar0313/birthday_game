#include <ncal/ngConnectionManager.h>
#include <ncal/ngConnectionSession.h>
#include <nsl/ngListIterator.h>

void ngConnectionManager::Initialize() {
	//TODO: code here if need.
}

void ngConnectionManager::HandleUpdate(int32 deltaMS)
{
	ngListIterator* pIterator = (ngListIterator *)m_sessions.Iterator();
	while(pIterator->HasNext()) {
		ngConnectionSession* pSession = (ngConnectionSession *)pIterator->Get();
		// if the current session is useless, remove it from list.
		//pSession->HandleUpdate(deltaMS);

		if (pSession->IsUseless() == TRUE) {
			pIterator->Remove();
			SAFE_DELETE(pSession);
		}else {
			pIterator->Next();
		}
		
	}
	SAFE_DELETE(pIterator);  
}

void ngConnectionManager::OnNewConnectionSession(ngConnectionSession* pSession) {
    NGASSERT(!m_sessions.Contains(pSession) && "[ngConnectionManager] duplicated session!~");
    m_sessions.Add(pSession);
}

#pragma mark - ngSingleton

ngConnectionManager* ngConnectionManager::m_pThis = NULL;

ngConnectionManager* ngConnectionManager::GetInstance() {
	if (m_pThis == NULL) {
		m_pThis = DNEW(ngConnectionManager);
        m_pThis->Initialize();
	}

	return m_pThis;
}

void ngConnectionManager::ReleaseInstance(void *pObj) {
    ngConnectionManager* pThis = (ngConnectionManager*)pObj;
    NGASSERT(pThis == m_pThis && "[ngConnectionManager] something wrong!~");
    DELETE_OBJECT(pThis, ngConnectionManager);
    m_pThis = NULL;
}

ngConnectionManager::ngConnectionManager()
: ngSingleton(ReleaseInstance, this) {
    
}

ngConnectionManager::~ngConnectionManager() {
}

