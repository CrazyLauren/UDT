// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CConfig.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 20.01.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <boost/config.hpp>
#include <algorithm>
#include <stack>




#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

typedef boost::property_tree::basic_ptree<std::string, std::string,
		std::less<std::string> > ptree_t;

namespace NSHARE
{
template<class T>
	class CBufferWriter :NSHARE::CDenyCopying
	{
	public:
		CBufferWriter(T& aBuf) :
			FBuf(aBuf)
		{
		}


		void Put(char c)
		{
			FBuf.push_back((typename T::value_type)c);
		}
		void PutUnsafe(char c)
		{
			FBuf.push_back((typename T::value_type)c);
		}
		void PutN(char c, size_t n)
		{
			FBuf.insert(FBuf.end(), n, (typename T::value_type)c);
		}

		void Flush()
		{
		}
		char Peek() const { CHECK(false); return 0; }
		char Take() { CHECK(false); return 0; }
		size_t Tell() const { CHECK(false); return 0; }
		char* PutBegin() { CHECK(false); return 0; }
		size_t PutEnd(char*) { CHECK(false); return 0; }
	private:
		T& FBuf;
	};
}
namespace rapidjson
{
	template<class T>
	inline void PutUnsafe(NSHARE::CBufferWriter<T>& stream, char c)
	{
		stream.PutUnsafe(c);
	}
}

