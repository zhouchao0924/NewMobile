
#pragma  once

#include <vector>
#include <string>
#include "ISerialize.h"

class kString
{
public:
	kString() { _wchar = false;}

	const char *c_str()
	{
		if (!_wchar && !_data.empty())
		{
			return &_data[0];
		}
		return "";
	}

	const wchar_t *w_str()
	{
		if (_wchar && !_data.empty())
		{
			return (wchar_t *)(&_data[0]);
		}
		return L"";
	}

	kString(const char *str)
	{
		if (str)
		{
			size_t len = strlen(str) + 1;
			_data.resize(len);
			memcpy(data(), str, len);
			_data.back() = 0;
			_wchar = false;
		}
	}

	bool operator == (const kString &v)
	{
		if (_wchar != v._wchar)
		{
			return false;
		}

		if (_data.size() != v._data.size())
		{
			return false;
		}

		return memcmp(data(), v.data(), size()) == 0;
	}

	size_t length()
	{
		return _wchar ? (_data.size() / 2 - 1) : (_data.size() - 1);
	}

	void *data() const { return (void  *)&_data[0]; }

	size_t size() { return _data.size(); }

	bool isWstr() { return _wchar; }

	void reset()
	{
		if (_data.empty())
		{
			if (_wchar)
			{
				_data.resize(2);
			}
			else
			{
				_data.resize(1);
			}
		}

		size_t i = _data.size();
		if (_wchar)
		{
			_data[i - 2] = 0;
		}
		_data[i - 1] = 0;
	}

	friend inline ISerialize & operator << (ISerialize &Ar, kString &v);
private:
	bool	_wchar;
	std::vector<char> _data;
};

inline ISerialize & operator << (ISerialize &Ar, kString &v)
{
	if (Ar.IsLoading())
	{
		int Len = 0;
		Ar << Len;
		bool LoadUCS2Char = Len < 0;
		if (LoadUCS2Char)
		{
			Len = -Len;
		}

		if (Len > 0)
		{
			if (LoadUCS2Char)
			{
				v._wchar = true;
				v._data.resize(2*Len);
				Ar.Serialize(v.data(), v.size());
			}
			else
			{
				v._wchar = false;
				v._data.resize(Len);
				Ar.Serialize(v.data(), v.size());
			}
			v.reset();
		}
	}
	else if(Ar.IsSaving())
	{
		int len = v.length() + 1;
		
		int flagLen = len;

		if (v.isWstr())
		{
			flagLen = -len;
		}

		Ar << flagLen;
	
		if (len > 0)
		{
			if (v.size() <= 0)
			{
				v.reset();
			}
			Ar.Serialize(v.data(), v.size());
		}
	}

	return Ar;
}



