// edreact.h
/////////////////////////////////////////////////////////

// editor-reactor-definitions
//
// ATTENTION: do not modify this code.
//
#include <aced.h>

/////////////////////////////////////////////////////////
// functions for CEditorReactor

//declaration of CEditorReactor
class CEditorReactor : public AcEditorReactor
{
public:
	void commandWillStart(const char*);
private:
	BOOL IsCmdReqdReaction(const CString&);
public:
	static void StartEditorReactor();
	static void EndEditorReactor();
};