#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/memorystream.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
namespace NSHARE
{
const NSHARE::CText CConfig::XML_VALUE_NAME = "value";
static size_t base64_encode(NSHARE::CText& _dest, const char* src, size_t aLen);
CConfig::data_t::data_t(const CText& key) :
		FKey(key)
{
}

CConfig::data_t::data_t(const CText& key, const CText& value) :
		FKey(key), //
		FValue(value)
{

}

CConfig::CConfig(const CText& key, void const* aTo, size_t aMaxLen) :
		FData(data_t(key))
{
	base64_encode(FData.MWrite().FValue, (char const*) aTo, aMaxLen);
}
CConfig::CConfig(const CText& key, CBuffer const & aTo) :
		FData(data_t(key))
{
	base64_encode(FData.MWrite().FValue, (char const*) aTo.ptr_const(),
			aTo.size());
}
CConfig::CConfig(const CText& key, const CConfig& rhs):
		FData(rhs.FData)
{
	FData.MWrite().FKey=key;
}

CConfig::CConfig(const CConfig& rhs) :
		FData(rhs.FData)
{
}

CConfig& CConfig::operator=(const CConfig& rhs)
{
	if (&rhs != this)
		FData = rhs.FData;
	return *this;
}
CConfig::~CConfig()
{
}
const char UNIX_PATH_SEPARATOR = '/';
const char WINDOWS_PATH_SEPARATOR = '\\';

bool is_relative(const CText& fileName)
{
#if defined(_WIN32)  && !defined(__CYGWIN__)
	CText native(fileName);

	CText::size_type slash = 0;
	while ((slash = native.find_first_of(UNIX_PATH_SEPARATOR, slash))
			!= CText::npos)
	{
		native[slash] = WINDOWS_PATH_SEPARATOR;
	}
	//"C:\data"
	if (native.size() >= 3 && native[1] == ':' && native[2] == '\\')
		return false;
	//"\\server1\data"
	if (native.size() >= 2 && native[0] == '\\' && native[1] == '\\')
		return false;
	return true;
#else
	CText native(fileName);

	CText::size_type slash = 0;
	while ((slash = native.find_first_of(WINDOWS_PATH_SEPARATOR, slash))
			!= CText::npos)
	{
		native[slash] = UNIX_PATH_SEPARATOR;
	}
	return !(native.size() >= 1 && native[0] == '/');
#endif
}
CText concatPaths(const CText& left, const CText& right)
{
#if defined(WIN32) && !defined(__CYGWIN__)
	const char delimiterNative = WINDOWS_PATH_SEPARATOR;
	const char delimiterForeign = UNIX_PATH_SEPARATOR;
#else
	const char delimiterNative = UNIX_PATH_SEPARATOR;
	const char delimiterForeign = WINDOWS_PATH_SEPARATOR;
#endif

	if (left.empty())
	{
		return (right);
	}
	char lastChar = left[left.size() - 1];

	if (lastChar == delimiterNative)
	{
		return left + right;
	}
	else if (lastChar == delimiterForeign)
	{
		return left.substr(0, left.size() - 1) + delimiterNative + right;
	}
	else // lastChar != a delimiter
	{
		return left + delimiterNative + right;
	}
}
CText getFullPath(const CText& relativeTo, const CText &relativePath)
{
	if (!is_relative(relativePath) || relativeTo.empty())
		return relativePath;

	if (relativePath.empty())
		return relativeTo;

	//---Concatinate the paths together
	CText filename;
	CText::size_type slash = relativeTo.find_last_of("/\\");
	if (slash != std::string::npos)
		filename = CText(relativeTo, 0, slash);

	filename = concatPaths(filename, relativePath);

	std::list<CText> directories;
	size_t start = 0;
	for (size_t i = 0; i < filename.size(); ++i)
	{
		if (filename[i] == '\\' || filename[i] == '/')
		{
			//Get the current directory
			CText dir = filename.substr(start, i - start);

			if (dir != "..")
			{
				if (dir != ".")
				{
					directories.push_back(dir);
				}
			}
			else if (!directories.empty())
			{
				directories.pop_back();
			}
			start = i + 1;
		}
	}

	CText path;
	for (std::list<CText>::iterator itr = directories.begin();
			itr != directories.end(); ++itr)
	{
		path += *itr;
		path += "/";
	}

	path += filename.substr(start, CText::npos);
	return path;
}
CConfig const& CConfig::MChild(const CText& childName) const
{
	data_t const& _data = FData.MRead();
	VLOG(6) << "Find child:" << childName << "; Number of children:"
						<< _data.FChildren.size();
	for (ConfigSet::const_iterator i = _data.FChildren.begin();
			i != _data.FChildren.end(); ++i)
	{
		VLOG(6) << "Child " << i->MKey();
		if (i->MKey() == childName)
		{
			VLOG(6) << "Found " << childName << " in " << MKey();
			return *i;
		}
	}

	return sMGetEmpty();
}
static uint8_t g_buffer[sizeof(CConfig)+__alignof(CConfig)];
CConfig const& CConfig::sMGetEmpty()
{
	static CConfig const& emptyConf=*new (get_alignment_address<CConfig>(g_buffer)) CConfig;///< allocate to static memory
	return emptyConf;
}
const CConfig* CConfig::MChildPtr(const CText& childName) const
{
	for (ConfigSet::const_iterator i = FData.MRead().FChildren.begin();
			i != FData.MRead().FChildren.end(); ++i)
	{
		if (i->MKey() == childName)
			return &(*i);
	}
	return NULL;
}

CConfig* CConfig::MMutableChild(const CText& childName)
{
	for (ConfigSet::iterator _it = FData.MWrite().FChildren.begin();
			_it != FData.MWrite().FChildren.end(); ++_it)
	{
		if (_it->MKey() == childName)
			return &(*_it);
	}

	return NULL;
}

void CConfig::MMerge(const CConfig& rhs)
{
	for (ConfigSet::const_iterator _it = rhs.FData.MRead().FChildren.begin();
			_it != rhs.FData.MRead().FChildren.end(); ++_it)
	{
		MRemove(_it->MKey());
		MAdd(*_it);
	}

}
void CConfig::MBlendWith(const CConfig& rhs)
{
	for (ConfigSet::const_iterator _it = rhs.FData.MRead().FChildren.begin();
			_it != rhs.FData.MRead().FChildren.end(); ++_it)
	{
		if (_it->MIsSimple())
		{

		}
		else
		{
			if (MIsChild(_it->MKey()))
			{
				MMutableChild(_it->MKey())->MBlendWith(*_it);
			}
			else
				MAdd(*_it);
		}
	}
}
const CConfig* CConfig::MFind(const CText& key) const
{
	if (key == this->MKey())
		return this;
	for (ConfigSet::const_iterator _it = FData.MRead().FChildren.begin();
			_it != FData.MRead().FChildren.end(); ++_it)
		if (key == _it->MKey())
			return &(*_it);

	for (ConfigSet::const_iterator _it = FData.MRead().FChildren.begin();
			_it != FData.MRead().FChildren.end(); ++_it)
	{
		const CConfig* r = _it->MFind(key);
		if (r)
			return r;
	}

	return NULL;
}

CConfig* CConfig::MFind(const CText& key)
{
	if (key == this->MKey())
		return this;
	for (ConfigSet::iterator _it = FData.MWrite().FChildren.begin();
			_it != FData.MWrite().FChildren.end(); ++_it)
		if (key == _it->MKey())
			return &(*_it);

	for (ConfigSet::iterator _it = FData.MWrite().FChildren.begin();
			_it != FData.MWrite().FChildren.end(); ++_it)
	{
		CConfig* r = _it->MFind(key);
		if (r)
			return r;
	}

	return NULL;
}
CConfig CConfig::operator -(const CConfig& rhs) const
{
	CConfig result(*this);
	ConfigSet::const_iterator _it = rhs.MChildren().begin(), _it_end(
			rhs.MChildren().end());
	for (; _it != _it_end; ++_it)
	{
		result.MRemove(_it->MKey());
	}

	return result;
}
std::ostream& CConfig::MPrint(std::ostream & aStream) const
{
	data_t const& _data = FData.MRead();
	aStream << "Key:" << _data.FKey;
	if (!_data.FValue.empty())
		aStream << "; Value:" << _data.FValue<<":EndValue; ";
	if (!_data.FChildren.empty())
	{
		aStream << "; Child of " << _data.FKey;
		for (ConfigSet::const_iterator _it = _data.FChildren.begin();
				_it != _data.FChildren.end(); ++_it)
			aStream << std::endl << *_it;
		aStream << std::endl << "; End child of  " << _data.FKey;
	}
	return aStream;
}
template<>
inline void CConfig::MReadFrom(ptree_t const& aTree, bool aFirst)
{
	if (aTree.empty() && aTree.data().empty())
	{
		VLOG(2) << "Node empty";
		return;
	}
	else
	{
		if (aFirst)
		{
			data_t &_data = FData.MWrite();
			_data.FKey = aTree.front().first;
			_data.FValue = aTree.data();
			MReadFrom(aTree.front().second, false);
		}
		else
		{
			ptree_t::const_iterator _it = aTree.begin();
			for (; _it != aTree.end(); ++_it)
			{
				//VLOG(2) << "Property =" << _it->first << " :" << this;
				ptree_t::value_type const& _val = *_it;
				if (_val.second.empty()) //leaf
				{
					VLOG(3) << "Add leaf " << _val.first << " = "
										<< _val.second.data() << " :" << this;
					MAdd(_val.first, _val.second.data());
				}
				else if (_val.first == "<xmlattr>") //node
				{
					VLOG(2) << "Convert xmlattr";
					CConfig _child(_val.first, _val.second.data());
					_child.MReadFrom(_val.second, false);
					ConfigSet const& _set = _child.MChildren();
					ConfigSet::const_iterator _jt(_set.begin()), _jt_end(
							_set.end());
					for (; _jt != _jt_end; ++_jt)
					{
						VLOG(2) << "Push attr " << (_jt)->MKey() << " == "
											<< (_jt)->MValue();
						MAdd(*_jt);
					}
				}
				else //node
				{
					VLOG(2) << "Node =" << _val.first << " :" << this;
					CConfig _child(_val.first, _val.second.data());
					_child.MReadFrom(_val.second, false);
					if (!_child.MIsEmpty())
						MAdd(_child);
				}
			}
			if (!aTree.data().empty())
			{
				CConfig _child(XML_VALUE_NAME, aTree.data());
				MAdd(_child);
			}
		}
		VLOG(2) << "End reading";
	}

}
template<>
inline void CConfig::MWriteTo(ptree_t& aTree, bool aFirst) const
{
	if (MIsEmpty())
	{
		VLOG(2) << "Node empty";
		return;
	}
	VLOG(2) << "Write " << MKey();
	if (MIsSimple())
	{
		VLOG(2) << "Put leaf " << MKey() << " = " << MValue();
		aTree.add(MKey().MToStdString(), MValue().MToStdString());
		return;
	}
	if (aFirst)
	{
		LOG_IF(DFATAL,MKey().empty()) << "Empty key not allowed";
		CText _path;
		if (MKey().empty())
			_path = "EmptyKeyNotAllowed";
		else
			_path = MKey();
		ptree_t & _node = aTree.add(_path.c_str(), "");
		MWriteTo(_node, false);
		VLOG(2) << "Write  ptree finished";
	}
	else
	{
		ConfigSet::const_iterator _it = FData.MRead().FChildren.begin();
		for (; _it != FData.MRead().FChildren.end(); ++_it)
		{
			if (_it->MIsEmpty())
			{
				VLOG(2) << "Node empty";
				continue;
			}
			CConfig const& _conf = *_it;

			LOG_IF(DFATAL,_conf.MKey().empty()) << "Empty key not allowed";

			CText const _path(
					!_conf.MKey().empty() ?
							_conf.MKey() : CText("EmptyKeyNotAllowed"));
			if (_it->MIsSimple())
			{
				VLOG(6) << "Put leaf " << _path << " = " << _conf.MValue();
				aTree.add(_path.c_str(), _conf.MValue().MToStdString());
			}
			else
			{

				VLOG(6) << "Property =" << _path << " :" << this;
				ptree_t & _node = aTree.add(_path.c_str(), "");
				_conf.MWriteTo(_node, false);
			}
		}
	}
}

namespace
{
using namespace rapidjson;
struct CJsonReader
{
	struct _stack_t
	{
		_stack_t()
		{
			FConf = NULL;
			FCurrentArrayLength = 0;
			FNumberOfArrayElements = 0;
		}
		NSHARE::CConfig* FConf;
		NSHARE::CText FArrayKey;
		unsigned FCurrentArrayLength;
		unsigned FNumberOfArrayElements;
	};

