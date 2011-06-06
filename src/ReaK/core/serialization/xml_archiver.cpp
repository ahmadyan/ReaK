
/*
 *    Copyright 2011 Sven Mikael Persson
 *
 *    THIS SOFTWARE IS DISTRIBUTED UNDER THE TERMS OF THE GNU GENERAL PUBLIC LICENSE v3 (GPLv3).
 *
 *    This file is part of ReaK.
 *
 *    ReaK is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    ReaK is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with ReaK (as LICENSE in the root folder).  
 *    If not, see <http://www.gnu.org/licenses/>.
 */

#include "xml_archiver.hpp"

#include <iostream>
#include <iterator>

#include "base/shared_object.hpp"

#include <cstdlib>

using namespace std;
using namespace boost;

namespace ReaK {

namespace serialization {


char xml_iarchive::getNextChar() {
  char c;
  file_stream.get(c);
  while((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
    file_stream.get(c);
  return c;
};

string xml_iarchive::readToken() {
  string result;
  char c = getNextChar();
  if(c != '<')
    return result;
  c = getNextChar();
  if((c == '!') || (c == '?')) {
    char line_str[512];
    file_stream.getline(line_str,512);
    return readToken();
  };
  while(c != '>') {
    result += c;
    file_stream.get(c);
  };
  return result;
};


void xml_iarchive::skipToEndToken(const string& name) {
  string token = readToken();
  trimStr(token);
  while(token != "/" + name) {
    token = readToken();
    trimStr(token);
  };
};

void xml_iarchive::trimStr(string& s) {
  unsigned int i=0;
  for(;((i < s.size()) && ((s[i] == ' ') || (s[i] == '\t') || (s[i] == '\n') || (s[i] == '\r')));++i) ;
  string result;
  for(;((i < s.size()) && (s[i] != ' ') && (s[i] != '\t') && (s[i] != '\n') && (s[i] != '\r'));++i)
    result += s[i];
  s = result;
};

bool xml_iarchive::readNamedValue(const string& value_name,string& value_str) {
  string token = readToken();
  trimStr(token);
  if((value_name.empty()) || (token != value_name))
    return false;

  char c;
  file_stream.get(c);
  while(c != '\"')
    file_stream.get(c);

  value_str.clear();
  file_stream.get(c);
  while(c != '\"') {
    value_str += c;
    file_stream.get(c);
  };

  token = readToken();
  unsigned int i=0;
  for(;((i<token.size()) && (token[i] != '/'));++i) ;
  string tmp;
  ++i;
  for(;((i<token.size()) && (token[i] != value_name[0]));++i) ;
  for(;((i<token.size()) && (tmp.size() < value_name.size()));++i)
    tmp += token[i];
  if(tmp != value_name)
    return false;
  return true;
};

archive_object_header xml_iarchive::readHeader(const string& obj_name) {
  archive_object_header result;

  string token = readToken();
  if(token.empty())
    return result;

  string name;
  unsigned int i=0;
  for(;((i < token.size()) && (token[i] == ' '));++i) ;
  for(;((i < token.size()) && (token[i] != ' '));++i)
    name += token[i];

  if((name != obj_name) || (i == token.size()))
    return result;

  map<string,string> values;
  while(i < token.size()) {
    for(;((i < token.size()) && ((token[i] == ' ') || (token[i] == '\t') || (token[i] == '\n') || (token[i] == '\r')));++i) ;
    string value_key;
    for(;((i < token.size()) && (token[i] != ' ') && (token[i] != '='));++i)
      value_key += token[i];
    string value_str;
    for(;((i < token.size()) && (token[i] != '\"'));++i) ;
    ++i;
    for(;((i < token.size()) && (token[i] != '\"'));++i)
      value_str += token[i];
    ++i;
    values[value_key] = value_str;
  };

  std::string IDstr = values["type_ID"];
  if(IDstr.empty())
    result.type_ID = NULL;
  else {
    std::vector<unsigned int> nums;
    for(i=0;i<IDstr.size();++i) {
      std::string numstr;
      for(;((i < IDstr.size()) && (IDstr[i] != '.'));++i) 
	numstr += IDstr[i];
      nums.push_back(strtoul(numstr.c_str(),NULL,0));
    };
    result.type_ID = new unsigned int[nums.size()];
    std::copy(nums.begin(),nums.end(),result.type_ID);
  };

  if(values["version"].empty())
    result.type_version = 0;
  else
    result.type_version = strtoul(values["version"].c_str(),NULL,0);

  if(values["object_ID"].empty())
    result.object_ID = 0;
  else
    result.object_ID = strtoul(values["object_ID"].c_str(),NULL,0);

  if(values["is_external"].empty())
    result.is_external = false;
  else if(values["is_external"] == "true")
    result.is_external = true;
  else
    result.is_external = false;

  return result;
};

xml_iarchive::xml_iarchive(const string& FileName) {
  file_stream.open(FileName.c_str());

  archive_object_header global_hdr = readHeader("reak_serialization");
  if(global_hdr.type_version != 2)
    throw ios_base::failure("ReaK XML Archive is of an unknown version!");

};

xml_iarchive::~xml_iarchive() {
  file_stream.close();
};


iarchive& RK_CALL xml_iarchive::load_serializable_ptr(shared_ptr<serializable>& Item) {
  return xml_iarchive::load_serializable_ptr(pair<string, shared_ptr<serializable>& >("Item",Item));
};

iarchive& RK_CALL xml_iarchive::load_serializable_ptr(const pair<string, shared_ptr<serializable>& >& Item) {
  archive_object_header hdr;
  Item.second = shared_ptr<serializable>();
  
  hdr = readHeader(Item.first);
  if((hdr.type_ID == NULL) || (hdr.type_version == 0)) {
    skipToEndToken(Item.first);
    delete[] hdr.type_ID;
    return *this;
  };

  if((hdr.object_ID < mObjRegistry.size()) && (mObjRegistry[hdr.object_ID])) {
    Item.second = mObjRegistry[hdr.object_ID];
    skipToEndToken(Item.first);
    delete[] hdr.type_ID;
    return *this;
  };

  if(hdr.is_external) {
    string ext_filename;
    xml_iarchive::load_string(pair<string, string& >("filename", ext_filename));

    skipToEndToken(Item.first);

    try {
      xml_iarchive a(ext_filename);
      a & Item;
    } catch(std::ios_base::failure e) {
      RK_ERROR("Could not load object: " << Item.first << " - failed with message: " << e.what());
      Item.second = shared_ptr<serializable>();
    };

    delete[] hdr.type_ID;
    return *this;
  };

  //Find the class in question in the repository.
  weak_ptr<rtti::so_type> p( rtti::so_type_repo::getInstance().findType(hdr.type_ID) );
  delete[] hdr.type_ID;
  if((p.expired()) || (p.lock()->TypeVersion() < hdr.type_version)) {
    skipToEndToken(Item.first);
    return *this;
  };
  shared_ptr<shared_object> po(p.lock()->CreateObject());
  if(!po) {
    skipToEndToken(Item.first);
    return *this;
  };

  Item.second = po;
  if(hdr.object_ID < mObjRegistry.size())
    mObjRegistry[hdr.object_ID] = Item.second;
  else if(hdr.object_ID == mObjRegistry.size())
    mObjRegistry.push_back(Item.second);                //in theory, only this condition should occur
  else if(hdr.object_ID > mObjRegistry.size()) {
    mObjRegistry.resize(hdr.object_ID + 1);
    mObjRegistry[hdr.object_ID] = Item.second;
  };

  Item.second->load(*this,hdr.type_version);

  skipToEndToken(Item.first);
  return *this;
};

iarchive& RK_CALL xml_iarchive::load_serializable(serializable& Item) {
  return xml_iarchive::load_serializable(pair<string, serializable&>("Item",Item));
};

iarchive& RK_CALL xml_iarchive::load_serializable(const pair<std::string, serializable& >& Item) {
  archive_object_header hdr;

  hdr = readHeader(Item.first);
  if((hdr.type_ID == NULL) || (hdr.type_version == 0)) {
    skipToEndToken(Item.first);
    return *this;
  };

  Item.second.load(*this,hdr.type_version);

  skipToEndToken(Item.first);
  delete[] hdr.type_ID;
  return *this;
};

iarchive& RK_CALL xml_iarchive::load_int(int& i) {
  return xml_iarchive::load_int(pair<string, int& >("int",i));
};

iarchive& RK_CALL xml_iarchive::load_int(const pair<string, int& >& i) {
  string value_str;
  if(readNamedValue(i.first,value_str)) {
    if(value_str.empty())
      i.second = 0;
    else
      i.second = atoi(value_str.c_str());
  } else
    i.second = 0;
  return *this;
};

iarchive& RK_CALL xml_iarchive::load_unsigned_int(unsigned int& u) {
  return xml_iarchive::load_unsigned_int(pair<string, unsigned int& >("unsigned int",u));
};

iarchive& RK_CALL xml_iarchive::load_unsigned_int(const pair<string, unsigned int& >& u) {
  string value_str;
  if(readNamedValue(u.first,value_str)) {
    if(value_str.empty())
      u.second = 0;
    else
      u.second = atoi(value_str.c_str());
  } else
    u.second = 0;
  return *this;
};

iarchive& RK_CALL xml_iarchive::load_float(float& f) {
  return xml_iarchive::load_float(pair<string, float& >("real",f));
};

iarchive& RK_CALL xml_iarchive::load_float(const pair<string, float& >& f) {
  string value_str;
  if(readNamedValue(f.first,value_str)) {
    if(value_str.empty())
      f.second = 0;
    else
      f.second = atof(value_str.c_str());
  } else
    f.second = 0;
  return *this;
};

iarchive& RK_CALL xml_iarchive::load_double(double& d) {
  return xml_iarchive::load_double(pair<string, double& >("real",d));
};

iarchive& RK_CALL xml_iarchive::load_double(const pair<string, double& >& d) {
  string value_str;
  if(readNamedValue(d.first,value_str)) {
    if(value_str.empty())
      d.second = 0;
    else
      d.second = atof(value_str.c_str());
  } else
    d.second = 0;
  return *this;
};

iarchive& RK_CALL xml_iarchive::load_bool(bool& b) {
  return xml_iarchive::load_bool(pair<string, bool& >("bool",b));
};

iarchive& RK_CALL xml_iarchive::load_bool(const pair<string, bool& >& b) {
  string value_str;
  if(readNamedValue(b.first,value_str)) {
    if(value_str.empty())
      b.second = false;
    else if(value_str == "true")
      b.second = true;
    else
      b.second = false;
  } else
    b.second = false;
  return *this;
};

iarchive& RK_CALL xml_iarchive::load_string(string& s) {
  return xml_iarchive::load_string(pair<string, string& >("string",s));
};

iarchive& RK_CALL xml_iarchive::load_string(const pair<string, string& >& s) {
  readNamedValue(s.first,s.second);
  return *this;
};











xml_oarchive::xml_oarchive(const string& FileName) {
  file_stream.open(FileName.c_str());

  file_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>" << std::endl;
  file_stream << "<!DOCTYPE reak_serialization>" << std::endl;
  file_stream << "<reak_serialization version=\"2\">" << std::endl;
  tabulation = 0;
};

xml_oarchive::~xml_oarchive() {

  file_stream << "</reak_serialization>" << std::endl;

  file_stream.close();
};

void xml_oarchive::addTabulations() {
  for(unsigned int i=0;i<tabulation;++i) {
    file_stream << "    ";
  };
};


oarchive& RK_CALL xml_oarchive::saveToNewArchive_impl(const boost::shared_ptr<serializable>& Item, const std::string& FileName) {
  return xml_oarchive::saveToNewArchiveNamed_impl(std::pair<std::string, const boost::shared_ptr<serializable>& >("Item",Item),FileName);
};

oarchive& RK_CALL xml_oarchive::saveToNewArchiveNamed_impl(const pair<std::string, const boost::shared_ptr<serializable>& >& Item, const std::string& FileName) {
  archive_object_header hdr;
  bool already_saved(false);

  if(Item.second) {
    map< shared_ptr<serializable>, unsigned int>::const_iterator it = mObjRegMap.find(Item.second);

    if(it != mObjRegMap.end()) {
      hdr.object_ID = it->second;
      already_saved = true;
    } else {
      hdr.object_ID = mObjRegistry.size();
      mObjRegistry.push_back(Item.second);
      mObjRegMap[Item.second] = hdr.object_ID;
    };

    shared_ptr<rtti::so_type> obj_type = Item.second->getObjectType();
    const unsigned int* type_ID = obj_type->TypeID_begin();
    hdr.type_version = obj_type->TypeVersion();
    hdr.is_external = true;
    hdr.size = 0;

    addTabulations();
    file_stream << "<" << Item.first << " type_ID=\"";
    while(*type_ID) {
      file_stream << *type_ID << ".";
      ++type_ID;
    };
    file_stream << "0\" version=\"" << hdr.type_version
	        << "\" object_ID=\"" << hdr.object_ID
	        << "\" is_external=\"true\">" << endl;
  } else {
    already_saved = true;

    addTabulations();
    file_stream << "<" << Item.first << " type_ID=\"0\" version=\"0\" object_ID=\"0\" is_external=\"false\">" << endl;
  };

  if(!already_saved) {
    tabulation++;
    addTabulations();
    file_stream << "<filename>\"" << FileName << "\"</filename>" << endl;
    tabulation--;

    xml_oarchive a(FileName);
    a & Item;
  };

  addTabulations();
  file_stream << "</" << Item.first << ">" << endl;
  return *this;
};


oarchive& RK_CALL xml_oarchive::save_serializable_ptr(const shared_ptr<serializable>& Item) {
  return *this & pair<string, const shared_ptr<serializable>& >("Item",Item);
};


oarchive& RK_CALL xml_oarchive::save_serializable_ptr(const pair<string, const shared_ptr<serializable>& >& Item) {

  archive_object_header hdr;
  bool already_saved(false);

  if(Item.second) {
    map< shared_ptr<serializable>, unsigned int>::const_iterator it = mObjRegMap.find(Item.second);

    if(it != mObjRegMap.end()) {
      hdr.object_ID = it->second;
      already_saved = true;
    } else {
      hdr.object_ID = mObjRegistry.size();
      mObjRegistry.push_back(Item.second);
      mObjRegMap[Item.second] = hdr.object_ID;
    };

    shared_ptr<rtti::so_type> obj_type = Item.second->getObjectType();
    const unsigned int* type_ID = obj_type->TypeID_begin();
    hdr.type_version = obj_type->TypeVersion();
    hdr.is_external = false;
    hdr.size = 0;

    addTabulations();
    file_stream << "<" << Item.first << " type_ID=\"";
    while(*type_ID) {
      file_stream << *type_ID << "."; 
      ++type_ID;
    };
    file_stream << "0\" version=\"" << hdr.type_version
	        << "\" object_ID=\"" << hdr.object_ID
	        << "\" is_external=\"false\">" << endl;
  } else {
    already_saved = true;

    addTabulations();
    file_stream << "<" << Item.first << " type_ID=\"0\" version=\"0\" object_ID=\"0\" is_external=\"false\">" << endl;
  };

  if(!already_saved) {

    tabulation++;
    Item.second->save(*this,hdr.type_version);
    tabulation--;

  };

  addTabulations();
  file_stream << "</" << Item.first << ">" << endl;
  return *this;
};


oarchive& RK_CALL xml_oarchive::save_serializable(const serializable& Item) {
  return *this & pair<string, const serializable& >("Item",Item);
};

oarchive& RK_CALL xml_oarchive::save_serializable(const pair<std::string, const serializable& >& Item) {
  archive_object_header hdr;
  const unsigned int* type_ID = Item.second.getObjectType()->TypeID_begin();
  hdr.type_version = Item.second.getObjectType()->TypeVersion();
  hdr.object_ID = 0;
  hdr.size = 0;
  hdr.is_external = false;

  addTabulations();
  file_stream << "<" << Item.first << " type_ID=\""; 
  while(*type_ID) {
    file_stream << *type_ID << ".";
    ++type_ID;
  };
  file_stream << "0\" version=\"" << hdr.type_version << "\">" << endl;

  tabulation++;
  Item.second.save(*this,hdr.type_version);
  tabulation--;

  addTabulations();
  file_stream << "</" << Item.first << ">" << endl;
  return *this;
};


oarchive& RK_CALL xml_oarchive::save_int(int i) {
  return xml_oarchive::save_int(pair<string, int >("int",i));
};


oarchive& RK_CALL xml_oarchive::save_int(const pair<string, int >& i) {
  addTabulations();
  file_stream << "<" << i.first << ">\"" << i.second << "\"</" << i.first << ">" << endl;
  return *this;
};

oarchive& RK_CALL xml_oarchive::save_unsigned_int(unsigned int u) {
  return xml_oarchive::save_unsigned_int(pair<string, unsigned int >("unsigned int",u));
};


oarchive& RK_CALL xml_oarchive::save_unsigned_int(const pair<string, unsigned int >& u) {
  addTabulations();
  file_stream << "<" << u.first << ">\"" << u.second << "\"</" << u.first << ">" << endl;
  return *this;
};


oarchive& RK_CALL xml_oarchive::save_float(float f) {
  return xml_oarchive::save_float(pair<string, float >("real",f));
};


oarchive& RK_CALL xml_oarchive::save_float(const pair<string, float >& f) {
  addTabulations();
  file_stream << "<" << f.first << ">\"" << f.second << "\"</" << f.first << ">" << endl;
  return *this;
};


oarchive& RK_CALL xml_oarchive::save_double(double d) {
  return xml_oarchive::save_double(pair<string, double >("real",d));
};


oarchive& RK_CALL xml_oarchive::save_double(const pair<string, double >& d) {
  addTabulations();
  file_stream << "<" << d.first << ">\"" << d.second << "\"</" << d.first << ">" << endl;
  return *this;
};


oarchive& RK_CALL xml_oarchive::save_bool(bool b) {
  return xml_oarchive::save_bool(pair<string, bool >("bool",b));
};


oarchive& RK_CALL xml_oarchive::save_bool(const pair<string, bool >& b) {
  addTabulations();
  file_stream << "<" << b.first << ">\"" << (b.second ? "true" : "false") << "\"</" << b.first << ">" << endl;
  return *this;
};


oarchive& RK_CALL xml_oarchive::save_string(const string& s) {
  return xml_oarchive::save_string(pair<string, const string& >("string",s));
};


oarchive& RK_CALL xml_oarchive::save_string(const pair<string, const string& >& s) {
  addTabulations();
  file_stream << "<" << s.first << ">\"" << s.second << "\"</" << s.first << ">" << endl;
  return *this;
};



}; //serialization


}; //ReaK
