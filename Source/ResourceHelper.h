// resourcehelper.h
/////////////////////////////////////////////////////////

#ifndef __RESOURCE_HELPER__H__
#define __RESOURCE_HELPER__H__

class CTemporaryResourceOverride
{
public:
  //CTemporaryResourceOverride(HINSTANCE hInstNew);
  CTemporaryResourceOverride(); // default construction

  virtual ~CTemporaryResourceOverride();

  static void SetDefaultResource(HINSTANCE hInstNew);

public:
  static HINSTANCE m_hInstanceDefault;

private:
  void   CommonConstruction(HINSTANCE);

  HINSTANCE m_hInstanceOld;
};

#endif // __RESOURCE_HELPER__H__