	NSHARE::CConfig& FHead;
	NSHARE::CText FKey;
	std::stack<_stack_t> FStack;

	CJsonReader(NSHARE::CConfig& aTo) :
			FHead(aTo)
	{
	}
	~CJsonReader()
	{
	}
	bool Null()
	{
        MAddLeaf(NSHARE::CText());
		return true;
	}
	bool Bool(bool b)
	{
		MAddLeaf(NSHARE::to_string(b));
		return true;
	}
	bool Int(int i)
	{
		MAddLeaf(NSHARE::to_string(i));
		return true;
	}
	bool Uint(unsigned u)
	{
		MAddLeaf(NSHARE::to_string(u));
		return true;
	}
	bool Int64(int64_t i)
	{
		MAddLeaf(NSHARE::to_string(i));
		return true;
	}
	bool Uint64(uint64_t u)
	{
		MAddLeaf(NSHARE::to_string(u));
		return true;
	}
	bool Double(double d)
	{
		MAddLeaf(NSHARE::to_string(d));
		return true;
	}
	bool RawNumber(const char* str, SizeType length, bool copy)
	{

		MAddLeaf(NSHARE::CText(str/*, length*/));
		return true;
	}
	bool String(const char* str, SizeType length, bool copy)
	{

		MAddLeaf(NSHARE::CText(str/*, length*/));///bug fix lengh - is size
		return true;
	}

