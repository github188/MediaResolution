#ifndef StrUtility_H
#define StrUtility_H
//#include "Global.h"
#include <iostream>
#include "Markup.h"
#include <map>
#include <deque>
#include <vector>

using namespace std;

namespace ty
{
	class StrUtility
	{
	public:
		std::string path[20];
		static int Atoi(string snum, size_t maxlen=5, int defaultret=0, int radix=10 );
	    bool ForeachXml(CMarkup& xml, int level = 0);
		bool GetxmlValue(std::string key, string &value);
		bool GetxmlValue(std::string key, int &value);
		bool GetxmlValue(std::string key, deque<string> &valuelist); 
		void ClearMap();
		static bool StringSplit(std::string src, std::string pattern, std::vector<std::string>& strVec);
		static int replace_all(std::string& str,  const std::string& pattern,  const std::string& newpat);
		static void GetTicket(std::string& ticket, int len);
	private:
		std::multimap<string, string>  XmlList_Map;
	};
}



#endif
