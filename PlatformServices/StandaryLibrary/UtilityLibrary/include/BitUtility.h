#ifndef _TY_Common_BitUtility_
#define _TY_Common_BitUtility_

#include "map"
using namespace std;

namespace TY
{
	namespace Common
	{
		class BitUtility
		{
			//×Ö¶Î
		private:
			unsigned char val;

		private:
			static const map<int,int> data;
			static map<int,int> CreateMap()
			{
				map<int,int> m;
				m[1] = 1;
				m[2] = 3;
				m[3] = 7;
				m[4] = 15;
				m[5] = 31;
				m[6] = 63;
				m[7] = 127;
				return m;
			}

		public:
			BitUtility(unsigned char val)
			{
				this->val=val;
			}

		public:
			int GetOneBit(int num);
			int GetMultiBit(int startnum, int endnum);
			void SetVal(unsigned char newval)
			{
				this->val=newval;
			}
		};
	}
}

#endif
