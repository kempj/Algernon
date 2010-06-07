#ifndef G6TOOLS
#define G6TOOLS

namespace g6
{

	long long int GetSize (const char* g6);
	bool** GetMatrix (const char* g6);
	


	long long int GetSize (const char* g6)
	{

		long long unsigned int size = 0;
		int index = 0;

        if(g6[0]<126 && g6[0]>62)
        {
            size = g6[0]-63;
			return size;
        }else if(g6[1]<126 && g6[0]>62)
        {
            size = (g6[1]-63);
            for(int i=2; i<4; i++)
            {
				if(g6[i]<63)
					return 0;//unexpected end of string
                size <<= 6;
                size = (g6[i]-63) | size;//note: the eight bit of asccii is alway 1
				//size = ((g6[i]-63) | 127) | size;//safe version?
			}
			return size;

        }else if(g6[2]>62)
        {
            size = (g6[2]-63) | size;
            for(int i=3; i<8; i++)
            {
				if(g6[i]<63)
					return 0;//unexpected end of string
				size <<= 6;
                size = (g6[i]-63) | size;
            }
			return size;
        }
		return 0; //unexpected end of string
	};
	bool** GetMatrix (const char* g6)
	{
		long long unsigned int size = GetSize(g6);
		if(size == 0)
			return NULL; //bad data

		bool** matrix;

		long long unsigned int index;
		if(g6[0]!='~')
			index = 1;
		else if(g6[1]!='~')
			index = 4;
		else
			index = 8;
		
		char bit=32;//values 1,2,4,8,16,32
		matrix = new bool*[size];
		for(long long unsigned int i =0; i< size; i++)
		{
			matrix[i] = new bool[size];
			matrix[i][i] = false;
			for(long long unsigned int j =0; j< i; j++)
			{
				if(((g6[index]-63)&bit)==bit)
				{
					matrix[j][i] = true;
					matrix[i][j] = true;
				}
				else
				{
					matrix[j][i] = false;
					matrix[i][j] = false;
				}
				if(bit>1)
					bit >>= 1;
				else
				{
					bit = 32;
					index++;
				}
				
			}
		}
	return matrix;
	}
}

#endif