	void MAddLeaf(NSHARE::CText const& aVal)
	{
		VLOG(6) << "Add leaf " << FKey << " = " << aVal << " to "
							<< FStack.top().FConf->MKey();
		if (!FStack.top().FArrayKey.empty())
		{
			FStack.top().FConf->MAdd(FStack.top().FArrayKey, aVal);
			++FStack.top().FCurrentArrayLength;
		}
		else
			FStack.top().FConf->MSet(FKey, aVal);
	}
	bool StartObject()
	{
		if (FStack.empty())
		{
			VLOG(6) << "Heading ";
			_stack_t _stack;
			_stack.FConf = &FHead;
			FStack.push(_stack);
			return true;
		}
		if (!FStack.top().FArrayKey.empty())
		{
			VLOG(6) << "StartObject() For Array " << FStack.top().FArrayKey;

			NSHARE::CConfig _conf(FStack.top().FArrayKey);

			FStack.top().FConf->MAdd(_conf);
			++FStack.top().FCurrentArrayLength;

			_stack_t _stack;
			_stack.FConf = &FStack.top().FConf->MChildren().back();
			FStack.push(_stack);
		}
		else
		{
			VLOG(6) << "StartObject() For Object";
			VLOG(6) << "Add new object " << FKey;
			NSHARE::CConfig _conf(FKey);
			FStack.top().FConf->MAdd(_conf);
			_stack_t _stack;
			_stack.FConf = &FStack.top().FConf->MChildren().back();
			FStack.push(_stack);
		}
		return true;
	}
	bool Key(const char* str, SizeType length, bool copy)
	{
		VLOG(6) << "Key(" << str << ", " << length << ", " << copy << ")";
		FKey = NSHARE::CText(str, length);
		//FStack.top()->M
		return true;
	}
	bool EndObject(SizeType memberCount)
	{
		if (!FStack.empty())
		{
			VLOG(6) << "EndObject(" << memberCount << ") for "
								<< FStack.top().FConf->MKey();
			CHECK_EQ(FStack.top().FConf->MChildren().size(),
					memberCount + FStack.top().FNumberOfArrayElements);
			FStack.pop();
		}
		return true;
	}

