#pragma once

#ifndef TypeLibApp
extern "C" 
{
	long CreateMegaObject( const char * sInterfaceName, const char * sClassName, void ** pObject );
	long ReleaseMegaObject( void * pObject );
};

#endif
#include <assert.h>
template < class T>
class CMegaObjectT
{
	T * m_pT;
public:
	CMegaObjectT() : m_pT( NULL )
	{
	}

	CMegaObjectT( const char * sInterfaceName, const char * sClassName ) : m_pT( NULL )
	{
		Create( sInterfaceName, sClassName );
	}
	~CMegaObjectT()
	{
		if ( m_pT )
		{
			::ReleaseMegaObject( m_pT );
			m_pT = NULL;
		}
	}
	bool Create( const char * sInterfaceName, const char * sClassName )
	{
		assert( !m_pT );
		::CreateMegaObject( sInterfaceName, sClassName, (void**)&m_pT );
		if ( !m_pT )
		{
			char sMsg[64] = {0};
			sprintf_s( sMsg, 63, "创建%s失败", sClassName );
			::MessageBox( NULL, sMsg, NULL, MB_OK );
			return false;
		}

		return true;
	}

	bool CreateByAttrib( const char * sInterfaceName, const char * sAttrib, const char * sValue )
	{
		MegaClient::IInterfaceList * p = NULL;
		CreateMegaObject( "IInterfaceList", sInterfaceName, (void**)&p );

		std::string sClassName;
		if ( p )
		{
			for ( long i = 0; sClassName == "" && i < p->GetCount(); i++ )
			{
				MegaClient::IInterface * pInterface = p->GetClass(i);
				std::string strValue = pInterface->GetValue( sAttrib );
				if ( strValue == sValue )
				{
					sClassName = pInterface->GetClassName();
					break;
				}
			}
		}
		ReleaseMegaObject( p );

		if ( sClassName == "" )
			return false;

		return Create( sInterfaceName, sClassName.c_str() );
	}

	bool CreateByCheck( const char * sInterfaceName, const char * sKey, const char * sValue )
	{
		MegaClient::IInterfaceList * p = NULL;
		CreateMegaObject( "IInterfaceList", sInterfaceName, (void**)&p );

		std::string sClassName;
		if ( p )
		{
			for ( long i = 0; sClassName == "" && i < p->GetCount(); i++ )
			{
				MegaClient::IInterface * pInterface = p->GetClass(i);
				std::string sCheck = pInterface->GetValue( sKey );
				const char * sBegin = sCheck.c_str();

				std::string sTemp;
				for ( long j = 0; ; j++ )
				{
					if ( sBegin[j] == '|' || sBegin[j] == '\0' )
					{
						// Check
						if ( sTemp == sValue )
						{
							sClassName = pInterface->GetClassName();
							break;
						}

						sTemp = "";

						if ( sBegin[j] == '\0' )	// 循环结束
							break;
					}
					else 
					{
						sTemp += sBegin[j];
					}
				}
			}
		}
		ReleaseMegaObject( p );

		if ( sClassName == "" )
			return false;

		return Create( sInterfaceName, sClassName.c_str() );
	}

	T * operator ->()
	{
		assert( m_pT );
		return m_pT;
	}
	operator T*()
	{
		assert( m_pT );
		return m_pT;
	}
	T ** operator &()
	{
		return &m_pT;
	}
	operator bool()
	{
		return m_pT != NULL;
	}

	void operator =( CMegaObjectT<T> & p )
	{
		assert( !m_pT );
		m_pT = p.m_pT;
		p.m_pT = NULL;
	}
};

