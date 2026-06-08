/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa client command line options parser.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "opcua_options.h"

#include <opc/ua/protocol/string_utils.h>

#include <iostream>
#include <map>

#ifndef CONFIG_PATH
#define CONFIG_PATH "/etc/opcua/client"
#endif

using namespace std;

namespace OpcUa
{

struct Args {
	struct Option {
		int pos = -1;
		string name;
		string shortname;
		string desc;
		
		bool expects_value = false;
        	string value;
		
		Option(string n, string d = "", int p = -1) : pos(p), desc(d) {
			auto cp = n.find(',');
			if (cp != std::string::npos) {
				name = n.substr(0, cp);
				shortname = n.substr(cp + 1);
			} else name = n;
		}
	};
	
	using OptionPtr = shared_ptr<Option>;

    	map<string, OptionPtr> options;
    	vector<string> positionals;
    
	Args() {}

	void add_option(string name, string desc, int pos = -1) { 
		auto opt = OptionPtr( new Option(name,desc) ); 
		options[opt->name] = opt;
		if (opt->shortname.size() > 0) options[opt->shortname] = opt;
	}
	
	int count(string name) const {
		auto it = options.find(name);
		return (it != options.end() && !it->second->value.empty()) ? 1 : 0;
	}
	
	string get(string name) const {
		auto it = options.find(name);
		if (it != options.end()) return it->second->value;
		return "";
	}
	
	template<class T>
	T convert(string name) const {
		string v;
		auto it = options.find(name);
		if (it != options.end()) v = it->second->value;
		
		std::stringstream ss(v);

		if constexpr (std::is_same_v<T, uint8_t>) {
        		unsigned int tmp;
        		ss >> tmp;
        		return static_cast<uint8_t>(tmp);
		} else {
			T result{};
			ss >> result;
			return result;
		}
	}
	
	void print_options() {
	    cout << endl;
	    map<string, OptionPtr> opts;
	    for (auto& o : options) opts[o.second->name] = o.second;
	    for (auto& o : options) {
	    	auto opt = o.second;
	    	cout << opt->name;
	    	if (opt->shortname.size() > 0) cout << "," << opt->shortname;
	    	cout << endl << opt->desc << endl << endl;
	    }
	    cout << endl;
	}
	
	void parse(int argc, char** argv) {
		auto match_option = [&](const std::string& key) -> OptionPtr* {
			auto it = options.find(key);
			if (it != options.end()) return &it->second;
			return nullptr;
		};

		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];

			if (arg.size() > 1 && arg[0] == '-') {
				std::string key = arg;

				// strip "--"
				if (arg.rfind("--", 0) == 0) key = arg.substr(2);
				else key = arg.substr(1);

				auto opt_ptr = match_option(key);
				if (!opt_ptr || !*opt_ptr) continue;
				auto& opt = **opt_ptr;

				// assume: if next arg exists and is not option → value
				if (i + 1 < argc) {
					std::string next = argv[i + 1];

					if (next.empty() || next[0] != '-') {
						opt.value = next;
						opt.expects_value = true;
						++i;
					} else opt.value = "true"; // flag
				} else opt.value = "true";
			}

			else positionals.push_back(arg);
		}
	}
};

const char * OPTION_HELP = "help";
const char * OPTION_GET_ENDPOINTS = "get-endpoints";
const char * OPTION_BROWSE = "browse";
const char * OPTION_READ = "read";
const char * OPTION_WRITE = "write";
const char * OPTION_CREATE_SUBSCRIPTION = "create-subscription";
const char * OPTION_FIND_ServerS = "find-servers";
const char * OPTION_REGISTER_MODULE = "register-module";
const char * OPTION_UNREGISTER_MODULE = "unregister-module";

const char * OPTION_MODULE_Id = "id";
const char * OPTION_MODULE_PATH = "path";
const char * OPTION_CONFIG_DIR = "config-dir";

const char * OPTION_Server_URI = "uri";
const char * OPTION_ATTRIBUTE = "attribute";
const char * OPTION_NODE_Id = "node-id";


const char * OPTION_VALUE_BYTE  = "value-byte";
const char * OPTION_VALUE_SBYTE = "value-sbyte";
const char * OPTION_VALUE_UINT16 = "value-uint16";
const char * OPTION_VALUE_INT16 = "value-int16";
const char * OPTION_VALUE_UINT32 = "value-uint32";
const char * OPTION_VALUE_INT32 = "value-int32";
const char * OPTION_VALUE_UINT64 = "value-uint64";
const char * OPTION_VALUE_INT64 = "value-int64";
const char * OPTION_VALUE_FLOAT = "value-float";
const char * OPTION_VALUE_DOUBLE = "value-double";
const char * OPTION_VALUE_STRING = "value-string";

// codegen
#include "opcua_options_attribute_ids.h"

NodeId GetNodeIdOptionValue(const Args & args)
{
  const std::string & value = args.get(OPTION_NODE_Id);
  return OpcUa::ToNodeId(value);
}