	bool StartArray()
	{
		VLOG(6) << "StartArray() for " << FStack.top().FConf->MKey()
							<< " Key " << FKey << " Count = "
							<< FStack.top().FCurrentArrayLength;
		FStack.top().FArrayKey = FKey;
		FStack.top().FCurrentArrayLength = 0;
		CHECK(!FKey.empty());
		return true;
	}
	bool EndArray(SizeType elementCount)
	{
		VLOG(6) << "EndArray(" << elementCount << ") for "
							<< FStack.top().FConf->MKey() << " Key " << FKey;

		FStack.top().FArrayKey.clear();
		CHECK_EQ(FStack.top().FCurrentArrayLength, elementCount);

		FStack.top().FNumberOfArrayElements +=
				FStack.top().FCurrentArrayLength > 0 ?
						FStack.top().FCurrentArrayLength - 1 : 0;
		FStack.top().FCurrentArrayLength = 0;
		return true;
	}
};

template<class T>
void write_json_impl(T & writer, NSHARE::CConfig const& aFrom, bool aIsArray);

void write_json_to_pretty(NSHARE::CConfig const& aFrom, NSHARE::CText* aTo)
{
	StringBuffer s;
	PrettyWriter<StringBuffer> writer(s);
	writer.StartObject();
	write_json_impl(writer, aFrom, false);
	writer.EndObject();
	aTo->assign(s.GetString());
}
void write_json_to_pretty(NSHARE::CConfig const& aFrom, std::ostream& aStream)
{
	OStreamWrapper s(aStream);
	PrettyWriter<OStreamWrapper> writer(s);
	writer.StartObject();
	write_json_impl(writer, aFrom, false);
	writer.EndObject();
}

void write_json_to(NSHARE::CConfig const& aFrom, NSHARE::CBuffer& aTo)
{
	typedef std::vector<NSHARE::CBuffer::value_type> _buf_t;
	_buf_t _vec;
	CBufferWriter<_buf_t> s(_vec);
	Writer<CBufferWriter<_buf_t> > writer(s);
	writer.StartObject();
	write_json_impl(writer, aFrom, false);
	writer.EndObject();
	aTo=NSHARE::CBuffer( _vec.begin(), _vec.end());
}
void write_json_to(NSHARE::CConfig const& aFrom, NSHARE::CText* aTo)
{
	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
	write_json_impl(writer, aFrom, false);
	writer.EndObject();
	aTo->assign(s.GetString());
}
void write_json_to(NSHARE::CConfig const& aFrom, std::ostream& aStream)
{
	OStreamWrapper s(aStream);
	Writer<OStreamWrapper> writer(s);
	writer.StartObject();
	write_json_impl(writer, aFrom, false);
	writer.EndObject();
}
template<class T>
void write_json_impl(T & writer, NSHARE::CConfig const& aFrom, bool aIsArray)
{

	if (aFrom.MIsEmpty())
	{
		VLOG(2) << "Node empty";
		return;
	}
	VLOG(6) << "Write " << aFrom.MKey();
	if (aFrom.MIsSimple())
	{
		VLOG(6) << "Put leaf " << aFrom.MKey() << " = " << aFrom.MValue();
		if (!aIsArray)
			writer.Key(aFrom.MKey().c_str());
		writer.String(aFrom.MValue().c_str());
		return;
	}
	else
	{
		bool _is = !aFrom.MKey().empty();
		if (_is)
		{
			if (!aIsArray)
				writer.Key(aFrom.MKey().c_str());
			writer.StartObject();
		}
		typedef std::map<NSHARE::CText, ConfigSet> _arrays_t;
		typedef std::map<NSHARE::CText, unsigned> _counter_t;

		ConfigSet const& _childs = aFrom.MChildren();

		ConfigSet _objects;
		_arrays_t _arrays;
		_counter_t _counter;
		{
			NSHARE::ConfigSet::const_iterator _it = _childs.begin();
			for (; _it != _childs.end(); ++_it)
			{
				_counter[_it->MKey()]++;
			}
			_it = _childs.begin();
			for (; _it != _childs.end(); ++_it)
			{
				if (_counter[_it->MKey()] > 1)
					_arrays[_it->MKey()].push_back(*_it);
				else
					_objects.push_back(*_it);
			}
		}
		{
			ConfigSet::const_iterator _it;
			_it = _objects.begin();
			for (; _it != _objects.end(); ++_it)
			{
				NSHARE::CConfig const& _conf = (*_it);
				if (_conf.MIsEmpty())
				{
					VLOG(2) << "Node empty";
					continue;
				}

				LOG_IF(DFATAL,_conf.MKey().empty()) << "Empty key not allowed";
				VLOG(6) << "put object " << _conf.MKey();
				write_json_impl(writer, _conf, false);
			}
		}
		_arrays_t::const_iterator _jt = _arrays.begin();

		for (; _jt != _arrays.end(); ++_jt)
		{
			ConfigSet::const_iterator _it(_jt->second.begin()), _it_end(
					_jt->second.end());
			VLOG(2) << "put array " << _jt->first;
			writer.Key(_jt->first.c_str());
			writer.StartArray();
			for (; _it != _it_end; ++_it)
			{
				NSHARE::CConfig const& _conf = (*_it);
				if (_conf.MIsEmpty())
				{
					VLOG(2) << "Node empty";
					continue;
				}

				LOG_IF(DFATAL,_conf.MKey().empty()) << "Empty key not allowed";

				write_json_impl(writer, _conf, true);
			}
			writer.EndArray();
		}

		if (_is)
			writer.EndObject();

	}
}
}
bool CConfig::MFromJSON(std::istream& aStream)
{
	Reader _reader;
	IStreamWrapper _buf(aStream);

	CJsonReader handler(*this);

	return _reader.Parse<kParseCommentsFlag | kParseTrailingCommasFlag>(_buf,
			handler);
}
bool CConfig::MFromJSON(NSHARE::CBuffer const& aBuf)
{
	if (aBuf.empty())
	{
		LOG(ERROR)<<"Empty string.";
		return false;
	}
	Reader _reader;
	MemoryStream _buf((char*)aBuf.ptr_const(),aBuf.size());

	CJsonReader handler(*this);

	return _reader.Parse<kParseCommentsFlag | kParseTrailingCommasFlag>(_buf,
			handler);
}
bool CConfig::MFromJSON(NSHARE::CText const& aText)
{
	if (aText.empty())
	{
		DLOG(ERROR)<<"Empty string.";
		return false;
	}
	Reader _reader;
	StringStream _buf(aText.c_str());

	CJsonReader handler(*this);

	return _reader.Parse<kParseCommentsFlag | kParseTrailingCommasFlag>(_buf,
			handler);
}
NSHARE::CText CConfig::MToJSON(bool aPretty) const
{
	NSHARE::CText _rval;
	MToJSON(_rval, aPretty);
	return _rval;
}
bool CConfig::MToJSON(NSHARE::CText& aText, bool aPretty) const
{
	if (aPretty)
		write_json_to_pretty(*this, &aText);
	else
		write_json_to(*this, &aText);
	return true;
}
bool CConfig::MToJSON(NSHARE::CBuffer& aVal) const
{
	write_json_to(*this, aVal);
	return true;
}
bool CConfig::MToJSON(std::ostream& aStream, bool aPretty) const
{
	if (aPretty)
		write_json_to_pretty(*this, aStream);
	else
		write_json_to(*this, aStream);
	return true;
}
bool CConfig::MToXML(std::ostream& aStream, bool aPretty) const
{
	ptree_t _tree;
	MWriteTo(_tree);

#if (BOOST_VERSION / 100000 >=1) &&(BOOST_VERSION / 100 % 1000<56)
	boost::property_tree::xml_writer_settings<char> settings('\t', 1);
#else
	boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
#endif

	try
	{
		if (aPretty)
			boost::property_tree::xml_parser::write_xml(aStream, _tree,
					settings);
		else
			boost::property_tree::xml_parser::write_xml(aStream, _tree);
	} catch (boost::property_tree::xml_parser_error &e)
	{
		LOG(ERROR)<<"Cannot write xml "<<e.message()<<" "<<e.filename()<<":"<<e.line();
		return false;
	}
	return true;
}
bool CConfig::MFromXML(std::istream& aStream)
{
	VLOG(2) << "Read from xml";
	ptree_t _tree;
	try
	{
		boost::property_tree::xml_parser::read_xml(aStream, _tree);
		MReadFrom(_tree);

	} catch (boost::property_tree::xml_parser_error &e)
	{
		LOG(ERROR)<<"Cannot read xml "<<e.message()<<" "<<e.filename()<<":"<<e.line();
		(void) e;
		return false;
	}
	return true;
}
size_t base64_encode_len(size_t len)
{
	return ((len + 2) / 3 * 4) + 1;
}

//from StackOverflow
size_t base64_encode(NSHARE::CText& _dest, const char* src, size_t aLen)
{
	using namespace boost::archive::iterators;
	char tail[3] =
	{ 0, 0, 0 };
	typedef base64_from_binary<transform_width<const char *, 6, 8> > base64_enc;

	size_t one_third_len = aLen / 3;
	size_t len_rounded_down = one_third_len * 3;
	size_t j = len_rounded_down + one_third_len;

	_dest.reserve(base64_encode_len(aLen));

	base64_enc _begin(src);
	base64_enc _end(src + len_rounded_down);
	for (; _begin != _end; ++_begin)
	{
		_dest.push_back(*_begin);
	}
	if (len_rounded_down != aLen)
	{
		size_t i = 0;
		for (; i < aLen - len_rounded_down; ++i)
		{
			tail[i] = src[len_rounded_down + i];
		}

		base64_enc _begin(tail);
		base64_enc _end(tail + 3);
		for (; _begin != _end; ++_begin)
		{
			_dest.push_back(*_begin);
		}
		for (i = aLen + one_third_len + 1; i < j + 4; ++i)
		{
			_dest[i] = '=';
		}

		return i;
	}

	return j;
}
const char* base64_decode(char* dest, const char* src, size_t* len)
{
	using namespace boost::archive::iterators;
	size_t output_len = *len;

	typedef transform_width<binary_from_base64<const char*>, 8, 6> base64_dec;

	size_t i = 0;
	try
	{
		base64_dec src_it(src);
		for (; i < output_len; ++i)
		{
			*dest++ = *src_it;
			++src_it;
		}
	} catch (dataflow_exception const& e)
	{
		VLOG(1) << "Overflow error:" << e.what();
	}

	*len = i;
	return src + (i + 2) / 3 * 4; // bytes in = bytes out / 3 rounded up * 4
}
void base64_decode(CBuffer & aTo, NSHARE::CText const& aFrom)
{
	using namespace boost::archive::iterators;
	typedef transform_width<binary_from_base64<const uint8_t*>, 8, 6> base64_dec;

	aTo.reserve(aFrom.length_code() / 2);
	CText::size_type const _len = aFrom.find_last_not_of('=');
	CHECK_NE(_len, CText::npos);
	try
	{
		base64_dec src_it(aFrom.c_str()), _it_end(aFrom.c_str() + _len);
		for (; src_it != _it_end; ++src_it)
		{
			uint8_t _val = *src_it;
			aTo.push_back(_val);
		}
	} catch (dataflow_exception const& e)
	{
		LOG(ERROR)<<"Overflow error:"<<e.what();
	}

}
void CConfig::MValue(CBuffer & aTo) const
{
	base64_decode(aTo, MValue());
}

size_t CConfig::MValueBuf(size_t aMaxLen, void* aTo) const
{
	base64_decode((char*) aTo, MValue().c_str(), &aMaxLen);
	VLOG(2) << "Buf length=" << aMaxLen;
	return aMaxLen;
}
CConfig& CConfig::MAdd(const CText& key, void const* aTo, size_t aMaxLen)
{
	FData.MWrite().FChildren.push_back(CConfig(key));
	CConfig& _new = FData.MWrite().FChildren.back();
	base64_encode(_new.FData.MWrite().FValue, (char const*) aTo, aMaxLen);
	VLOG(6) << "Data string for " << key << " is " << _new.MValue();
	VLOG(7)<<(*this);
	return *this;
}
CConfig& CConfig::MAddTo(const CText& key, CBuffer const & aTo)
{
	return MAdd(key, aTo.ptr_const(), aTo.size());
}
template<>
CBuffer CConfig::MValue<CBuffer>(CBuffer _val) const
{
	MValue(_val);
	return _val;
}
bool CConfig::sMUnitTest()
{
	COMPILE_ASSERT(impl::cconfig::
			   deserialize_check<CProgramName>::result
			   == sizeof(impl::cconfig::is_method_t),NotValidDeductName);
	COMPILE_ASSERT(impl::cconfig::
			   deserialize_check<int>::result
			   == sizeof(impl::cconfig::nobody_t),
			   NotValidDeduct);
	{
		CConfig _conf(impl::cconfig::ser_t<int>::serialize(5));

		int const _val = impl::cconfig::
				der_t<int>::deserialize(_conf);
		CHECK_EQ(_val, 5);


	}
	{
		CConfig _conf("test");
		CConfig _copy(_conf);

		CHECK(_copy.MIsValid());
		CHECK(_conf.MIsValid());

		CHECK(!_conf.FData.MIsOne());

		_copy.MAdd(_conf);
		CHECK(!_conf.FData.MIsOne());
		CHECK(_copy.FData.MIsOne());

		CHECK(_copy.MIsValid());
		CHECK(_conf.MIsValid());
	}
	{
		CText const _test_str=
				"{\"test\":{\"num\":\"10\","
				"\"test_0\":{\"double\":\"1.7976e+308\",\"float\":\"3.4028e+38\",\"uint64\":\"0\",\"int64\":\"0\",\"buf\":{}},"
				"\"test_1\":{\"double\":\"9.0506e+263\",\"float\":\"2.0907e+33\",\"uint64\":\"1844674407370955161\",\"int64\":\"922337203685477580\",\"buf\":\"AA==\"},"
				"\"test_2\":{\"double\":\"4.5566e+219\",\"float\":\"1.2846e+28\",\"uint64\":\"3689348814741910322\",\"int64\":\"1844674407370955160\",\"buf\":\"AAE=\"},"
				"\"test_3\":{\"double\":\"2.294e+175\",\"float\":\"7.8929e+22\",\"uint64\":\"5534023222112865483\",\"int64\":\"2767011611056432740\",\"buf\":\"AAEC\"},"
				"\"test_4\":{\"double\":\"1.1549e+131\",\"float\":\"4.8495e+17\",\"uint64\":\"7378697629483820644\",\"int64\":\"3689348814741910320\",\"buf\":\"AAECAw==\"},"
				"\"test_5\":{\"double\":\"5.8147e+86\",\"float\":\"2.9796e+12\",\"uint64\":\"9223372036854775805\",\"int64\":\"4611686018427387900\",\"buf\":\"AAECAwQ=\"},"
				"\"test_6\":{\"double\":\"2.9274e+42\",\"float\":\"1.8307e+7\",\"uint64\":\"11068046444225730966\",\"int64\":\"5534023222112865480\",\"buf\":\"AAECAwQF\"},"
				"\"test_7\":{\"double\":\"0\",\"float\":\"112.4871\",\"uint64\":\"12912720851596686127\",\"int64\":\"6456360425798343060\",\"buf\":\"AAECAwQFBg==\"},"
				"\"test_8\":{\"double\":\"0\",\"float\":\"0\",\"uint64\":\"14757395258967641288\",\"int64\":\"7378697629483820640\",\"buf\":\"AAECAwQFBgc=\"},"
				"\"test_9\":{\"double\":\"0\",\"float\":\"0\",\"uint64\":\"16602069666338596449\",\"int64\":\"8301034833169298220\",\"buf\":\"AAECAwQFBgcI\"}"
				"}}";

		CConfig _ser_data;
		bool const _is=_ser_data.MFromJSON(_test_str);
		CConfig _probe=_ser_data.MChild("test");
		CHECK(_probe.MIsValid());
		VLOG(6)<<_probe.MToJSON(true);

		CConfig _test("test");
		size_t const _number=_probe.MValue("num",0);
		_test.MAdd("num",_number);

		for(size_t i=0;i<_number;++i)
		{
			CConfig _conf(CText::sMPrintf(CCodeUTF8(), "test_%d",i));

			const double _double=std::numeric_limits< double>::max()/ exp( (double)(std::numeric_limits< double>::max_exponent/_number*i));
			CText _tmp;
			float_to_str(_double,_tmp,4);
			_conf.MAdd("double",_tmp);

			const float _float=std::numeric_limits< float>::max()/ expf( std::numeric_limits< float>::max_exponent/_number*(float)i);
			_tmp.clear();
			float_to_str(_float,_tmp,4);
			_conf.MAdd("float",_tmp);

			const uint64_t _64=std::numeric_limits< uint64_t>::max()/_number*i;
			_conf.MAdd("uint64",_64);

			const int64_t _i64=std::numeric_limits< int64_t>::max()/_number*i;
			_conf.MAdd("int64",_i64);

			CBuffer _buf(i);
			{///< Filling message
				NSHARE::CBuffer::iterator _it=_buf.begin(),_it_end=_buf.end();
				for(unsigned j=0;_it!=_it_end;++j,++_it)
				{
					*_it=j%255;
				}
			}

			_conf.MAdd("buf",_buf);

			_test.MAdd(_conf);
		}
		NSHARE::CText const _result(_test.MToJSON(false));
		CHECK_EQ(_test_str,_result);
	}
	return true;
}
} /* namespace NSHARE */
