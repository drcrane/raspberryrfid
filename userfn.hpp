#ifndef __USERFN_HPP
#define __USERFN_HPP

#include "sqlite3.h"

class UserService;

class User {
	private:
		unsigned long long int id;
		char* friendlyName;
		bool jpegBufferLoaded;
		unsigned char* jpegBuffer;
		unsigned long int jpegBufferSize;
		UserService* userService;

	public:
		User(unsigned long long int id, char* friendlyName, unsigned char* jpegBuffer, unsigned long int jpegBufferSize);
		~User();
		unsigned long long int GetId() { return this->id; }
		char* GetFriendlyName() { return this->friendlyName; }
		unsigned char* GetJpegBuffer();
		unsigned long int GetJpegBufferSize();
};

class UserService {
	private:
		sqlite3* db;
		sqlite3_stmt* getuser_stmt;

	public:
		UserService(sqlite3 *db);
		int Add(User* user);
		User* GetById(unsigned long long int userid);
		int DeleteById(unsigned long long int userid);
};

#endif // __USERFN_HPP
