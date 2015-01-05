#include "StrUtility.h"
namespace ty
{

int StrUtility::Atoi( std::string snum, size_t maxlen, int defaultret, int radix )
{
    if ( 10 == radix )
    {
        if ( !snum.empty() && snum.size()<=maxlen )
            return atoi( snum.c_str() );
    }
    else if ( 16 == radix )
    {
        if ( !snum.empty() && snum.size()<=maxlen )
        {
            int num=0;
            sscanf( snum.c_str(), "%x", &num );
            return num;
        }
    }

    return defaultret;
}

bool StrUtility::ForeachXml(CMarkup& xml,int level)
{
    while(xml.FindElem())
    {
        std::string strTagName = xml.GetTagName();
        if(level>=19) return false;
        path[level] = strTagName;
        std::string strAttribName;
        std::string strAttrib;
        std::string key;
        for(int k=0; k<=level; k++)
        {
            key+=path[k];
            if(k!=level) key+="/";
        }
        for(int nAttributeIndex=0;; nAttributeIndex++)
        {
            strAttribName=xml.GetAttribName(nAttributeIndex);
            if(strAttribName.length()!=0)
            {
                strAttrib=xml.GetAttrib(strAttribName.c_str());
                XmlList_Map.insert(make_pair(key+"_"+strAttribName,strAttrib));
            }
            else
                break;
        };
        std::string strData = xml.GetData();
        if(strData.length()!=0)
        {
            XmlList_Map.insert(make_pair(key,strData));
        }
        if(xml.IntoElem())
        {
            level++;
            ForeachXml(xml,level);
            xml.OutOfElem();
            level--;
        }
    }
    return true;
}

bool StrUtility::GetxmlValue(std::string key,std::string &value)
{
    std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> ppp;
    ppp = XmlList_Map.equal_range(key);
    if(ppp.first==ppp.second) return false;
    value = ppp.first->second;
    return true;
}

bool StrUtility::GetxmlValue(std::string key, int &value)
{
    std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> ppp;
    ppp = XmlList_Map.equal_range(key);
    if(ppp.first==ppp.second) return false;
    value=atoi(ppp.first->second.c_str());
    return true;
}

bool StrUtility::GetxmlValue(std::string key,std::deque< std::string > &valuelist)
{

    std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> ppp;
    ppp = XmlList_Map.equal_range(key);
    if(ppp.first==ppp.second) return false;
    for(std::multimap<std::string,std::string>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
    {
        valuelist.push_back(it2->second);
    }
    return true;
}

bool StrUtility::StringSplit(std::string src, std::string pattern, std::vector<std::string>& strVec)
{
    std::string::size_type pos;
    src +=pattern;//扩展字符串以方便操作
    int size=src.size();

    for(int i=0; i<size; i++)
    {
        pos = src.find(pattern,i);
        if(pos<size)
        {
            std::string s=src.substr(i,pos-i);
            strVec.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return true;
}

int StrUtility::replace_all(std::string& str,  const std::string& pattern,  const std::string& newpat)
{
    int count = 0;
    const size_t nsize = newpat.size();
    const size_t psize = pattern.size();

    for(size_t pos = str.find(pattern, 0);
            pos != std::string::npos;
            pos = str.find(pattern,pos + nsize))
    {
        str.replace(pos, psize, newpat);
        count++;
    }

    return count;
}

void StrUtility::ClearMap()
{
    XmlList_Map.clear();
}

void StrUtility::GetTicket(string& ticket, int nLen)
{
    // 定义产生验证流所使用的字符集
    static char sChSet[] = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for(int j = 0; j < nLen - 1; j++)// gene 31 rand char
    {
        int n = rand() % (sizeof(sChSet) - 1);
        //ticket[j] = sChSet[n];
        ticket.append(sChSet + n, 1);
    }
}

}
