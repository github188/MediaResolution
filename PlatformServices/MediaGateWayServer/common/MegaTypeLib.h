#pragma once

// 对象类型定义
#define SINGLE_REF	"SINGLE_REF"// 单实例，单引用，此种类型只有一个实例，并且只能被引用一次，当多次引用时，会引用失败
#define MULTI_REF	"MULTI_REF"	// 单实例，多引用，此种类型只有一个实例，可以引用多次，对引用者来说，就像使用多个对象一样
#define MULTI_OBJ	"MULTI_OBJ"	// 多实例，每次引用都会创建一个实例出来

#define CREATEMEGAOBJECT( INTFACE, INTFACE_NAME, CLASS, CLASS_NAME, CLASS_TYPE ) \
	if ( 0 == strcmp( sInterfaceName, INTFACE_NAME ) \
		&& 0 == strcmp( sClassName, CLASS_NAME ) )\
	{\
		if ( std::string( CLASS_TYPE ) == SINGLE_REF )\
		{\
			std::vector< std::pair< std::string, void *> >::iterator pos;\
			for ( pos = g_DataArray.begin(); pos != g_DataArray.end(); pos++ )\
				if ( pos->first == INTFACE_NAME )\
					return -1;\
			MegaClient::INTFACE * p = new CLASS;\
			g_DataArray.push_back( std::make_pair( INTFACE_NAME, p ) );\
			*pObject = p;\
		}\
		else if ( std::string( CLASS_TYPE ) == MULTI_REF )\
		{\
			bool bFound = false;\
			std::vector< std::pair< std::string, void *> >::iterator pos;\
			for ( pos = g_DataArray.begin(); pos != g_DataArray.end(); pos++ )\
			{\
				if ( pos->first == INTFACE_NAME )\
				{\
					bFound = true;\
					break;\
				}\
			}\
			if ( !bFound )\
			{\
				MegaClient::INTFACE * p = new CLASS;\
				g_DataArray.push_back( std::make_pair( INTFACE_NAME, p ) );\
				*pObject = p;\
				g_DataRef[ INTFACE_NAME ] = 1;\
			}\
			else\
			{\
				*pObject = pos->second;\
				g_DataRef[ INTFACE_NAME ]++;\
			}\
			return 0;\
		}\
		else if ( std::string( CLASS_TYPE ) == MULTI_OBJ )\
		{\
			MegaClient::INTFACE * p = new CLASS;\
			g_DataArray.push_back( std::make_pair( INTFACE_NAME, p ) );\
			*pObject = p;\
			return 0;\
		}\
	}

#define DELETEMEGAOBJECT( INTFACE, INTFACE_NAME, CLASS, CLASS_NAME, CLASS_TYPE ) \
	if ( pos->first == INTFACE_NAME )\
	{\
		if ( std::string( CLASS_TYPE ) == SINGLE_REF || std::string( CLASS_TYPE ) == MULTI_OBJ )\
		{\
			delete (CLASS*)(MegaClient::INTFACE*)pObject;\
			g_DataArray.erase( pos );\
		}\
		else if ( std::string( CLASS_TYPE ) == MULTI_REF )\
		{\
			g_DataRef[ INTFACE_NAME ]--;\
			if ( g_DataRef[ INTFACE_NAME ] <= 0 )\
			{\
				delete (CLASS*)(MegaClient::INTFACE*)pObject;\
				g_DataArray.erase( pos );\
			}\
		}\
	}
;

namespace MegaClient
{
	class IInterface
	{
	public:
		virtual const char * GetClassName() = 0;
		virtual const char * GetInterfaceName() = 0;
		virtual const char * GetComment() = 0;
		virtual long GetCount() = 0;
		virtual const char * GetAttrib( long index ) = 0;
		virtual const char * GetValue( long index ) = 0;
		virtual const char * GetValue( const char * sName ) = 0;
	};

	class IInterfaceList
	{
	public:
		virtual long GetCount() = 0;
		virtual IInterface * GetClass( long lIndex ) = 0;
	};

	class ILog
	{
	public:
		virtual void OutPut( const char * sInterfaceName, const char * sClassName, const char * sFormat, ... ) = 0;
	};

	class ILogManager
	{
	public:
		virtual long SetLog( ILog * pLog ) = 0;
	};
};

#include <vector>
#include <string>
#include <assert.h>
class CInterface : public MegaClient::IInterface
{
	std::vector< std::pair< std::string, std::string > > m_vAttrib;
public:
	CInterface()
	{
	}
	CInterface( const char * sAttribTable[][2] )
	{
		for ( long i = 0; sAttribTable[i][0] != NULL; i++ )
			m_vAttrib.push_back( std::make_pair( sAttribTable[i][0], sAttribTable[i][1] ) );
	};

	bool AddAttrib( const char * sName, const char * sValue )
	{
		std::vector< std::pair< std::string, std::string > >::iterator pos;
		for ( pos = m_vAttrib.begin(); pos != m_vAttrib.end(); pos++ )
			if ( pos->first == sName )
				return false;
		m_vAttrib.push_back( std::make_pair( sName, sValue ) );
		return true;
	}

	const char * GetClassName()
	{
		std::vector< std::pair< std::string, std::string > >::iterator pos;
		for ( pos = m_vAttrib.begin(); pos != m_vAttrib.end(); pos++ )
			if ( pos->first == "Name" )
				return pos->second.c_str();
		return 	"";
	}
	const char * GetInterfaceName()
	{
		std::vector< std::pair< std::string, std::string > >::iterator pos;
		for ( pos = m_vAttrib.begin(); pos != m_vAttrib.end(); pos++ )
			if ( pos->first == "Interface" )
				return pos->second.c_str();
		return 	"";
	}
	const char * GetComment()
	{
		std::vector< std::pair< std::string, std::string > >::iterator pos;
		for ( pos = m_vAttrib.begin(); pos != m_vAttrib.end(); pos++ )
			if ( pos->first == "Comment" )
				return pos->second.c_str();
		return 	"";
	}

	long GetCount()
	{
		return (long)m_vAttrib.size();
	}

	const char * GetAttrib( long index )
	{
		if ( index >= 0 && index < (long)m_vAttrib.size() )
			return m_vAttrib[ index ].first.c_str();
		return 	"";
	}

	const char * GetValue( long index )
	{
		if ( index >= 0 && index < (long)m_vAttrib.size() )
			return m_vAttrib[ index ].second.c_str();
		return 	"";
	}

	const char * GetValue( const char * sName )
	{
		std::vector< std::pair< std::string, std::string > >::iterator pos;
		for ( pos = m_vAttrib.begin(); pos != m_vAttrib.end(); pos++ )
			if ( pos->first == sName )
				return pos->second.c_str();
		return 	"";
	}
};