Variant GetOptionValue(const Args & args)
{
  if (args.count(OPTION_VALUE_BYTE))
    {
      return Variant(args.convert<uint8_t>(OPTION_VALUE_BYTE));
    }

  if (args.count(OPTION_VALUE_SBYTE))
    {
      return Variant(args.convert<uint8_t>(OPTION_VALUE_SBYTE));
    }

  if (args.count(OPTION_VALUE_UINT16))
    {
      return Variant(args.convert<uint16_t>(OPTION_VALUE_UINT16));
    }

  if (args.count(OPTION_VALUE_INT16))
    {
      return Variant(args.convert<int16_t>(OPTION_VALUE_INT16));
    }

  if (args.count(OPTION_VALUE_UINT32))
    {
      return Variant(args.convert<uint32_t>(OPTION_VALUE_UINT32));
    }

  if (args.count(OPTION_VALUE_INT32))
    {
      return Variant(args.convert<int32_t>(OPTION_VALUE_INT32));
    }

  if (args.count(OPTION_VALUE_UINT64))
    {
      return Variant(args.convert<uint64_t>(OPTION_VALUE_UINT64));
    }

  if (args.count(OPTION_VALUE_INT64))
    {
      return Variant(args.convert<int64_t>(OPTION_VALUE_INT64));
    }

  if (args.count(OPTION_VALUE_FLOAT))
    {
      return Variant(args.convert<float>(OPTION_VALUE_FLOAT));
    }

  if (args.count(OPTION_VALUE_DOUBLE))
    {
      return Variant(args.convert<double>(OPTION_VALUE_DOUBLE));
    }

  if (args.count(OPTION_VALUE_STRING))
    {
      return Variant(args.get(OPTION_VALUE_STRING));
    }

  return Variant();
}

CommandLine::CommandLine(int argc, char ** argv)
  : NamespaceIndex(0)
  , Attribute(AttributeId::Unknown)
  , IsHelp(false)
  , IsGetEndpoints(false)
  , IsBrowse(false)
  , IsRead(false)
  , IsWrite(false)
  , IsCreateSubscription(false)
  , IsFindServers(false)
  , IsAddModule(false)
  , IsRemoveModule(false)
{
  // Declare the supported options.
  Args args;
  
  args.add_option(OPTION_HELP, "produce help message");
  args.add_option(OPTION_GET_ENDPOINTS, "List endpoints endpoints.");
  args.add_option(OPTION_BROWSE, "browse command.");
  args.add_option(OPTION_READ, "read command.");
  args.add_option(OPTION_WRITE, "write command.");
  args.add_option(OPTION_CREATE_SUBSCRIPTION, "create subscription command.");
  args.add_option(OPTION_FIND_ServerS, "find servers command.");
  args.add_option(OPTION_REGISTER_MODULE, "Register new module.");
  args.add_option(OPTION_UNREGISTER_MODULE, "Unregister module.");

  args.add_option(OPTION_Server_URI, "Uri of the server.");
  args.add_option(OPTION_ATTRIBUTE, "Name of attribute.");
  args.add_option(OPTION_NODE_Id, "NodeId in the form 'nsu=uri;srv=1;ns=0;i=84.");
  args.add_option(OPTION_VALUE_BYTE, "Byte value.");
  args.add_option(OPTION_VALUE_SBYTE, "Signed byte value.");
  args.add_option(OPTION_VALUE_UINT16, "UInt16 value.");
  args.add_option(OPTION_VALUE_INT16, "Int16 value.");
  args.add_option(OPTION_VALUE_UINT32, "UInt32 value.");
  args.add_option(OPTION_VALUE_INT32, "Int32 value.");
  args.add_option(OPTION_VALUE_UINT64, "UInt64 value.");
  args.add_option(OPTION_VALUE_INT64, "Int64 value.");
  args.add_option(OPTION_VALUE_FLOAT, "Float value.");
  args.add_option(OPTION_VALUE_DOUBLE, "Double value.");
  args.add_option(OPTION_VALUE_STRING, "String value.");
  args.add_option(OPTION_MODULE_Id, "Id of the new module.");
  args.add_option(OPTION_MODULE_PATH, "Path to the new module shared library.");
  args.add_option(OPTION_CONFIG_DIR, "Path to the directory with modules configuration files. By default '" CONFIG_PATH "'.");

  args.parse(argc, argv);

  if (args.count(OPTION_HELP))
    {
      IsHelp = true;
      args.print_options();
      return;
    }

  if (args.count(OPTION_Server_URI))
    {
      ServerURI = args.get(OPTION_Server_URI);
    }


  if (args.count(OPTION_NODE_Id))
    {
      Node = GetNodeIdOptionValue(args);
    }

  if (args.count(OPTION_ATTRIBUTE))
    {
      Attribute = GetAttributeIdOptionValue(args);
    }

  Value = GetOptionValue(args);
  IsGetEndpoints = args.count(OPTION_GET_ENDPOINTS) != 0;
  IsBrowse = args.count(OPTION_BROWSE) != 0;
  IsRead = args.count(OPTION_READ) != 0;
  IsWrite = args.count(OPTION_WRITE) != 0;
  IsCreateSubscription = args.count(OPTION_CREATE_SUBSCRIPTION) != 0;
  IsFindServers = args.count(OPTION_FIND_ServerS) != 0;

  if (args.count(OPTION_REGISTER_MODULE))
    {
      IsAddModule = true;
      ModulePath = args.get(OPTION_MODULE_PATH);
      ModuleId = args.get(OPTION_MODULE_Id);
    }

  if (args.count(OPTION_UNREGISTER_MODULE))
    {
      IsRemoveModule = true;
      ModuleId = args.get(OPTION_MODULE_Id);
    }

  if (args.count(OPTION_CONFIG_DIR))
    {
      ConfigDir = args.get(OPTION_CONFIG_DIR);
    }

  else
    {
      ConfigDir = CONFIG_PATH;
    }
}
}
