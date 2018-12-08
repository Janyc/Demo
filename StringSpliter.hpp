#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

namespace nsShare_Inc
{
	class CStringSpliter
	{
	public:
		static	void Split( const string& str,vector<string>& vecStr,char chSpliter )
		{
			std::stringstream  ssTemp;
			ssTemp.str(str);
			string	strTemp;
			vecStr.clear();
			int iPart = 0;
			while(getline(ssTemp,strTemp,chSpliter))
			{
				vecStr.push_back(strTemp);
			}
		}

		static void Split( const string& str,map<string,string>& mapStr,char chSpliter1,char chSpliter2 )
		{
			std::stringstream  ssTemp;
			ssTemp.str(str);
			string	strName,strValue;
			mapStr.clear();
			while(getline(ssTemp,strName,chSpliter1)&&getline(ssTemp,strValue,chSpliter2))
			{//:名称=值 &
				mapStr[strName] = strValue;
			}
		}

		static void Split_KeyToLower( const string& str,map<string,string>& mapStr,char chSpliter1,char chSpliter2 )
		{
			std::stringstream  ssTemp;
			ssTemp.str(str);
			string	strName,strValue;
			mapStr.clear();
			while(getline(ssTemp,strName,chSpliter1)&&getline(ssTemp,strValue,chSpliter2))
			{//:名称=值 &
				transform(strName.begin(),strName.end(),strName.begin(),tolower);
				mapStr[strName] = strValue;
			}
		}
	};
}